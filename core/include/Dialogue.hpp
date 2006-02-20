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

#ifndef HAVE_DIALOGUE_HPP
#define HAVE_DIALOGUE_HPP

#include <string>
using namespace std;

namespace nepenthes
{


    class Module;
    class Socket;
    class Nepenthes;
    class Message;

    typedef enum
    {
        CL_UNSURE = 0,
        CL_READONLY,
        CL_ASSIGN,
		CL_ASSIGN_AND_DONE,
        CL_DROP,
    } ConsumeLevel;


    class Dialogue
    {
    public:
        virtual ~Dialogue(){};
        virtual ConsumeLevel incomingData(Message * msg) = 0;
        virtual ConsumeLevel outgoingData(Message * msg) = 0;
        virtual ConsumeLevel handleTimeout(Message * msg) = 0;
        virtual ConsumeLevel connectionLost(Message * msg) = 0;      // recv < 0
        virtual ConsumeLevel connectionShutdown(Message * msg) = 0;  // recv() == 0
        virtual ConsumeLevel getConsumeLevel()
		{
			return m_ConsumeLevel;
		}
        virtual void setConsumeLevel(ConsumeLevel cl)
		{
			m_ConsumeLevel = cl;
		}
        virtual string getDialogueName()
		{
			return m_DialogueName;
		};
        virtual string getDialogueDescription()
		{
			return m_DialogueDescription;
		};

		virtual Socket *getSocket()
		{
			return m_Socket;
		}


		/**
		 * if more than one dialogues is assigned to a socket, 
		 * a dialogue who fits the traffic and recognizes a shellcode, can return CL_ASSIGN_AND_DONE
		 * the Socket will then call setStateDone for each assigned dialogue.
		 * its upto the dialogue itself to implement it in a way it sets the dialogue to a _DONE state, 
		 * so it does not hexdump the already by some other dialogue recognized traffic
		 */
		virtual void syncState(ConsumeLevel cl)
		{
			return;
		}

    protected:
        Module      *m_Module;
        Socket      *m_Socket;
        Nepenthes   *m_Nepenthes;

        ConsumeLevel m_ConsumeLevel;
        string      m_DialogueName;
        string      m_DialogueDescription;

    };

}
#endif
