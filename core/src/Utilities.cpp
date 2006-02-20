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


#include "Utilities.hpp"
#include "LogManager.hpp"

using namespace nepenthes;

/*------------ macros for storing/extracting msb first words -------------*/

#define GET_32BIT(cp) (((unsigned long)(unsigned char)(cp)[0] << 24) | \
                       ((unsigned long)(unsigned char)(cp)[1] << 16) | \
                       ((unsigned long)(unsigned char)(cp)[2] << 8) |  \
                       ((unsigned long)(unsigned char)(cp)[3]))

#define GET_16BIT(cp) (((unsigned long)(unsigned char)(cp)[0] << 8) | \
                       ((unsigned long)(unsigned char)(cp)[1]))

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
  (((unsigned long)(unsigned char)(cp)[0]) |      \
  ((unsigned long)(unsigned char)(cp)[1] << 8) |  \
  ((unsigned long)(unsigned char)(cp)[2] << 16) | \
  ((unsigned long)(unsigned char)(cp)[3] << 24))

#define GET_16BIT_LSB_FIRST(cp)                  \
  (((unsigned long)(unsigned char)(cp)[0]) |     \
  ((unsigned long)(unsigned char)(cp)[1] << 8))

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






Utilities::Utilities()
{

}

Utilities::~Utilities()
{
}


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



string Utilities::md5sum(char *msg, int len)
{

	struct MD5Context MD5Struct;
	MD5Init(&MD5Struct);

	unsigned char MD5Result[16];
	MD5Update(&MD5Struct,(const unsigned char *)msg,len);
	MD5Final(MD5Result,&MD5Struct);

	string MD5Sum = "";

	//char sum[32];
	//int i;

	for(unsigned int i = 0; i < 16; ++i)
	{
		MD5Sum += ((MD5Result[i] >> 4) < 10 ? (MD5Result[i] >> 4) + '0' : (MD5Result[i] >> 4) + ('a' - 10));
		MD5Sum += ((MD5Result[i] & 0xF) < 10 ? (MD5Result[i] & 0xF) + '0' : (MD5Result[i] & 0xF) + ('a' - 10));
	}
	return MD5Sum;
}


void Utilities::hexdump(byte *data, uint len)
{
	char conv[] = "0123456789abcdef";

	printf("=------------------[ hexdump(0x%08x , 0x%08x) ]-------------------=\n", (unsigned int)data, len);
	for( unsigned int i = 0; i < len; i += 0x10 )
	{
		printf("0x%04x  ", i);


		unsigned int j;
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

void Utilities::hexdump(unsigned int mask, byte *data, uint len)
{
	char conv[] = "0123456789abcdef";

	string md5 = "hexdump/";
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

	g_Nepenthes->getLogMgr()->logf(mask,"=------------------[ hexdump(0x%08x , 0x%08x) ]-------------------=\n", (unsigned int)data, len);
	for( unsigned int i = 0; i < len; i += 0x10 )
	{
		string line;
//		printf("0x%04x  ", i);
		line = "";

		unsigned int j;
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

void Utilities::b64enc(unsigned char *in, int inlen, unsigned char *out)
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

void Utilities::b64dec(unsigned char *in, int inlen, unsigned char *out)
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

int Utilities::b64encode_len(unsigned char *in)
{
	int l = strlen((char *)in);
	return 4*((l+2)/3);
}

int Utilities::b64encode_len(unsigned char *in, int l)
{
    return 4*((l+2)/3);
}


int Utilities::b64decode_len(unsigned char *in)
{
	int l = strlen((char *)in);
	return 3*((l+3)/4);
}

unsigned char *Utilities::b64encode_alloc(unsigned char *in)
{
	int l = b64encode_len(in);
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

unsigned char *Utilities::b64encode_alloc(unsigned char *in, int inlen)
{
	int l = b64encode_len(in,inlen);
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
	int inlen = strlen((char *)in);
	while ( inlen > 0 )
	{
		b64enc(in, inlen, out);
		inlen -= 3;
		in += 3;
		out += 4;
	}
}

void Utilities::b64encode(unsigned char *in,int inlen, unsigned char *out)
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
	int l = b64decode_len(in);
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
	int inlen = strlen((char *)in);
	while ( inlen > 0 )
	{
		b64dec(in, inlen, out);
		inlen -= 4;
		in += 4;
		out += 3;
	}
}


/*
** Function: hmac_md5
*/

void Utilities::hmac_md5(
unsigned char*  text,                /* pointer to data stream */
int             text_len,            /* length of data stream */
unsigned char*  key,                 /* pointer to authentication key */
int             key_len,             /* length of authentication key */
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
        int i;
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
int             text_len,            /* length of data stream */
unsigned char*  key,                 /* pointer to authentication key */
int             key_len)             /* length of authentication key */
{
	
	unsigned char MD5Result[16];
	string MD5Sum;
	hmac_md5(text, text_len, key, key_len,MD5Result);
	for(unsigned int i = 0; i < 16; ++i)
	{
		MD5Sum += ((MD5Result[i] >> 4) < 10 ? (MD5Result[i] >> 4) + '0' : (MD5Result[i] >> 4) + ('a' - 10));
		MD5Sum += ((MD5Result[i] & 0xF) < 10 ? (MD5Result[i] & 0xF) + '0' : (MD5Result[i] & 0xF) + ('a' - 10));
	}
	return MD5Sum;

}
