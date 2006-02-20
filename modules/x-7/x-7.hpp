
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

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace std;

namespace nepenthes
{

	class X7 : public Module , public DialogueFactory
	{
	public:
		X7(Nepenthes *);
		~X7();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();
	};

	class X7Dialogue : public Dialogue
	{
	public:
		X7Dialogue(Socket *socket);
		~X7Dialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
	};



typedef struct dns_header
{
  uint16_t  transid;
/*#if defined (WORDS_BIGENDIAN)
  u_int16_t flag_qr:1,
            flag_opcode:4,
            flag_aa:1,
            flag_tc:1,
            flag_rd:1,
            flag_ra:1,
            flag_zero:3,
            flag_rcode:4;
#else 
*/
  u_int16_t flag_rcode:4,
            flag_zero:3,
            flag_ra:1,
            flag_rd:1,
            flag_tc:1,
            flag_aa:1,
            flag_opcode:4,
            flag_qr:1;
//#endif 

  u_int16_t number_questions;
  u_int16_t number_answers;
  u_int16_t number_authority;
  u_int16_t number_additional;
} dns_header_t;


#define NAME_SIZE 512

typedef struct dns_rr{
  char name[NAME_SIZE];
  uint16_t type;
  uint16_t tclass;
  uint32_t ttl;
  uint16_t rdatalen;
  char data[NAME_SIZE];
} dns_rr_t;


/*
 * Query/response flag
 */

#define DNS_QRFLAG_QUERY        0
#define DNS_QRFLAG_RESPONSE     1

/*
 * Opcode flag
 */

#define DNS_OPCODEFLAG_STANDARD     0
#define DNS_OPCODEFLAG_INVERSE      1
#define DNS_OPCODEFLAG_STATUS       2

/*
 * Rcode (return code) flag
 */

#define DNS_RCODEFLAG_NOERROR        0
#define DNS_RCODEFLAG_FORMATERROR    1
#define DNS_RCODEFLAG_SERVERERROR    2
#define DNS_RCODEFLAG_NAMEERROR      3
#define DNS_RCODEFLAG_NOTIMPLEMENTED 4
#define DNS_RCODEFLAG_SERVICEREFUSED 5

/*
 * Query type
 */

#define DNS_QUERYTYPE_A              1
#define DNS_QUERYTYPE_NS             2
#define DNS_QUERYTYPE_CNAME          5
#define DNS_QUERYTYPE_SOA            6
#define DNS_QUERYTYPE_PTR            12
#define DNS_QUERYTYPE_HINFO          13
#define DNS_QUERYTYPE_MX             15
#define DNS_QUERYTYPE_AAAA           28
#define DNS_QUERYTYPE_AXFR           252
#define DNS_QUERYTYPE_ANY            255

/*
 * Query class
 */

#define DNS_QUERYCLASS_IP            1

}
extern nepenthes::Nepenthes *g_Nepenthes;
