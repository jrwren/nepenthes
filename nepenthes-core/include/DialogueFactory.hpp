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
#ifndef HAVE_DIALOGUEFACTORY_HPP
#define HAVE_DIALOGUEFACTORY_HPP

#include <string>

using namespace std;

namespace nepenthes
{
	class Socket;
	class Dialogue;

	/**
	 * whenever we bind a Socket, we have to assign a DialogueFactory.
	 * the dialogueFactory will create Dialogues for accepted() connections
	 * and assign this fresh Dialogue to the new Socket
	 */
	class DialogueFactory
    {
    public:
        virtual ~DialogueFactory(){};
        virtual Dialogue * createDialogue(Socket *socket) = 0;

        string getFactoryName()
		{
			return m_DialogueFactoryName;
		}
        string getFactoryDescription()
		{
			return m_DialogueFactoryDescription;
		}

		virtual void socketClosed(Socket *socket)
		{
			return;
		}

    protected:
        string m_DialogueFactoryName;
        string m_DialogueFactoryDescription;

        Socket *m_Socket;
    };
}

#endif
