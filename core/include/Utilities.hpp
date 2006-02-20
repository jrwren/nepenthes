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

/* Additional notes:
 * 
 * the md5sum logic is based on : md5sum.c & md5sum.h 
 * here is the md5sm.c header
 *
 *
 * This code has been heavily hacked by Tatu Ylonen <ylo@ssh.fi> to
 *  make it compile on machines like Cray that don't have a 32 bit integer
 *  type. 
 *
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 *
 *
 * endof md5sum.c header
 */

 

/* $Id$ */
#ifndef HAVE_UTILITIES_HPP
#define HAVE_UTILITIES_HPP


#include <string>
#include "Nepenthes.hpp"

using namespace std;

namespace nepenthes
{
	typedef unsigned long word32;
	typedef word32 md5_uint32;

	struct MD5Context
	{
		md5_uint32 buf[4];
		md5_uint32 bits[2];
		unsigned char in[64];
	};

	typedef struct MD5Context MD5_CTX;


	class Utilities
	{
	public:
		Utilities();
		virtual ~Utilities();
		virtual string md5sum(char *msg, int len);

		void MD5Init(struct MD5Context *context);
		void MD5Update(struct MD5Context *context, unsigned char const *buf,unsigned len);
		void MD5Final(unsigned char digest[16], struct MD5Context *context);

		virtual void hexdump(byte *data, uint len);
		virtual void hexdump(unsigned int mask, byte *data, uint len);

		virtual unsigned char *b64encode_alloc(unsigned char *in);
		virtual unsigned char *b64encode_alloc(unsigned char *in, int inlen);
		virtual unsigned char *b64decode_alloc(unsigned char *in);
		virtual void hmac_md5( unsigned char* text, int text_len, unsigned char* key, int key_len, unsigned char* digest);
		virtual string hmac_md5( unsigned char* text, int text_len, unsigned char* key, int key_len);
	private:
		void MD5Transform(md5_uint32 buf[4], const unsigned char in[64]);

		int b64encode_len(unsigned char *in);
		int b64encode_len(unsigned char *in, int inlen);

		int b64decode_len(unsigned char *in);
		void b64encode(unsigned char *in, unsigned char *out);
		void b64encode(unsigned char *in, int inlen, unsigned char *out);
		void b64decode(unsigned char *in, unsigned char *out);

		static void b64enc(unsigned char *in, int inlen, unsigned char *out);
		static void b64dec(unsigned char *in, int inlen, unsigned char *out);


	};
}

#endif
