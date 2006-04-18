/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SC_ID = 258,
     SC_LPAR = 259,
     SC_RPAR = 260,
     SC_LBR = 261,
     SC_RBR = 262,
     SC_COMMA = 263,
     SC_SEMI = 264,
     SC_COLON = 265,
     SC_NONE = 266,
     SC_FLAGS = 267,
     SC_PATTERN = 268,
     SC_TYPE = 269,
     SC_MAPPING = 270,
     SC_STRING = 271,
     SC_XOR = 272,
     SC_LINKXOR = 273,
     SC_KONSTANZXOR = 274,
     SC_LEIMBACHXOR = 275,
     SC_ALPHANUMERICXOR = 276,
     SC_BIND_SHELL = 277,
     SC_CONNECTBACK_SHELL = 278,
     SC_CONNECTBACK_FILETRANSFER = 279,
     SC_BIND_FILETRANSFER = 280,
     SC_EXECUTE = 281,
     SC_DOWNLOAD = 282,
     SC_URL = 283,
     SC_BASE64 = 284,
     SC_KEY = 285,
     SC_SUBKEY = 286,
     SC_SIZE = 287,
     SC_SIZEINVERT = 288,
     SC_HOST = 289,
     SC_PORT = 290,
     SC_COMMAND = 291,
     SC_URI = 292,
     SC_DECODER = 293,
     SC_PRELOAD = 294,
     SC_POSTLOAD = 295,
     SC_HOSTKEY = 296,
     SC_PORTKEY = 297,
     SC_PAYLOAD = 298
   };
#endif
#define SC_ID 258
#define SC_LPAR 259
#define SC_RPAR 260
#define SC_LBR 261
#define SC_RBR 262
#define SC_COMMA 263
#define SC_SEMI 264
#define SC_COLON 265
#define SC_NONE 266
#define SC_FLAGS 267
#define SC_PATTERN 268
#define SC_TYPE 269
#define SC_MAPPING 270
#define SC_STRING 271
#define SC_XOR 272
#define SC_LINKXOR 273
#define SC_KONSTANZXOR 274
#define SC_LEIMBACHXOR 275
#define SC_ALPHANUMERICXOR 276
#define SC_BIND_SHELL 277
#define SC_CONNECTBACK_SHELL 278
#define SC_CONNECTBACK_FILETRANSFER 279
#define SC_BIND_FILETRANSFER 280
#define SC_EXECUTE 281
#define SC_DOWNLOAD 282
#define SC_URL 283
#define SC_BASE64 284
#define SC_KEY 285
#define SC_SUBKEY 286
#define SC_SIZE 287
#define SC_SIZEINVERT 288
#define SC_HOST 289
#define SC_PORT 290
#define SC_COMMAND 291
#define SC_URI 292
#define SC_DECODER 293
#define SC_PRELOAD 294
#define SC_POSTLOAD 295
#define SC_HOSTKEY 296
#define SC_PORTKEY 297
#define SC_PAYLOAD 298




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



