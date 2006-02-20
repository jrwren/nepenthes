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

#include <ctype.h>
#include <string>
#include <vector>

#include "log-irc.hpp"
#include "IrcDialogue.hpp"


#include "Buffer.hpp"
#include "Buffer.cpp"

#include "SocketManager.hpp"
#include "Message.hpp"
#include "DownloadManager.hpp"
#include "LogManager.hpp"
#include "DialogueFactoryManager.hpp"


#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_mod

using namespace nepenthes;
using namespace std;




/**
 * Dialogue::Dialogue(Socket *)
 * construktor for the IrcDialogue, creates a new IrcDialogue
 * 
 * replies some crap to the socket
 * 
 * @param socket the Socket the Dialogue has to use
 */
IrcDialogue::IrcDialogue(Socket *socket, LogIrc * logirc)
{
	m_Socket = socket;
    m_DialogueName = "IrcDialogue";
	m_DialogueDescription = "eXample Dialogue";
	
	m_ConsumeLevel = CL_ASSIGN;
	m_LogIrc = logirc;

	m_State = IRCDIA_REQUEST_SEND;

	if ( m_LogIrc->useTor() == true )
	{
		socks4_header_t s4hHeader;
		memset(&s4hHeader,0,sizeof(socks4_header_t));

		s4hHeader.ucVersion = 4;
		s4hHeader.ucCommand = 1; // connect request
		s4hHeader.usDestPort = htons(m_LogIrc->getIrcPort());
		s4hHeader.ulDestAddr = m_LogIrc->getIrcIP();

		m_Socket->doRespond((char *) &s4hHeader,8+ strlen(s4hHeader.szUser)+1 );
	} else
	{
		m_State = IRCDIA_CONNECTED;
		string nick = "NICK ";
		nick += m_LogIrc->getIrcNick();
		nick += "\r\n";

		m_Socket->doRespond((char *)nick.c_str(),nick.size());

		string user = "USER ";
		user += m_LogIrc->getIrcIdent();
		user += " 0 0 :";
		user += m_LogIrc->getIrcUserInfo();
		user += "\r\n";
		m_Socket->doRespond((char *)user.c_str(),user.size());
	}

	m_Buffer = new Buffer(1024);

	m_Pinged = false;
}

IrcDialogue::~IrcDialogue()
{
	if (m_Buffer != NULL)
	{
		delete m_Buffer;
	}
	m_LogIrc->setDialogue(NULL);
}

/**
 * Dialogue::incomingData(Message *)
 * 
 * a small and ugly shell where we can use
 * "download protocol://localction:port/path/to/file
 * to trigger a download
 * 
 * @param msg the Message the Socker received.
 * 
 * 
 * @return CL_ASSIGN
 */
ConsumeLevel IrcDialogue::incomingData(Message *msg)
{
	
	switch (m_State)
	{
	case IRCDIA_REQUEST_SEND:
		if ( ((socks4_header_t *)msg->getMsg())->ucCommand == 90 )
		{
			logInfo("%s","connected to ircd via tor\n");
			m_State = IRCDIA_CONNECTED;

			string nick = "NICK ";
			nick += m_LogIrc->getIrcNick();
			nick += "\r\n";

			m_Socket->doRespond((char *)nick.c_str(),nick.size());

			string user = "USER ";
			user += m_LogIrc->getIrcIdent();
			user += " 0 0 :";
			user += m_LogIrc->getIrcUserInfo();
			user += "\r\n";

			m_Socket->doRespond((char *)user.c_str(),user.size());
			

		} else
		{
			logWarn("%s","tor did not accept our connection \n");
			return CL_DROP;
		}
		break;
	case IRCDIA_CONNECTED:
		{
			m_Buffer->add(msg->getMsg(),msg->getSize());
			processBuffer();
		}
		break;

	}
	return CL_ASSIGN;
}


void IrcDialogue::processBuffer()
{
	logPF();
	unsigned char *linestart = (unsigned char *)m_Buffer->getData();
	unsigned char *linestopp = (unsigned char *)m_Buffer->getData();

	uint32_t i=0;
	while (i < m_Buffer->getSize())
	{
		
		if ( linestopp[i] == '\n')
		{
			i++;
//			printf("IRCLINE len %i \n'%.*s'\n",(int32_t)(linestopp+i-linestart),(int32_t)(linestopp+i-linestart),linestart);
			string line((char *)m_Buffer->getData(),(int32_t)(linestopp+i - linestart));	
			if (line[line.size()-1] == '\n')
			{
				line[line.size()-1] = '\0';
			}
			if (line[line.size()-2] == '\r')
			{
				line[line.size()-2] = '\0';
			}
			printf("test '%s'\n",line.c_str());
			processLine(&line);
			m_Buffer->cut(i);
			i=0;
			linestart = linestopp+i;
		}else
		{
			i++;
		}
	}
}

void IrcDialogue::processLine(string *line)
{
	vector<string> words;

	uint32_t i=0;      
	bool haschar = false;
	uint32_t wordstart=0;
	uint32_t wordstopp=0;

	while ( i<=line->size() )
	{
		if ( ( ( (*line)[i] == ' ' || (*line)[i] == '\0') && haschar == true) )
		{
			wordstopp = i;
			string word = line->substr(wordstart,wordstopp-wordstart);
//			logInfo("Word is %i %i '%s' \n",wordstart,wordstopp,word.c_str());
			words.push_back(word);
			haschar = false;
		} else
		if ( isgraph((*line)[i]) && haschar == false )
		{
			haschar = true;
			wordstart = i;
		}
		i++;
	}

/*
	for (i=0;i<words.size();i++)
	{
		logSpam("word is '%s'\n",words[i].c_str());
	}
*/

	if (words[0] == "PING" )
	{
		string reply = "PONG ";
		reply += words[1];
		reply += "\r\n";
		m_Socket->doRespond((char *)reply.c_str(),reply.size());

	}else
	if ( words[1] == "376" || words[1] == "422" )
	{
		string reply = "JOIN ";
		reply += m_LogIrc->getIrcChannel();
		reply += "\r\n";
		m_Socket->doRespond((char *)reply.c_str(),reply.size());
		m_LogIrc->setDialogue(this);
	}else
	if (words[1] == "PONG" )
	{
		m_Pinged = false;
	}else
	if ( words[1] == "433" )
	{
		string nick = "NICK ";
		nick += m_LogIrc->getIrcNick();
		nick += (char) ((int32_t)rand()%20 + 97);
		nick += "\r\n";

		m_Socket->doRespond((char *)nick.c_str(),nick.size());

	}

	return;

}

/**
 * Dialogue::outgoingData(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel IrcDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

/**
 * Dialogue::handleTimeout(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel IrcDialogue::handleTimeout(Message *msg)
{
	if (m_Pinged == false)
	{
		m_Pinged = true;
    	string ping = "PING :12356789\r\n";
		m_Socket->doRespond((char *)ping.c_str(),ping.size());
		return CL_ASSIGN;
	}else
	{
		m_LogIrc->doRestart();
		return CL_DROP;
	}

}

/**
 * Dialogue::connectionLost(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel IrcDialogue::connectionLost(Message *msg)
{
	logPF();
	m_LogIrc->doRestart();
	return CL_DROP;
}

/**
 * Dialogue::connectionShutdown(Message *)
 * as we are not interested in these socket actions 
 * we simply return CL_DROP to show the socket
 * 
 * @param msg
 * 
 * @return CL_DROP
 */
ConsumeLevel IrcDialogue::connectionShutdown(Message *msg)
{
	logPF();
	m_LogIrc->doRestart();
	return CL_DROP;
}

struct FlagMapping
{
	int32_t 	m_LogFlag;
	char 	*m_ColorFlag;
};


const struct FlagMapping colors[] =
{
	{
		l_crit,
		"\x03\x34" // helles rot
	},
	{
		l_warn,
		"\x03\x36"	// lila
	},
	{
		l_debug,
		"\x03\x31\x33"	// pink
	},
	{
		l_info,
		"\x03\x39" // helles gruen 
	},
    {
		l_spam,
		"\x03\x31\x34" // dunkel grau
	}
};



void 	IrcDialogue::logIrc(uint32_t mask, const char *message)
{
	if (
		((mask & l_dl || mask & l_sub) && mask & l_mgr && !(mask & l_spam)  ) ||
		(mask & l_warn || mask & l_crit) 
		)
	{
		if (strlen(message) > 450)
			return;
		string line ="PRIVMSG ";
		line += m_LogIrc->getIrcChannel();
		line += " :";

		uint32_t i=0;

		for (i=0;i<sizeof(colors)/sizeof(struct FlagMapping);i++)
		{
			if (mask & colors[i].m_LogFlag)
			{
				line += colors[i].m_ColorFlag;
			}
		}

		line += message;

		m_Socket->doRespond((char *)line.c_str(), line.size());
	}
}
