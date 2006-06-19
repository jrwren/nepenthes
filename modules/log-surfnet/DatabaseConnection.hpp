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



#include "config.h"

#ifdef HAVE_POSTGRES
#include <libpq-fe.h>
#endif

#include <string>
using namespace std;

namespace nepenthes
{

	class DatabaseConnection
	{
	public:
		DatabaseConnection(const char *server, const char *user, const char *passwd, const char *db);
		~DatabaseConnection();

		bool Init();
		bool Exit();

		int32_t getSensorID(uint32_t ip);
		int32_t addAttack(int32_t severity, uint32_t attackerip, uint16_t attackerport, uint32_t decoyip, uint16_t decoyport, string hwa, int32_t sensorid);

		void updateAttackSeverity(int32_t attackid, int32_t newseverity);
		void addDetail(int32_t attackid, int32_t sensorid, int32_t detailtype , const char *text);


	private:
#ifdef HAVE_POSTGRES
		PGconn     *m_PGConnection;
#endif

		string m_Server;
		string m_User;
		string m_Pass;
		string m_DB;
	};
}


