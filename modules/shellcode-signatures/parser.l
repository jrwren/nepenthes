/* $Id$ */
%{
#include <string.h>
#include "y.tab.h"

inline void string_reset();
inline char *string_get_buffer();
inline int string_get_len();
int line_number = 1;

static inline void string_append(char *, int);
static char *string_buffer = NULL;
static int string_len = 0;
static int string_cap = 0;

%}

LETTER [a-zA-Z]
DIGIT [0-9]
DELIM " "|"\t"|"\r"
NEWLINE "\n"

%x comment string

%%


"("									{ return SC_LPAR; }
")"									{ return SC_RPAR; }
"{"									{ return SC_LBR; }
"}"									{ return SC_RBR; }

";"									{ return SC_SEMI; }
":"									{ return SC_COLON; }
","									{ return SC_COMMA; }

"pattern"							{ return SC_PATTERN; }
"type"								{ return SC_TYPE; }
"mapping"							{ return SC_MAPPING; }
"flags"								{ return SC_FLAGS; }

"none"								{ return SC_NONE; }

"xor"								{ return SC_XOR; }
"linkxor"							{ return SC_LINKXOR; }
"konstanzxor"						{ return SC_KONSTANZXOR; }
"leimbachxor"						{ return SC_LEIMBACHXOR; }
"bindshell"							{ return SC_BIND_SHELL; }
"connectbackshell"					{ return SC_CONNECTBACK_SHELL; }
"connectbackfiletransfer"			{ return SC_CONNECTBACK_FILETRANSFER; }
"execute"							{ return SC_EXECUTE; }
"download"							{ return SC_DOWNLOAD; }
"url"								{ return SC_URL; }
"connectbacklinkfiletransfer"		{ return SC_CONNECTBACK_LINK_FILETRANSFER; }
"bindlinkfiletransfer"				{ return SC_BIND_LINK_FILETRANSFER; }

"key"								{ return SC_KEY; }
"size"								{ return SC_SIZE; }
"sizeinvert"						{ return SC_SIZEINVERT; }
"host"								{ return SC_HOST; }
"port"								{ return SC_PORT; }
"command"							{ return SC_COMMAND; }
"uri"								{ return SC_URI; }

{LETTER}({LETTER}|{DIGIT}|"_")*		{ string_append(yytext, strlen(yytext)); return SC_ID; }


"/*"								{ BEGIN(comment); }
<comment>[^*\n]*					{ }
<comment>"*"+"/"					{ BEGIN(INITIAL); }
<comment>"*"*						{ }
<comment>"\n"						{ line_number++; }

"\""								{ BEGIN(string); }
<string>"\""						{ BEGIN(INITIAL); return SC_STRING; }
<string>"\\n"						{ string_append("\n", 1); }
<string>"\\r"						{ string_append("\r", 1); }
<string>"\\t"						{ string_append("\t", 1); }
<string>"\\\""						{ string_append("\"", 1); }
<string>"\\0"						{ string_append("\0", 1); }
<string>"\\\\"						{ string_append("\\", 1); }
<string>"\\x"[0-9a-fA-F]{2}			{
	char hexval[] = {'0', 'x', *(yytext + 2), *(yytext + 3), '\0'};
	unsigned int c;

	sscanf(hexval, "%x", &c);
	string_append((char *)&c, 1);
}
<string>[^\"\\]*					{ string_append(yytext, strlen(yytext)); }

"//"[^\n]*							{ }

{DELIM}*							{ }
{NEWLINE}							{ line_number++; }

%%

void string_append(char *s, int len)
{
	while( string_len + len > string_cap )
	{
		printf("enlarging from %d", string_cap);

		if( string_cap == 0 )
			string_cap = 0x10;
		else
			string_cap *= 2;
			
		printf(" to %d\n", string_cap);
		
		string_buffer = realloc(string_buffer, string_cap);
	}
	
	memcpy(string_buffer + string_len, s, len);
	string_len += len;
}

char *string_get_buffer()
{
	return string_buffer;
}

int string_get_len()
{
	return string_len;
}

void string_reset()
{
	string_len = 0;
}
