/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/

 /* $Id$ */

#include <dirent.h>

#include "submit-postgres.hpp"
#include "Download.hpp"
#include "DownloadBuffer.hpp"
#include "Utilities.hpp"
#include "SubmitManager.hpp"
#include "LogManager.hpp"
#include "SQLHandler.hpp"
#include "SQLManager.hpp"
#include "SQLResult.hpp"
#include "Config.hpp"


using namespace nepenthes;


/**
 * as we may need a global pointer to our Nepenthes in our modules,
 * and cant access the cores global pointer to nepenthes
 * we have to use a own global pointer to nepenthes per module
 * we need this pointer for logInfo() etc
 */
Nepenthes *g_Nepenthes;
SubmitPostgres *g_SubmitPostgres;

/**
 * Constructor
 * creates a new SubmitPostgres Module, where SubmitPostgres is public Module, public SubmitHanvler
 * - sets the ModuleName
 * - sets the ModuleDescription
 * - sets the SubmitHandlerName
 * - sets the SubmitHandlerDescription
 * - sets the Modules global pointer to the Nepenthes
 * 
 * @param nepenthes pointer to our nepenthes master class
 */
SubmitPostgres::SubmitPostgres(Nepenthes *nepenthes)
{
	m_ModuleName        = "submit-postgres";
	m_ModuleDescription = "submit files to a postgres database";
	m_ModuleRevision    = "$Rev$";
	m_Nepenthes = nepenthes;

	m_SubmitterName = "submit-postgres";
	m_SubmitterDescription = "submit files to a postgres database";

	m_SQLHandler = NULL;

	g_Nepenthes = nepenthes;
	g_SubmitPostgres = this;
}


/**
 * exerything important happens in ::Exit() as we have a return value there
 */
SubmitPostgres::~SubmitPostgres()
{
	if (m_SQLHandler != NULL)
	{
		delete m_SQLHandler;
	}

	while (m_OutstandingQueries.size() > 0)
	{
		delete m_OutstandingQueries.front();
		m_OutstandingQueries.pop_front();
	}
}

/**
 * Module::Init()
 * register the submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a module loading error
 */
bool SubmitPostgres::Init()
{

	// the config
	if ( m_Config == NULL )
	{
		logCrit("I (%s:%i) need a config\n",__FILE__,__LINE__);
		return false;
	}

	try 
	{
		m_Server = m_Config->getValString("submit-postgres.server");
		m_User = m_Config->getValString("submit-postgres.user");
		m_Pass = m_Config->getValString("submit-postgres.pass");
		m_DB = m_Config->getValString("submit-postgres.db");
		m_Options = m_Config->getValString("submit-postgres.options");
	}
	catch (...)
	{
		logCrit("submit-postgres, missing config values\n");
		return false;
	}


	// the spool dir
	try 
	{
		m_SpoolDir = m_Config->getValString("submit-postgres.spooldir");
	}
	catch (...)
	{
		m_SpoolDir = "var/spool/submitpostgres/";
		logWarn("submit-postgres, no spooldir set in config file submit-postgres.conf, using %s as default\n",m_SpoolDir.c_str());
	}

	// verify existance
	struct stat s;
	
	if(stat(m_SpoolDir.c_str(), &s) != 0)
	{
		logCrit("Can not access spooldir %s\n",m_SpoolDir.c_str());
		return false;
	}

/*	// this is useless when changing uid, as the user is changed after this
	// therefore commented code with a FIXME in mind
	if (access(m_SpoolDir.c_str(),R_OK|W_OK) != 0)
	{
		logCrit("read/write on %s failed (%s)\n",m_SpoolDir.c_str(),strerror(errno));
		return false;
	}
*/

	m_ModuleManager = m_Nepenthes->getModuleMgr();

	m_SQLHandler = g_Nepenthes->getSQLMgr()->createSQLHandler("postgres",
															   m_Server,
															   m_User,
															   m_Pass,
															   m_DB,
															   m_Options,
															  this);

	if (m_SQLHandler == NULL)
	{
		logCrit("No postgres sql handler installed, do something\n");
		return false;
	}

	REG_SUBMIT_HANDLER(this);



	

	DIR *dir;
	struct dirent *dent;
	
	if((dir = opendir(m_SpoolDir.c_str())) == NULL)
	{
		logWarn("could not open spool dir\n");
		return true; // for now this is not critical FIXME
	}
	
	while((dent = readdir(dir)) != NULL)
	{
		string filepath = m_SpoolDir + "/" + string(dent->d_name);
		struct stat s;

		logInfo("Checking %s\n",filepath.c_str());
		
		if(stat(filepath.c_str(), &s) != 0)
        	continue;
		
		
		if(S_ISREG(s.st_mode) == 0)
        	continue;
		
		
		PGDownloadContext *ctx = PGDownloadContext::unserialize(filepath.c_str());
		if (ctx == NULL)
			continue;

		string query;
		query = "SELECT mwcollect.sensor_exists_sample('";
		query += ctx->getHashMD5();
		query += "','";
		query += ctx->getHashSHA512();
		query += "')";

		logSpam("Query is %s\n",query.c_str());

		m_SQLHandler->addQuery(&query,this,ctx);

		ctx->setState(PG_SAMPLE_EXISTS);
		m_OutstandingQueries.push_back(ctx);

	}
	closedir(dir);
	return true;
}


/**
 * Module::Exit()
 * 
 * unregister the Submitter
 * 
 * @return return true if everything was fine, else false
 *         false indicates a heavy error
 */
bool SubmitPostgres::Exit()
{
	if (m_SQLHandler != NULL)
	{
		delete m_SQLHandler;
		m_SQLHandler = NULL;
	}
	return true;
}


/**
 * SubmitHandler::Submit(Download *down)
 * 
 * @param down   the download to hexdump
 */
void SubmitPostgres::Submit(Download *down)
{
	logPF();

	PGDownloadContext *ctx = new PGDownloadContext(down);

	string query;

	query = "SELECT mwcollect.sensor_exists_sample('";
	query += ctx->getHashMD5();
	query += "','";
	query += ctx->getHashSHA512();
	query += "')";

	logSpam("Query is %s\n",query.c_str());

	m_SQLHandler->addQuery(&query,this,ctx);

	ctx->setState(PG_SAMPLE_EXISTS);
	m_OutstandingQueries.push_back(ctx);
}

/**
 * SubmitHandler::Hit(Download *down)
 * 
 * 
 * @param down   the download to hexdump
 */
void SubmitPostgres::Hit(Download *down)
{
	logPF();

	Submit(down);
	return;
}


bool SubmitPostgres::sqlSuccess(SQLResult *result)
{
	logPF();
	logSpam("Query %s had success (%i results)\n",result->getQuery().c_str(),(*(result->getResult())).size());

	vector< map<string,string> > resvec = *result->getResult();

/*
	vector< map<string,string> >::iterator it = resvec.begin();
	map<string,string>::iterator jt;


	//	this is my reply debugger 
	
	string msg;

	for ( jt = it->begin(); jt != it->end(); jt++ )
	{
		msg = msg + "| " + jt->first + " ";
	}

	msg += "|\n";

	for ( it = resvec.begin(); it != resvec.end(); it++ )
	{

		for ( jt = it->begin(); jt != it->end(); jt++ )
		{
			msg = msg + "| " + string((*it)[jt->first].data(),(*it)[jt->first].size()) + " ";
		}
		msg += "|\n";
	}

	logInfo("%s\n",msg.c_str());
*/

	PGDownloadContext *ctx = (PGDownloadContext *)result->getObject();

	switch (ctx->getState())
	{
	case PG_SAMPLE_EXISTS:
		if (resvec[0]["sensor_exists_sample"] == "t")
		{
			string query;
			query = "SELECT mwcollect.sensor_add_instance('";
			query += ctx->getHashMD5();
			query += "','";
			query += ctx->getHashSHA512();
			query += "','";
			query += ctx->getRemoteHost();
			query += "','";
			query += ctx->getLocalHost();
			query += "','";
			query += m_SQLHandler->escapeString(ctx->getUrl());
			query += "')";
			logSpam("Query is %s\n",query.c_str());

			m_SQLHandler->addQuery(&query,this,ctx);

			ctx->setState(PG_INSTANCE_ADD);
			m_OutstandingQueries.push_back(ctx);

		}else
		{
			string query;
			query = "SELECT mwcollect.sensor_add_sample('";
			query += ctx->getHashMD5();
			query += "','";
			query += ctx->getHashSHA512();
			query += "','";
			query += m_SQLHandler->escapeBinary(ctx->getFileContent());
			query += "','";
			query += ctx->getRemoteHost();
			query += "','";
			query += ctx->getLocalHost();
			query += "','";
			query += m_SQLHandler->escapeString(ctx->getUrl());
			query += "')";
			logSpam("Query is %s\n",query.c_str());

			m_SQLHandler->addQuery(&query,this,ctx);

			ctx->setState(PG_SAMPLE_ADD);
			m_OutstandingQueries.push_back(ctx);
		}
		break;

	case PG_INSTANCE_ADD:
		if (resvec[0]["sensor_add_instance"] == "f")
			logCrit("ERROR inserting instance\n");
		m_OutstandingQueries.front()->remove();
		delete m_OutstandingQueries.front();
		break;

	case PG_SAMPLE_ADD:
		if (resvec[0]["sensor_add_sample"] == "f")
			logCrit("ERROR inserting sample\n");
		m_OutstandingQueries.front()->remove();
		delete m_OutstandingQueries.front();
		break;

	default:
		logCrit("UNEXPECTED STATE IN %s:%i\n",__FILE__,__LINE__);
	}


	m_OutstandingQueries.pop_front();
	return true;
}

bool SubmitPostgres::sqlFailure(SQLResult *result)
{
	logPF();
	m_OutstandingQueries.front()->remove();
	delete m_OutstandingQueries.front();
    m_OutstandingQueries.pop_front();
	return true;
}


void SubmitPostgres::sqlConnected()
{
	logPF();
}

void SubmitPostgres::sqlDisconnected()
{
	logPF();
}

string SubmitPostgres::getSpoolPath()
{
	return m_SpoolDir;
}

extern "C" int32_t module_init(int32_t version, Module **module, Nepenthes *nepenthes)
{
	if(version == MODULE_IFACE_VERSION)
	{
		*module = new SubmitPostgres(nepenthes);
		return 1;
	} else
	{
		return 0;
	}
}


