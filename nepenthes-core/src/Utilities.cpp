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

#ifdef WIN32
#include <memory.h>
#else
#endif


#include <string.h>
#include <sys/types.h>
# include <inttypes.h>
#include "Utilities.hpp"
#include "LogManager.hpp"

using namespace nepenthes;


Utilities::Utilities()
{

}

Utilities::~Utilities()
{
}

// START MD5Sum

/*------------ macros for storing/extracting msb first words -------------*/

#define GET_32BIT(cp) (((uint32_t)(unsigned char)(cp)[0] << 24) | \
                       ((uint32_t)(unsigned char)(cp)[1] << 16) | \
                       ((uint32_t)(unsigned char)(cp)[2] << 8) |  \
                       ((uint32_t)(unsigned char)(cp)[3]))

#define GET_16BIT(cp) (((uint32_t)(unsigned char)(cp)[0] << 8) | \
                       ((uint32_t)(unsigned char)(cp)[1]))

#define PUT_32BIT(cp, value) do {  \
  (cp)[0] = (char)((value) >> 24); \
  (cp)[1] = (char)((value) >> 16); \
  (cp)[2] = (char)((value) >> 8);  \
  (cp)[3] = (char)(value); } while (0)

#define PUT_16BIT(cp, value) do { \
  (cp)[0] = (value) >> 8;         \
  (cp)[1] = (value); } while (0)

/*------------ macros for storing/extracting lsb first words -------------*/

#define GET_32BIT_LSB_FIRST(cp)                   \
  (((uint32_t)(unsigned char)(cp)[0]) |      \
  ((uint32_t)(unsigned char)(cp)[1] << 8) |  \
  ((uint32_t)(unsigned char)(cp)[2] << 16) | \
  ((uint32_t)(unsigned char)(cp)[3] << 24))

#define GET_16BIT_LSB_FIRST(cp)                  \
  (((uint32_t)(unsigned char)(cp)[0]) |     \
  ((uint32_t)(unsigned char)(cp)[1] << 8))

#define PUT_32BIT_LSB_FIRST(cp, value) do {    \
  (cp)[0] = (char)(value);                     \
  (cp)[1] = (char)((value) >> 8);              \
  (cp)[2] = (char)((value) >> 16);             \
  (cp)[3] = (char)((value) >> 24); } while (0)

#define PUT_16BIT_LSB_FIRST(cp, value) do {   \
  (cp)[0] = (char)(value);                    \
  (cp)[1] = (char)((value) >> 8); } while (0)



/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void Utilities::MD5Transform(md5_uint32 buf[4], const unsigned char inext[64])
{
    register word32 a, b, c, d, i;
    word32 in[16];

    for (i = 0; i < 16; i++)
      in[i] = GET_32BIT_LSB_FIRST(inext + 4 * i);

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void Utilities::MD5Init(struct MD5Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void Utilities::MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len)
{
    md5_uint32 t;

    /* Update bitcount */

    t = ctx->bits[0];
    if ((ctx->bits[0] = (t + ((md5_uint32)len << 3)) & 0xffffffff) < t)
        ctx->bits[1]++;         /* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */

    if (t) {
        unsigned char *p = ctx->in + t;

        t = 64 - t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        MD5Transform(ctx->buf, ctx->in);
        buf += t;
        len -= t;
    }
    /* Process data in 64-byte chunks */

    while (len >= 64) {
        memcpy(ctx->in, buf, 64);
        MD5Transform(ctx->buf, ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */

    memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void Utilities::MD5Final(unsigned char digest[16], struct MD5Context *ctx)
{
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        MD5Transform(ctx->buf, ctx->in);

        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    } else {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }

    /* Append length in bits and transform */
    PUT_32BIT_LSB_FIRST(ctx->in + 56, ctx->bits[0]);
    PUT_32BIT_LSB_FIRST(ctx->in + 60, ctx->bits[1]);

    MD5Transform(ctx->buf, ctx->in);
    PUT_32BIT_LSB_FIRST(digest, ctx->buf[0]);
    PUT_32BIT_LSB_FIRST(digest + 4, ctx->buf[1]);
    PUT_32BIT_LSB_FIRST(digest + 8, ctx->buf[2]);
    PUT_32BIT_LSB_FIRST(digest + 12, ctx->buf[3]);
    memset(ctx, 0, sizeof(ctx));        /* In case it's sensitive */
}



string Utilities::md5sum(char *msg, int32_t len)
{

	struct MD5Context MD5Struct;
	MD5Init(&MD5Struct);

	unsigned char MD5Result[16];
	MD5Update(&MD5Struct,(const unsigned char *)msg,len);
	MD5Final(MD5Result,&MD5Struct);

	string MD5Sum = "";

	//char sum[32];
	//int32_t i;

	for(uint32_t i = 0; i < 16; ++i)
	{
		MD5Sum += ((MD5Result[i] >> 4) < 10 ? (MD5Result[i] >> 4) + '0' : (MD5Result[i] >> 4) + ('a' - 10));
		MD5Sum += ((MD5Result[i] & 0xF) < 10 ? (MD5Result[i] & 0xF) + '0' : (MD5Result[i] & 0xF) + ('a' - 10));
	}
	return MD5Sum;
}

// ENDOF MD5Sum



void Utilities::hexdump(byte *data, uint32_t len)
{
	char conv[] = "0123456789abcdef";

	printf("=------------------[ hexdump(0x%08x , 0x%08x) ]-------------------=\n", (uint32_t)data, len);
	for( uint32_t i = 0; i < len; i += 0x10 )
	{
		printf("0x%04x  ", i);


		uint32_t j;
		for( j = 0; j < 0x10; j++ )
		{
			if( i + j < len )
			{
				printf("%c%c ",conv[((data[i + j] & 0xFF) >> 4)],conv[((data[i + j] & 0xff) & 0x0F)]);
			}
			else
				printf("   ");

			if( j == 7 )
				printf(" ");
		}

		printf(" ");

		for( j = 0; j < 0x10; j++ )
		{
			if( i + j < len )
				printf("%c", isprint(data[i + j]) ? data[i + j] : '.');
			else
				printf(" ");
			if( j == 7 )
				printf(" ");
		}

		printf("\n");
	}
	printf("=-------------------------------------------------------------------------=\n");
}

void Utilities::hexdump(uint32_t mask, byte *data, uint32_t len)
{
	char conv[] = "0123456789abcdef";

	string md5 = "var/log/hexdumps/";
	md5.append(md5sum((char *)data, len));
	md5.append(".bin");

	FILE *f;

	
	if( !len )
	{
		g_Nepenthes->getLogMgr()->log(mask,"=----------------[ ignoring hexdump request of 0 bytes. ]-----------------=\n");
		return;
	}

	if( (f = fopen(md5.c_str(), "wb")) )
	{
		fwrite((const void *)data, len, 1, f);
		fclose(f);

		g_Nepenthes->getLogMgr()->logf(mask,"=--------[ %s ]---------=\n", md5.c_str());
	}

	g_Nepenthes->getLogMgr()->logf(mask,"=------------------[ hexdump(0x%08x , 0x%08x) ]-------------------=\n", (uint32_t)data, len);
	for( uint32_t i = 0; i < len; i += 0x10 )
	{
		string line;
//		printf("0x%04x  ", i);
		line = "";

		uint32_t j;
		for( j = 0; j < 0x10; j++ )
		{
			if( i + j < len )
			{
//				printf("%c%c ",conv[((data[i + j] & 0xFF) >> 4)],conv[((data[i + j] & 0xff) & 0x0F)]);
				line += conv[((data[i + j] & 0xFF) >> 4)];
				line += conv[((data[i + j] & 0xff) & 0x0F)];
				line += " ";
			}
			else
//				printf("   ");
				line += "   ";

			if( j == 7 )
//				printf(" ");
				line += " ";
		}

//		printf(" ");
		line += " ";		

		for( j = 0; j < 0x10; j++ )
		{
			if( i + j < len )
//				printf("%c", isprint(data[i + j]) ? data[i + j] : '.');
				line += isprint(data[i + j]) ? data[i + j] : '.';
			else
//				printf(" ");
				line += " ";
			if( j == 7 )
//				printf(" ");
				line += " ";
		}

		line += "\n";
//		printf("\n");
	g_Nepenthes->getLogMgr()->logf(mask,"0x%04x  %s",i,line.c_str());
		
	}
	g_Nepenthes->getLogMgr()->log(mask,"=-------------------------------------------------------------------------=\n");
//	printf("=-------------------------------------------------------------------------=\n");
	
}







// START BASE64

static unsigned char b64e[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',	/*  0- 7 */
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',	/*  8-15 */
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',	/* 16-23 */
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',	/* 24-31 */
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',	/* 32-39 */
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',	/* 40-47 */
	'w', 'x', 'y', 'z', '0', '1', '2', '3',	/* 48-55 */
	'4', '5', '6', '7', '8', '9', '+', '/'	/* 56-63 */
};

static unsigned char b64d[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 0x00-0x0F */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 0x10-0x1F */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x3f, /* 0x20-0x2F */
	0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x00,0x00,0x00,0x00,0x00,0x00, /* 0x30-0x3F */
	0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e, /* 0x40-0x4F */
	0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x00,0x00,0x00,0x00,0x00, /* 0x50-0x5F */
	0x00,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28, /* 0x60-0x6F */
	0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x00,0x00,0x00,0x00,0x00	 /* 0x70-0x7F */
};

void Utilities::b64enc(unsigned char *in, int32_t inlen, unsigned char *out)
{
	unsigned char t0 = (inlen > 0) ? in[0] : 0;
	unsigned char t1 = (inlen > 1) ? in[1] : 0;
	unsigned char t2 = (inlen > 2) ? in[2] : 0;

	if ( inlen <= 0 ) return;
	out[0] = b64e[(t0 >> 2) & 0x3f];
	out[1] = b64e[((t0 << 4) & 0x30) | (t1 >> 4)];

	if ( inlen <= 1 ) return;
	out[2] = b64e[((t1 << 2) & 0x3c) | (t2 >> 6)];

	if ( inlen <= 2 ) return;
	out[3] = b64e[t2 & 0x3f];
}

void Utilities::b64dec(unsigned char *in, int32_t inlen, unsigned char *out)
{
	unsigned char t0 = (inlen > 0) ? b64d[in[0] & 0x7f] : 0;
	unsigned char t1 = (inlen > 1) ? b64d[in[1] & 0x7f] : 0;
	unsigned char t2 = (inlen > 2) ? b64d[in[2] & 0x7f] : 0;
	unsigned char t3 = (inlen > 3) ? b64d[in[3] & 0x7f] : 0;

	if ( inlen <= 0 ) return;
	out[0] = (t0 << 2) | (t1 >> 4);
	if ( inlen <= 1 )
	{
		out[1] = 0;
		return;
	}
	out[1] = (t1 << 4) | ((t2 & 0x3c) >> 2);
	if ( inlen <= 2 )
	{
		out[2] = 0;
		return;
	}
	out[2] = (t2 << 6) | t3;
}

int32_t Utilities::b64encode_len(unsigned char *in)
{
	int32_t l = strlen((char *)in);
	return 4*((l+2)/3);
}

int32_t Utilities::b64encode_len(unsigned char *in, int32_t l)
{
    return 4*((l+2)/3);
}


int32_t Utilities::b64decode_len(unsigned char *in)
{
	int32_t l = strlen((char *)in);
	return 3*((l+3)/4);
}

unsigned char *Utilities::b64encode_alloc(unsigned char *in)
{
	int32_t l = b64encode_len(in);
	unsigned char *n = (unsigned char *) malloc(l+1);
	if ( n != NULL )
	{
		n[l--] = 0;
		while ( l >= 0 )
		{
			n[l--] = '=';
		}
		b64encode(in,n);
	}
	return n;
}

unsigned char *Utilities::b64encode_alloc(unsigned char *in, int32_t inlen)
{
	int32_t l = b64encode_len(in,inlen);
	unsigned char *n = (unsigned char *) malloc(l+1);
	if ( n != NULL )
	{
		n[l--] = 0;
		while ( l >= 0 )
		{
			n[l--] = '=';
		}
		b64encode(in,inlen,n);
	}
	return n;
}


void Utilities::b64encode(unsigned char *in, unsigned char *out)
{
	int32_t inlen = strlen((char *)in);
	while ( inlen > 0 )
	{
		b64enc(in, inlen, out);
		inlen -= 3;
		in += 3;
		out += 4;
	}
}

void Utilities::b64encode(unsigned char *in,int32_t inlen, unsigned char *out)
{
    while ( inlen > 0 )
	{
		b64enc(in, inlen, out);
		inlen -= 3;
		in += 3;
		out += 4;
	}
}


unsigned char *Utilities::b64decode_alloc(unsigned char *in)
{
	int32_t l = b64decode_len(in);
	unsigned char *n = (unsigned char *) malloc(l+1);
	if ( n != NULL )
	{
		while ( l >= 0 )
		{
			n[l--] = 0;
		}
		b64decode(in,n);
	}
	return n;
}

void Utilities::b64decode(unsigned char *in, unsigned char *out)
{
	int32_t inlen = strlen((char *)in);
	while ( inlen > 0 )
	{
		b64dec(in, inlen, out);
		inlen -= 4;
		in += 4;
		out += 3;
	}
}

// ENDOF BASE64



// START HMAC_MD5

/*
** Function: hmac_md5
*/

void Utilities::hmac_md5(
unsigned char*  text,                /* pointer to data stream */
int32_t             text_len,            /* length of data stream */
unsigned char*  key,                 /* pointer to authentication key */
int32_t             key_len,             /* length of authentication key */
unsigned char   *digest)              /* caller digest to be filled in */

{
        MD5_CTX context;
        unsigned char k_ipad[65];    /* inner padding -
                                      * key XORd with ipad
                                      */
        unsigned char k_opad[65];    /* outer padding -
                                      * key XORd with opad
                                      */
        unsigned char tk[16];
        int32_t i;
        /* if key is longer than 64 bytes reset it to key=MD5(key) */
        if (key_len > 64) {

                MD5_CTX      tctx;

                MD5Init(&tctx);
                MD5Update(&tctx, key, key_len);
                MD5Final(tk, &tctx);

                key = tk;
                key_len = 16;
        }

        /*
         * the HMAC_MD5 transform looks like:
         *
         * MD5(K XOR opad, MD5(K XOR ipad, text))
         *
         * where K is an n byte key
         * ipad is the byte 0x36 repeated 64 times
         * opad is the byte 0x5c repeated 64 times
         * and text is the data being protected
         */

        /* start out by storing key in pads */

#ifdef WIN32
        memset( k_ipad, 0, sizeof k_ipad);
        memset( k_opad, 0, sizeof k_opad);
        memcpy( k_ipad, key, key_len);
        memcpy( k_opad, key, key_len);
#else
        bzero( k_ipad, sizeof k_ipad);
        bzero( k_opad, sizeof k_opad);
        bcopy( key, k_ipad, key_len);
        bcopy( key, k_opad, key_len);
#endif
        /* XOR key with ipad and opad values */
        for (i=0; i<64; i++) {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
        }
        /*
         * perform inner MD5
         */
        MD5Init(&context);                   /* init context for 1st pass */
        MD5Update(&context, k_ipad, 64);      /* start with inner pad */
        MD5Update(&context, text, text_len); /* then text of datagram */
        MD5Final(digest, &context);          /* finish up 1st pass */
        /*
         * perform outer MD5
         */
        MD5Init(&context);                   /* init context for 2nd
                                              * pass */
        MD5Update(&context, k_opad, 64);     /* start with outer pad */
        MD5Update(&context, digest, 16);     /* then results of 1st
                                              * hash */
        MD5Final(digest, &context);          /* finish up 2nd pass */
}

string Utilities::hmac_md5(
unsigned char*  text,                /* pointer to data stream */
int32_t             text_len,            /* length of data stream */
unsigned char*  key,                 /* pointer to authentication key */
int32_t             key_len)             /* length of authentication key */
{
	
	unsigned char MD5Result[16];
	string MD5Sum;
	hmac_md5(text, text_len, key, key_len,MD5Result);
	for(uint32_t i = 0; i < 16; ++i)
	{
		MD5Sum += ((MD5Result[i] >> 4) < 10 ? (MD5Result[i] >> 4) + '0' : (MD5Result[i] >> 4) + ('a' - 10));
		MD5Sum += ((MD5Result[i] & 0xF) < 10 ? (MD5Result[i] & 0xF) + '0' : (MD5Result[i] & 0xF) + ('a' - 10));
	}
	return MD5Sum;

}



// ENDOF HMAC_MD5


// START SHA512

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

#define SHA512_F1(x) (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define SHA512_F2(x) (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))
#define SHA512_F3(x) (ROTR(x,  1) ^ ROTR(x,  8) ^ SHFR(x,  7))
#define SHA512_F4(x) (ROTR(x, 19) ^ ROTR(x, 61) ^ SHFR(x,  6))

#define UNPACK32(x, str)                       \
{                                              \
    *((str) + 3) = (uint8_t) ((x)      );      \
    *((str) + 2) = (uint8_t) ((x) >>  8);      \
    *((str) + 1) = (uint8_t) ((x) >> 16);      \
    *((str) + 0) = (uint8_t) ((x) >> 24);      \
}

#define PACK32(str, x)                         \
{                                              \
    *(x) = ((uint32_t) *((str) + 3)      )     \
         | ((uint32_t) *((str) + 2) <<  8)     \
         | ((uint32_t) *((str) + 1) << 16)     \
         | ((uint32_t) *((str) + 0) << 24);    \
}

#define UNPACK64(x, str)                       \
{                                              \
    *((str) + 7) = (uint8_t) ((x)      );      \
    *((str) + 6) = (uint8_t) ((x) >>  8);      \
    *((str) + 5) = (uint8_t) ((x) >> 16);      \
    *((str) + 4) = (uint8_t) ((x) >> 24);      \
    *((str) + 3) = (uint8_t) ((x) >> 32);      \
    *((str) + 2) = (uint8_t) ((x) >> 40);      \
    *((str) + 1) = (uint8_t) ((x) >> 48);      \
    *((str) + 0) = (uint8_t) ((x) >> 56);      \
}

#define PACK64(str, x)                         \
{                                              \
    *(x) = ((uint64_t) *((str) + 7)      )     \
         | ((uint64_t) *((str) + 6) <<  8)     \
         | ((uint64_t) *((str) + 5) << 16)     \
         | ((uint64_t) *((str) + 4) << 24)     \
         | ((uint64_t) *((str) + 3) << 32)     \
         | ((uint64_t) *((str) + 2) << 40)     \
         | ((uint64_t) *((str) + 1) << 48)     \
         | ((uint64_t) *((str) + 0) << 56);    \
}

/* Macros used for loops unrolling */

#define SHA256_SCR(i)                          \
{                                              \
    w[i] =  SHA256_F4(w[i - 2]) + w[i - 7]     \
          + SHA256_F3(w[i - 15]) + w[i - 16];  \
}

#define SHA512_SCR(i)                          \
{                                              \
    w[i] =  SHA512_F4(w[i - 2]) + w[i - 7]     \
          + SHA512_F3(w[i - 15]) + w[i - 16];  \
}

#define SHA256_EXP(a, b, c, d, e, f, g, h, j)               \
{                                                           \
    t1 = wv[h] + SHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
         + sha256_k[j] + w[j];                              \
    t2 = SHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[h] = t1 + t2;                                        \
}

#define SHA512_EXP(a, b, c, d, e, f, g ,h, j)               \
{                                                           \
    t1 = wv[h] + SHA512_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
         + sha512_k[j] + w[j];                              \
    t2 = SHA512_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[h] = t1 + t2;                                        \
}

uint32_t sha224_h0[8] =
            {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
             0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};

uint32_t sha256_h0[8] =
            {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

uint64_t sha384_h0[8] =
            {0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL,
             0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL,
             0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
             0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL};

uint64_t sha512_h0[8] =
            {0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
             0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
             0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
             0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};

uint32_t sha256_k[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

uint64_t sha512_k[80] =
            {0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
             0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
             0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
             0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
             0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
             0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
             0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
             0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
             0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
             0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
             0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
             0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
             0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
             0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
             0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
             0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
             0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
             0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
             0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
             0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
             0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
             0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
             0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
             0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
             0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
             0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
             0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
             0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
             0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
             0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
             0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
             0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
             0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
             0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
             0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
             0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
             0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
             0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
             0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
             0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

/* SHA-256 functions */

void Utilities::sha256_transf(sha256_ctx *ctx, unsigned char *message,
                   uint32_t block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    unsigned char *sub_block;
    uint32_t i;

#ifndef UNROLL_LOOPS
    int32_t j;
#endif

    for (i = 1; i <= block_nb; i++) {
        sub_block = message + ((i - 1) << 6);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
#else
        PACK32(&sub_block[ 0], &w[ 0]); PACK32(&sub_block[ 4], &w[ 1]);
        PACK32(&sub_block[ 8], &w[ 2]); PACK32(&sub_block[12], &w[ 3]);
        PACK32(&sub_block[16], &w[ 4]); PACK32(&sub_block[20], &w[ 5]);
        PACK32(&sub_block[24], &w[ 6]); PACK32(&sub_block[28], &w[ 7]);
        PACK32(&sub_block[32], &w[ 8]); PACK32(&sub_block[36], &w[ 9]);
        PACK32(&sub_block[40], &w[10]); PACK32(&sub_block[44], &w[11]);
        PACK32(&sub_block[48], &w[12]); PACK32(&sub_block[52], &w[13]);
        PACK32(&sub_block[56], &w[14]); PACK32(&sub_block[60], &w[15]);

        SHA256_SCR(16); SHA256_SCR(17); SHA256_SCR(18); SHA256_SCR(19);
        SHA256_SCR(20); SHA256_SCR(21); SHA256_SCR(22); SHA256_SCR(23);
        SHA256_SCR(24); SHA256_SCR(25); SHA256_SCR(26); SHA256_SCR(27);
        SHA256_SCR(28); SHA256_SCR(29); SHA256_SCR(30); SHA256_SCR(31);
        SHA256_SCR(32); SHA256_SCR(33); SHA256_SCR(34); SHA256_SCR(35);
        SHA256_SCR(36); SHA256_SCR(37); SHA256_SCR(38); SHA256_SCR(39);
        SHA256_SCR(40); SHA256_SCR(41); SHA256_SCR(42); SHA256_SCR(43);
        SHA256_SCR(44); SHA256_SCR(45); SHA256_SCR(46); SHA256_SCR(47);
        SHA256_SCR(48); SHA256_SCR(49); SHA256_SCR(50); SHA256_SCR(51);
        SHA256_SCR(52); SHA256_SCR(53); SHA256_SCR(54); SHA256_SCR(55);
        SHA256_SCR(56); SHA256_SCR(57); SHA256_SCR(58); SHA256_SCR(59);
        SHA256_SCR(60); SHA256_SCR(61); SHA256_SCR(62); SHA256_SCR(63);

        wv[0] = ctx->h[0]; wv[1] = ctx->h[1];
        wv[2] = ctx->h[2]; wv[3] = ctx->h[3];
        wv[4] = ctx->h[4]; wv[5] = ctx->h[5];
        wv[6] = ctx->h[6]; wv[7] = ctx->h[7];

        SHA256_EXP(0,1,2,3,4,5,6,7, 0); SHA256_EXP(7,0,1,2,3,4,5,6, 1);
        SHA256_EXP(6,7,0,1,2,3,4,5, 2); SHA256_EXP(5,6,7,0,1,2,3,4, 3);
        SHA256_EXP(4,5,6,7,0,1,2,3, 4); SHA256_EXP(3,4,5,6,7,0,1,2, 5);
        SHA256_EXP(2,3,4,5,6,7,0,1, 6); SHA256_EXP(1,2,3,4,5,6,7,0, 7);
        SHA256_EXP(0,1,2,3,4,5,6,7, 8); SHA256_EXP(7,0,1,2,3,4,5,6, 9);
        SHA256_EXP(6,7,0,1,2,3,4,5,10); SHA256_EXP(5,6,7,0,1,2,3,4,11);
        SHA256_EXP(4,5,6,7,0,1,2,3,12); SHA256_EXP(3,4,5,6,7,0,1,2,13);
        SHA256_EXP(2,3,4,5,6,7,0,1,14); SHA256_EXP(1,2,3,4,5,6,7,0,15);
        SHA256_EXP(0,1,2,3,4,5,6,7,16); SHA256_EXP(7,0,1,2,3,4,5,6,17);
        SHA256_EXP(6,7,0,1,2,3,4,5,18); SHA256_EXP(5,6,7,0,1,2,3,4,19);
        SHA256_EXP(4,5,6,7,0,1,2,3,20); SHA256_EXP(3,4,5,6,7,0,1,2,21);
        SHA256_EXP(2,3,4,5,6,7,0,1,22); SHA256_EXP(1,2,3,4,5,6,7,0,23);
        SHA256_EXP(0,1,2,3,4,5,6,7,24); SHA256_EXP(7,0,1,2,3,4,5,6,25);
        SHA256_EXP(6,7,0,1,2,3,4,5,26); SHA256_EXP(5,6,7,0,1,2,3,4,27);
        SHA256_EXP(4,5,6,7,0,1,2,3,28); SHA256_EXP(3,4,5,6,7,0,1,2,29);
        SHA256_EXP(2,3,4,5,6,7,0,1,30); SHA256_EXP(1,2,3,4,5,6,7,0,31);
        SHA256_EXP(0,1,2,3,4,5,6,7,32); SHA256_EXP(7,0,1,2,3,4,5,6,33);
        SHA256_EXP(6,7,0,1,2,3,4,5,34); SHA256_EXP(5,6,7,0,1,2,3,4,35);
        SHA256_EXP(4,5,6,7,0,1,2,3,36); SHA256_EXP(3,4,5,6,7,0,1,2,37);
        SHA256_EXP(2,3,4,5,6,7,0,1,38); SHA256_EXP(1,2,3,4,5,6,7,0,39);
        SHA256_EXP(0,1,2,3,4,5,6,7,40); SHA256_EXP(7,0,1,2,3,4,5,6,41);
        SHA256_EXP(6,7,0,1,2,3,4,5,42); SHA256_EXP(5,6,7,0,1,2,3,4,43);
        SHA256_EXP(4,5,6,7,0,1,2,3,44); SHA256_EXP(3,4,5,6,7,0,1,2,45);
        SHA256_EXP(2,3,4,5,6,7,0,1,46); SHA256_EXP(1,2,3,4,5,6,7,0,47);
        SHA256_EXP(0,1,2,3,4,5,6,7,48); SHA256_EXP(7,0,1,2,3,4,5,6,49);
        SHA256_EXP(6,7,0,1,2,3,4,5,50); SHA256_EXP(5,6,7,0,1,2,3,4,51);
        SHA256_EXP(4,5,6,7,0,1,2,3,52); SHA256_EXP(3,4,5,6,7,0,1,2,53);
        SHA256_EXP(2,3,4,5,6,7,0,1,54); SHA256_EXP(1,2,3,4,5,6,7,0,55);
        SHA256_EXP(0,1,2,3,4,5,6,7,56); SHA256_EXP(7,0,1,2,3,4,5,6,57);
        SHA256_EXP(6,7,0,1,2,3,4,5,58); SHA256_EXP(5,6,7,0,1,2,3,4,59);
        SHA256_EXP(4,5,6,7,0,1,2,3,60); SHA256_EXP(3,4,5,6,7,0,1,2,61);
        SHA256_EXP(2,3,4,5,6,7,0,1,62); SHA256_EXP(1,2,3,4,5,6,7,0,63);

        ctx->h[0] += wv[0]; ctx->h[1] += wv[1];
        ctx->h[2] += wv[2]; ctx->h[3] += wv[3];
        ctx->h[4] += wv[4]; ctx->h[5] += wv[5];
        ctx->h[6] += wv[6]; ctx->h[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}

void Utilities::sha256(unsigned char *message, uint32_t len, unsigned char *digest)
{
    sha256_ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, message, len);
    sha256_final(&ctx, digest);
}

void Utilities::sha256_init(sha256_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int32_t i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha256_h0[i];
    }
#else
    ctx->h[0] = sha256_h0[0]; ctx->h[1] = sha256_h0[1];
    ctx->h[2] = sha256_h0[2]; ctx->h[3] = sha256_h0[3];
    ctx->h[4] = sha256_h0[4]; ctx->h[5] = sha256_h0[5];
    ctx->h[6] = sha256_h0[6]; ctx->h[7] = sha256_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void Utilities::sha256_update(sha256_ctx *ctx, unsigned char *message, uint32_t len)
{
    uint32_t block_nb;
    uint32_t new_len, rem_len;
    unsigned char *shifted_message;

    rem_len = SHA256_BLOCK_SIZE - ctx->len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA256_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(ctx, ctx->block, 1);
    sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void Utilities::sha256_final(sha256_ctx *ctx, unsigned char *digest)
{
    uint32_t block_nb;
    uint32_t pm_len;
    uint32_t len_b;

#ifndef UNROLL_LOOPS
    int32_t i;
#endif

    block_nb = (1 + ((SHA256_BLOCK_SIZE - 9)
                     < (ctx->len % SHA256_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha256_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
#else
   UNPACK32(ctx->h[0], &digest[ 0]);
   UNPACK32(ctx->h[1], &digest[ 4]);
   UNPACK32(ctx->h[2], &digest[ 8]);
   UNPACK32(ctx->h[3], &digest[12]);
   UNPACK32(ctx->h[4], &digest[16]);
   UNPACK32(ctx->h[5], &digest[20]);
   UNPACK32(ctx->h[6], &digest[24]);
   UNPACK32(ctx->h[7], &digest[28]);
#endif /* !UNROLL_LOOPS */
}

/* SHA 512 functions*/

void Utilities::sha512_transf(sha512_ctx *ctx, unsigned char *message,
                   uint32_t block_nb)
{
    uint64_t w[80];
    uint64_t wv[8];
    uint64_t t1, t2;
    unsigned char *sub_block;
    uint32_t i;

#ifndef UNROLL_LOOPS
    int32_t j;
#endif

    for (i = 1; i <= block_nb; i++) {
        sub_block = message + ((i - 1) << 7);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK64(&sub_block[j << 3], &w[j]);
        }

        for (j = 16; j < 80; j++) {
            SHA512_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 80; j++) {
            t1 = wv[7] + SHA512_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha512_k[j] + w[j];
            t2 = SHA512_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
#else
        PACK64(&sub_block[  0], &w[ 0]); PACK64(&sub_block[  8], &w[ 1]);
        PACK64(&sub_block[ 16], &w[ 2]); PACK64(&sub_block[ 24], &w[ 3]);
        PACK64(&sub_block[ 32], &w[ 4]); PACK64(&sub_block[ 40], &w[ 5]);
        PACK64(&sub_block[ 48], &w[ 6]); PACK64(&sub_block[ 56], &w[ 7]);
        PACK64(&sub_block[ 64], &w[ 8]); PACK64(&sub_block[ 72], &w[ 9]);
        PACK64(&sub_block[ 80], &w[10]); PACK64(&sub_block[ 88], &w[11]);
        PACK64(&sub_block[ 96], &w[12]); PACK64(&sub_block[104], &w[13]);
        PACK64(&sub_block[112], &w[14]); PACK64(&sub_block[120], &w[15]);

        SHA512_SCR(16); SHA512_SCR(17); SHA512_SCR(18); SHA512_SCR(19);
        SHA512_SCR(20); SHA512_SCR(21); SHA512_SCR(22); SHA512_SCR(23);
        SHA512_SCR(24); SHA512_SCR(25); SHA512_SCR(26); SHA512_SCR(27);
        SHA512_SCR(28); SHA512_SCR(29); SHA512_SCR(30); SHA512_SCR(31);
        SHA512_SCR(32); SHA512_SCR(33); SHA512_SCR(34); SHA512_SCR(35);
        SHA512_SCR(36); SHA512_SCR(37); SHA512_SCR(38); SHA512_SCR(39);
        SHA512_SCR(40); SHA512_SCR(41); SHA512_SCR(42); SHA512_SCR(43);
        SHA512_SCR(44); SHA512_SCR(45); SHA512_SCR(46); SHA512_SCR(47);
        SHA512_SCR(48); SHA512_SCR(49); SHA512_SCR(50); SHA512_SCR(51);
        SHA512_SCR(52); SHA512_SCR(53); SHA512_SCR(54); SHA512_SCR(55);
        SHA512_SCR(56); SHA512_SCR(57); SHA512_SCR(58); SHA512_SCR(59);
        SHA512_SCR(60); SHA512_SCR(61); SHA512_SCR(62); SHA512_SCR(63);
        SHA512_SCR(64); SHA512_SCR(65); SHA512_SCR(66); SHA512_SCR(67);
        SHA512_SCR(68); SHA512_SCR(69); SHA512_SCR(70); SHA512_SCR(71);
        SHA512_SCR(72); SHA512_SCR(73); SHA512_SCR(74); SHA512_SCR(75);
        SHA512_SCR(76); SHA512_SCR(77); SHA512_SCR(78); SHA512_SCR(79);

        wv[0] = ctx->h[0]; wv[1] = ctx->h[1];
        wv[2] = ctx->h[2]; wv[3] = ctx->h[3];
        wv[4] = ctx->h[4]; wv[5] = ctx->h[5];
        wv[6] = ctx->h[6]; wv[7] = ctx->h[7];

        SHA512_EXP(0,1,2,3,4,5,6,7, 0); SHA512_EXP(7,0,1,2,3,4,5,6, 1);
        SHA512_EXP(6,7,0,1,2,3,4,5, 2); SHA512_EXP(5,6,7,0,1,2,3,4, 3);
        SHA512_EXP(4,5,6,7,0,1,2,3, 4); SHA512_EXP(3,4,5,6,7,0,1,2, 5);
        SHA512_EXP(2,3,4,5,6,7,0,1, 6); SHA512_EXP(1,2,3,4,5,6,7,0, 7);
        SHA512_EXP(0,1,2,3,4,5,6,7, 8); SHA512_EXP(7,0,1,2,3,4,5,6, 9);
        SHA512_EXP(6,7,0,1,2,3,4,5,10); SHA512_EXP(5,6,7,0,1,2,3,4,11);
        SHA512_EXP(4,5,6,7,0,1,2,3,12); SHA512_EXP(3,4,5,6,7,0,1,2,13);
        SHA512_EXP(2,3,4,5,6,7,0,1,14); SHA512_EXP(1,2,3,4,5,6,7,0,15);
        SHA512_EXP(0,1,2,3,4,5,6,7,16); SHA512_EXP(7,0,1,2,3,4,5,6,17);
        SHA512_EXP(6,7,0,1,2,3,4,5,18); SHA512_EXP(5,6,7,0,1,2,3,4,19);
        SHA512_EXP(4,5,6,7,0,1,2,3,20); SHA512_EXP(3,4,5,6,7,0,1,2,21);
        SHA512_EXP(2,3,4,5,6,7,0,1,22); SHA512_EXP(1,2,3,4,5,6,7,0,23);
        SHA512_EXP(0,1,2,3,4,5,6,7,24); SHA512_EXP(7,0,1,2,3,4,5,6,25);
        SHA512_EXP(6,7,0,1,2,3,4,5,26); SHA512_EXP(5,6,7,0,1,2,3,4,27);
        SHA512_EXP(4,5,6,7,0,1,2,3,28); SHA512_EXP(3,4,5,6,7,0,1,2,29);
        SHA512_EXP(2,3,4,5,6,7,0,1,30); SHA512_EXP(1,2,3,4,5,6,7,0,31);
        SHA512_EXP(0,1,2,3,4,5,6,7,32); SHA512_EXP(7,0,1,2,3,4,5,6,33);
        SHA512_EXP(6,7,0,1,2,3,4,5,34); SHA512_EXP(5,6,7,0,1,2,3,4,35);
        SHA512_EXP(4,5,6,7,0,1,2,3,36); SHA512_EXP(3,4,5,6,7,0,1,2,37);
        SHA512_EXP(2,3,4,5,6,7,0,1,38); SHA512_EXP(1,2,3,4,5,6,7,0,39);
        SHA512_EXP(0,1,2,3,4,5,6,7,40); SHA512_EXP(7,0,1,2,3,4,5,6,41);
        SHA512_EXP(6,7,0,1,2,3,4,5,42); SHA512_EXP(5,6,7,0,1,2,3,4,43);
        SHA512_EXP(4,5,6,7,0,1,2,3,44); SHA512_EXP(3,4,5,6,7,0,1,2,45);
        SHA512_EXP(2,3,4,5,6,7,0,1,46); SHA512_EXP(1,2,3,4,5,6,7,0,47);
        SHA512_EXP(0,1,2,3,4,5,6,7,48); SHA512_EXP(7,0,1,2,3,4,5,6,49);
        SHA512_EXP(6,7,0,1,2,3,4,5,50); SHA512_EXP(5,6,7,0,1,2,3,4,51);
        SHA512_EXP(4,5,6,7,0,1,2,3,52); SHA512_EXP(3,4,5,6,7,0,1,2,53);
        SHA512_EXP(2,3,4,5,6,7,0,1,54); SHA512_EXP(1,2,3,4,5,6,7,0,55);
        SHA512_EXP(0,1,2,3,4,5,6,7,56); SHA512_EXP(7,0,1,2,3,4,5,6,57);
        SHA512_EXP(6,7,0,1,2,3,4,5,58); SHA512_EXP(5,6,7,0,1,2,3,4,59);
        SHA512_EXP(4,5,6,7,0,1,2,3,60); SHA512_EXP(3,4,5,6,7,0,1,2,61);
        SHA512_EXP(2,3,4,5,6,7,0,1,62); SHA512_EXP(1,2,3,4,5,6,7,0,63);
        SHA512_EXP(0,1,2,3,4,5,6,7,64); SHA512_EXP(7,0,1,2,3,4,5,6,65);
        SHA512_EXP(6,7,0,1,2,3,4,5,66); SHA512_EXP(5,6,7,0,1,2,3,4,67);
        SHA512_EXP(4,5,6,7,0,1,2,3,68); SHA512_EXP(3,4,5,6,7,0,1,2,69);
        SHA512_EXP(2,3,4,5,6,7,0,1,70); SHA512_EXP(1,2,3,4,5,6,7,0,71);
        SHA512_EXP(0,1,2,3,4,5,6,7,72); SHA512_EXP(7,0,1,2,3,4,5,6,73);
        SHA512_EXP(6,7,0,1,2,3,4,5,74); SHA512_EXP(5,6,7,0,1,2,3,4,75);
        SHA512_EXP(4,5,6,7,0,1,2,3,76); SHA512_EXP(3,4,5,6,7,0,1,2,77);
        SHA512_EXP(2,3,4,5,6,7,0,1,78); SHA512_EXP(1,2,3,4,5,6,7,0,79);

        ctx->h[0] += wv[0]; ctx->h[1] += wv[1];
        ctx->h[2] += wv[2]; ctx->h[3] += wv[3];
        ctx->h[4] += wv[4]; ctx->h[5] += wv[5];
        ctx->h[6] += wv[6]; ctx->h[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}

void Utilities::sha512(unsigned char *message, uint32_t len, unsigned char *digest)
{
    sha512_ctx ctx;

    sha512_init(&ctx);
    sha512_update(&ctx, message, len);
    sha512_final(&ctx, digest);
}

void Utilities::sha512_init(sha512_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int32_t i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha512_h0[i];
    }
#else
    ctx->h[0] = sha512_h0[0]; ctx->h[1] = sha512_h0[1];
    ctx->h[2] = sha512_h0[2]; ctx->h[3] = sha512_h0[3];
    ctx->h[4] = sha512_h0[4]; ctx->h[5] = sha512_h0[5];
    ctx->h[6] = sha512_h0[6]; ctx->h[7] = sha512_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void Utilities::sha512_update(sha512_ctx *ctx, unsigned char *message, uint32_t len)
{
    uint32_t block_nb;
    uint32_t new_len, rem_len;
    unsigned char *shifted_message;

    rem_len = SHA512_BLOCK_SIZE - ctx->len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA512_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA512_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha512_transf(ctx, ctx->block, 1);
    sha512_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA512_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 7],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 7;
}

void Utilities::sha512_final(sha512_ctx *ctx, unsigned char *digest)
{
    uint32_t block_nb;
    uint32_t pm_len;
    uint32_t len_b;

#ifndef UNROLL_LOOPS
    int32_t i;
#endif

    block_nb = 1 + ((SHA512_BLOCK_SIZE - 17)
                     < (ctx->len % SHA512_BLOCK_SIZE)) ;

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 7;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha512_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK64(ctx->h[i], &digest[i << 3]);
    }
#else
    UNPACK64(ctx->h[0], &digest[ 0]);
    UNPACK64(ctx->h[1], &digest[ 8]);
    UNPACK64(ctx->h[2], &digest[16]);
    UNPACK64(ctx->h[3], &digest[24]);
    UNPACK64(ctx->h[4], &digest[32]);
    UNPACK64(ctx->h[5], &digest[40]);
    UNPACK64(ctx->h[6], &digest[48]);
    UNPACK64(ctx->h[7], &digest[56]);
#endif /* !UNROLL_LOOPS */
}

/* SHA-384 functions */

void Utilities::sha384(unsigned char *message, uint32_t len, unsigned char *digest)
{
    sha384_ctx ctx;

    sha384_init(&ctx);
    sha384_update(&ctx, message, len);
    sha384_final(&ctx, digest);
}

void Utilities::sha384_init(sha384_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int32_t i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha384_h0[i];
    }
#else
    ctx->h[0] = sha384_h0[0]; ctx->h[1] = sha384_h0[1];
    ctx->h[2] = sha384_h0[2]; ctx->h[3] = sha384_h0[3];
    ctx->h[4] = sha384_h0[4]; ctx->h[5] = sha384_h0[5];
    ctx->h[6] = sha384_h0[6]; ctx->h[7] = sha384_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void Utilities::sha384_update(sha384_ctx *ctx, unsigned char *message, uint32_t len)
{
    uint32_t block_nb;
    uint32_t new_len, rem_len;
    unsigned char *shifted_message;

    rem_len = SHA384_BLOCK_SIZE - ctx->len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA384_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA384_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha512_transf(ctx, ctx->block, 1);
    sha512_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA384_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 7],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 7;
}

void Utilities::sha384_final(sha384_ctx *ctx, unsigned char *digest)
{
    uint32_t block_nb;
    uint32_t pm_len;
    uint32_t len_b;

#ifndef UNROLL_LOOPS
    int32_t i;
#endif

    block_nb = (1 + ((SHA384_BLOCK_SIZE - 17)
                     < (ctx->len % SHA384_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 7;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha512_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 6; i++) {
        UNPACK64(ctx->h[i], &digest[i << 3]);
    }
#else
    UNPACK64(ctx->h[0], &digest[ 0]);
    UNPACK64(ctx->h[1], &digest[ 8]);
    UNPACK64(ctx->h[2], &digest[16]);
    UNPACK64(ctx->h[3], &digest[24]);
    UNPACK64(ctx->h[4], &digest[32]);
    UNPACK64(ctx->h[5], &digest[40]);
#endif /* !UNROLL_LOOPS */
}

/* SHA-224 functions */

void Utilities::sha224(unsigned char *message, uint32_t len, unsigned char *digest)
{
    sha224_ctx ctx;

    sha224_init(&ctx);
    sha224_update(&ctx, message, len);
    sha224_final(&ctx, digest);
}

void Utilities::sha224_init(sha224_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int32_t i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha224_h0[i];
    }
#else
    ctx->h[0] = sha224_h0[0]; ctx->h[1] = sha224_h0[1];
    ctx->h[2] = sha224_h0[2]; ctx->h[3] = sha224_h0[3];
    ctx->h[4] = sha224_h0[4]; ctx->h[5] = sha224_h0[5];
    ctx->h[6] = sha224_h0[6]; ctx->h[7] = sha224_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void Utilities::sha224_update(sha224_ctx *ctx, unsigned char *message, uint32_t len)
{
    uint32_t block_nb;
    uint32_t new_len, rem_len;
    unsigned char *shifted_message;

    rem_len = SHA224_BLOCK_SIZE - ctx->len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA224_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA224_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(ctx, ctx->block, 1);
    sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA224_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void Utilities::sha224_final(sha224_ctx *ctx, unsigned char *digest)
{
    uint32_t block_nb;
    uint32_t pm_len;
    uint32_t len_b;

#ifndef UNROLL_LOOPS
    int32_t i;
#endif

    block_nb = (1 + ((SHA224_BLOCK_SIZE - 9)
                     < (ctx->len % SHA224_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha256_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 7; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
#else
   UNPACK32(ctx->h[0], &digest[ 0]);
   UNPACK32(ctx->h[1], &digest[ 4]);
   UNPACK32(ctx->h[2], &digest[ 8]);
   UNPACK32(ctx->h[3], &digest[12]);
   UNPACK32(ctx->h[4], &digest[16]);
   UNPACK32(ctx->h[5], &digest[20]);
   UNPACK32(ctx->h[6], &digest[24]);
#endif /* !UNROLL_LOOPS */
}


string Utilities::sha512sum(unsigned char *msg, uint32_t len)
{
	unsigned char SHA512Result[64];
	sha512(msg,len,SHA512Result);
	string SHA512Sum = "";

	for(uint32_t i = 0; i < 64; ++i)
	{
		SHA512Sum += ((SHA512Result[i] >> 4) < 10 ? (SHA512Result[i] >> 4) + '0' : (SHA512Result[i] >> 4) + ('a' - 10));
		SHA512Sum += ((SHA512Result[i] & 0xF) < 10 ? (SHA512Result[i] & 0xF) + '0' : (SHA512Result[i] & 0xF) + ('a' - 10));
	}
	return SHA512Sum;

}

// ENDOF SHA512




