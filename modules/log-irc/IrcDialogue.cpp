/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * Copyright (C) 2006  Georg Wicherski
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
#include <string.h>
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


#ifdef __GNUG__
	#define MY_COMPILER "g++"
#else
	#define MY_COMPILER "unknown Compiler"
#endif


IrcDialogue::IrcDialogue(Socket *socket, LogIrc * logirc)
{
	m_Socket = socket;
	m_DialogueName = "IrcDialogue";
	m_DialogueDescription = "IRC logging client";
	
	m_ConsumeLevel = CL_ASSIGN;
	m_LogIrc = logirc;
	m_LoggedOn = false;

	m_State = IRCDIA_REQUEST_SEND;

	if ( m_LogIrc->useTor() == true )
	{
		SocksHeader torHeader;
		memset(&torHeader, 0, sizeof(torHeader));

		torHeader.version = 4;
		torHeader.command = 1; // connect request
		torHeader.destPort = htons(m_LogIrc->getIrcPort());
		torHeader.destAddress = m_LogIrc->getIrcIP();

		m_Socket->doRespond((char *) &torHeader,8+strlen(torHeader.user)+1);
	}
	else
	{
		m_State = IRCDIA_CONNECTED;
		sendServerPass();
		sendNick(false);
		sendUser();
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

ConsumeLevel IrcDialogue::incomingData(Message *msg)
{
	switch (m_State)
	{
	case IRCDIA_REQUEST_SEND:
		if( ((SocksHeader *) msg->getMsg())->command == 90 )
		{
			logInfo("Connected to IRC server \"%s\" through TOR proxy \"%s\"\n", m_LogIrc->getIrcServer().c_str(), m_LogIrc->getTorServer().c_str());
			m_State = IRCDIA_CONNECTED;

			sendServerPass();
			sendNick(false);
			sendUser();

		}
		else
		{
			logInfo("Relaying to IRC server \"%s\" denied by TOR proxy \"%s\"\n", m_LogIrc->getIrcServer().c_str(), m_LogIrc->getTorServer().c_str());
			return CL_DROP;
		}
		
		break;
		
	case IRCDIA_CONNECTED:
		m_Buffer->add(msg->getMsg(),msg->getSize());
		processBuffer();
		
		break;

	}
	
	return CL_ASSIGN;
}

//! oxff: slightly faster and definitely better readable than old implementation
void IrcDialogue::processBuffer()
{
	uint32_t bufferLength = m_Buffer->getSize();
	uint32_t lineLength, processedLength;
	const char *look, *lineStart;
	
	if(bufferLength < 2)
		return;
		
	processedLength = 0;
	look = (char *) m_Buffer->getData();
	
	lineStart = look;
	lineLength = 1;
	++look;
	
	for(uint32_t i = 0; i < bufferLength; ++i, ++look)
	{
		if(* look == '\n' && * (look - 1) == '\r')
		{
			// -1 cuts the already processed '\r'
			processLine(lineStart, lineLength - 1);
			
			// + 1 includes the trailing '\n'
			processedLength += lineLength + 1;
			lineLength = 0;
			lineStart = look + 1;
		}
		else
			++lineLength;
	}
	
	m_Buffer->cut(processedLength);
}

//! oxff: again slightly faster and definitely better readable
//! oxff: now splits into IRC RFC elements, not words (leaving PRIVMSG lines)
void  IrcDialogue::processLine(const char *line, uint32_t lineLength)
{
	vector<string> lineElements;
	
	{
		const char *look = line;
		string element;
		
		if(* line == ':')
		{
			if(!--lineLength)
				return;
				
			++look;
		}
		
		//! oxff: this is a pre-condition to the parsing loop
		if(* look == ':')
		{
			logWarn("IRC Server \"%s\" sent line beginning with two colons\n", m_LogIrc->getIrcServer().c_str());
			return;
		}

		for(uint32_t i = 0; i < lineLength; ++i, ++look)
		{
			if(* look == ' ')
			{
				lineElements.push_back(element);
				element.erase();
			}
			else if(* look == ':' && * (look - 1) == ' ') // look behind allowed due to pre-condition
			{				
				element = string(look + 1, lineLength - i - 1);
				lineElements.push_back(element);
				element.erase();
				
				break;
			}
			else
				element.push_back(* look);
		}
		
		if(element.size())		
			lineElements.push_back(element);
	}
	
	if(lineElements.empty())
		return;
	
	
	if(lineElements.size() >= 1 && lineElements[1] == "433")
		sendNick(true);
	if(lineElements[0] == "PING" && lineElements.size() == 2)
	{
		string reply = "PONG " + lineElements[1] + "\r\n";
		
		m_Socket->doRespond((char *) reply.data(), reply.size());
	}
	else if(lineElements[0] == "PONG")
		m_Pinged = false;
	else if(lineElements.size() >= 2 && (lineElements[1] == "003" ||lineElements[1] == "004" ||  lineElements[1] == "005" || lineElements[1] == "376" || lineElements[1] == "422"))
		loggedOn();
	else if(lineElements.size() >= 4 && (lineElements[1] == "PRIVMSG" || lineElements[1] == "NOTICE"))
		processMessage(lineElements[0].c_str(), lineElements[2].c_str(), lineElements[3].c_str());
}

void IrcDialogue::processMessage(const char *origin, const char *target, const char *message)
{
	string responseMessage = string("PRIVMSG ");
	
	logDebug("<%s.%s.%s> \"%s\"\n", m_LogIrc->getIrcServer().c_str(), target, origin, message);
	
	if(m_NickName == target)
	{
		string originString = origin;
		
		responseMessage += originString.substr(0, originString.find('!'));
	}
	else
		responseMessage += target;
		
	if(!strcmp(message, "!version"))
	{
		responseMessage += " :nepenthes v" VERSION " compiled on [" __DATE__ " " __TIME__ "] with " MY_COMPILER " " __VERSION__ "\r\n";
		m_Socket->doRespond((char *) responseMessage.data(), responseMessage.size());
	}
	else if(!strncmp(message, "!pattern ", 9))
	{
		m_LogIrc->setLogPattern(message + 9);
		
		responseMessage += " :Updated log pattern to \"" + string(message + 9) + "\"\r\n";
		m_Socket->doRespond((char *) responseMessage.data(), responseMessage.size());
	}
	else if(!strcmp(message, "!help") && m_NickName == target)
	{
		static const char *helpLines[] =
		{
			" :nepenthes v" VERSION " log-irc control interface\r\n",
			" :  !version - print detailed version information\r\n",
			" :  !pattern <logtags> - set log pattern for this log-irc instance\r\n",
			" :  !help - display this help message\r\n",
			" : \r\n",
			" : <http://nepenthes.mwcollect.org/>\r\n",
		};
		
		for(uint32_t i = 0; i < sizeof(helpLines) / sizeof(char *); ++i)
		{
			string responseLine = responseMessage + helpLines[i];			
			m_Socket->doRespond((char *) responseLine.data(), responseLine.size());
		}
	}
}


void IrcDialogue::loggedOn()
{
	m_LogIrc->setDialogue(this);
	
	if(m_LoggedOn)
		return;
	
	string connectCommand = m_LogIrc->getConnectCommand();
	
	if(!connectCommand.empty())
		m_Socket->doRespond((char *) connectCommand.data(), connectCommand.size());

	string joinCommand = "JOIN " + m_LogIrc->getIrcChannel() + " " + m_LogIrc->getIrcChannelPass() + "\r\n";
	m_Socket->doRespond((char *) joinCommand.data(), joinCommand.size());
	
	m_LoggedOn = true;
}

ConsumeLevel IrcDialogue::outgoingData(Message *msg)
{
	return CL_ASSIGN;
}

ConsumeLevel IrcDialogue::handleTimeout(Message *msg)
{
	if (m_Pinged == false)
	{
		m_Pinged = true;
		
    		string ping = "PING :12356789\r\n";
		m_Socket->doRespond((char *) ping.data(), ping.size());
		
		return CL_ASSIGN;
	}
	else
	{
		m_LogIrc->doRestart();
		return CL_DROP;
	}
}

ConsumeLevel IrcDialogue::connectionLost(Message *msg)
{
	logPF();
	
	m_LogIrc->doRestart();
	return CL_DROP;
}

ConsumeLevel IrcDialogue::connectionShutdown(Message *msg)
{
	logPF();
	
	m_LogIrc->doRestart();
	return CL_DROP;
}



struct FlagMapping
{
	int32_t	m_LogFlag;
	const char 	*m_ColorFlag;
};

const struct FlagMapping colors[] =
{
	{
		l_crit,
		"\x03\x34" // light red
	},
	{
		l_warn,
		"\x03\x36" // purple
	},
	{
		l_debug,
		"\x03\x31\x33" // pink
	},
	{
		l_info,
		"\x03\x39" // light green
	},
	{
		l_spam,
		"\x03\x31\x34" // dark grey
	},
};


// oxff: made dynamic log tag pattern
void IrcDialogue::logIrc(uint32_t mask, const char *message)
{
	if(m_LogIrc->logMaskMatches(mask))
	{
		if (strlen(message) > 450)
			return;
			
		string line ="PRIVMSG " + m_LogIrc->getIrcChannel() + " :";
		uint32_t i=0;

		for (i=0; i < sizeof(colors) / sizeof(struct FlagMapping); ++i)
		{
			if(mask & colors[i].m_LogFlag)
			{
				line += colors[i].m_ColorFlag;
				break;
			}
		}

		line += message;

		m_Socket->doRespond((char *) line.data(), line.size());
	}
}	


void IrcDialogue::sendNick(bool random)
{
	m_NickName = m_LogIrc->getIrcNick();
	
	if(random)
	{
		m_NickName += "-";
		m_NickName += (char) ((int32_t)rand()%20 + 97);
		m_NickName += (char) ((int32_t)rand()%20 + 97);
		m_NickName += (char) ((int32_t)rand()%20 + 97);
	}
	
	string nickCommand = "NICK " + m_NickName + "\r\n";
	m_Socket->doRespond((char *) nickCommand.data(), nickCommand.size());
}

void IrcDialogue::sendUser()
{
	string user = "USER " + m_LogIrc->getIrcIdent() + " 0 0 :" + m_LogIrc->getIrcUserInfo() + "\r\n";
	m_Socket->doRespond((char *) user.data(), user.size());
}

void IrcDialogue::sendServerPass()
{
	if ( m_LogIrc->getIrcPass().size() > 0 )
	{
		string pass = "PASS " + m_LogIrc->getIrcPass() + "\r\n";
		m_Socket->doRespond((char *) pass.data(), pass.size());
	}
}

