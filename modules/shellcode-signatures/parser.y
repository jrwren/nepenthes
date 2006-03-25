/* $Id$ */
%{
   #include "parser-shared.h"

struct shellcode *shellcodes = NULL;


	extern char *yytext;

	extern int line_number;

	
%}


%token SC_ID SC_LPAR SC_RPAR SC_LBR SC_RBR SC_COMMA SC_SEMI SC_COLON SC_NONE SC_FLAGS SC_PATTERN SC_TYPE SC_MAPPING SC_STRING 
SC_XOR SC_LINKXOR SC_KONSTANZXOR SC_LEIMBACHXOR 
SC_BIND_SHELL 
SC_CONNECTBACK_SHELL 
SC_CONNECTBACK_FILETRANSFER 
SC_EXECUTE 
SC_DOWNLOAD 
SC_URL 
SC_CONNECTBACK_LINK_FILETRANSFER SC_BIND_LINK_FILETRANSFER
SC_KEY SC_SIZE SC_SIZEINVERT SC_HOST SC_PORT SC_COMMAND
SC_URI


%start body

%%

body
	: /* \epsilon */
	| body shellcode
	;

shellcode
	: identifier SC_LBR statements SC_RBR SC_SEMI
	{
		int i;
		
		printf("shellcode:\n");

		printf("\tname               %s\n", shellcodes->name);
		printf("\tnamespace          %s (%d) \n", get_namespace_by_numeric(shellcodes->sc_namespace), shellcodes->sc_namespace);
//		printf("\tpattern            %s\n", shellcodes->pattern);
		printf("\tmap-size           %d\n", shellcodes->map_items);
		printf("\tmap                ");

		for( i = 0; i < shellcodes->map_items; i++ )
		{
			printf("%s (%d) ", get_mapping_by_numeric(shellcodes->map[i]),shellcodes->map[i]);
		}

		printf("\n\n");

		/* prepare for the next one */
		init_shellcode();
	}
	;

identifier
	: namespace SC_COLON SC_COLON SC_ID
	{
		shellcodes->name = strndup(string_get_buffer(), string_get_len());
		string_reset();
	}
	;

namespace
	: SC_XOR
	{
		shellcodes->sc_namespace = sc_xor;
	}
	|
	SC_LINKXOR
	{
		shellcodes->sc_namespace = sc_linkxor;
	}
	|
	SC_KONSTANZXOR
	{
		shellcodes->sc_namespace = sc_konstanzxor;
	}
	|
	SC_LEIMBACHXOR
	{
		shellcodes->sc_namespace = sc_leimbachxor;
	}
	|
	SC_BIND_SHELL
	{
		shellcodes->sc_namespace = sc_bindshell;
	}
	|
	SC_CONNECTBACK_SHELL
	{
		shellcodes->sc_namespace = sc_connectbackshell;
	}
	|
	SC_CONNECTBACK_FILETRANSFER
	{
		 shellcodes->sc_namespace = sc_connectbackfiletransfer;
	}
	|
	SC_EXECUTE
	{
		shellcodes->sc_namespace = sc_execute;
	}
	|
	SC_DOWNLOAD
	{
		shellcodes->sc_namespace = sc_download;
	}
	|
	SC_URL
	{
		shellcodes->sc_namespace = sc_url;
	}
	|
	SC_CONNECTBACK_LINK_FILETRANSFER
	{
		shellcodes->sc_namespace = sc_link;
	}
	| 
	SC_BIND_LINK_FILETRANSFER
	{
		shellcodes->sc_namespace = sc_blink;
	}
	;

statements
	: /* \epsilon */
	| statement statements
	;

statement
	: inline_statement SC_SEMI
	;

inline_statement
	: pattern
	| flags
	| mapping
	;

flags
	: SC_FLAGS SC_NONE 
	{
		printf("flags none...\n");
	}
	;

mapping
	: SC_MAPPING SC_LPAR map_values SC_RPAR
	;

map_values
	: map_value map_value_comma_list
	;

map_value_comma_list
	: /* \epsilon */
	| SC_COMMA map_value map_value_comma_list
	;

map_value
	: SC_KEY
	{
		shellcodes->map[shellcodes->map_items++] = key;
	}
	| SC_SIZE
	{
		shellcodes->map[shellcodes->map_items++] = size;
	}
	| SC_SIZEINVERT
	{
		shellcodes->map[shellcodes->map_items++] = sizeinvert;
	}
	| SC_PORT
	{	
		 shellcodes->map[shellcodes->map_items++] = port;
	}
	| SC_HOST
	{
		shellcodes->map[shellcodes->map_items++] = host;
	}
	| SC_COMMAND
	{
		shellcodes->map[shellcodes->map_items++] = command;
	}
	| SC_URI
   	{
	shellcodes->map[shellcodes->map_items++] = uri;
	}
	;

pattern
	: SC_PATTERN SC_STRING strings
	{
		shellcodes->pattern = strndup(string_get_buffer(), string_get_len());
		shellcodes->pattern_size = string_get_len();
		string_reset();
	}
	;

strings
	:
	| SC_STRING strings
	;

%%

	struct shellcode *init_shellcode()
	{
		struct shellcode *s = (struct shellcode *)malloc(sizeof(struct shellcode));

		memset(s, 0, sizeof(struct shellcode));

		s->next = shellcodes;
		shellcodes = s;
		
		return s;
	}


	char *get_namespace_by_numeric(int num)
	{
	
		static char *namespacemapping[]=
		{
                	"xor",
	                "linkxor",
        	        "konstanzxor",
                	"leimbachxor",
	                "connectbackshell",
        	        "connectbackfiletransfer",
                	"bindshell",
	                "execute",
        	        "download",
	               	"url",
        	        "link",
	                "blink"
		};
		
		if ( num > sizeof(namespacemapping)/sizeof(char *) )
			return "unmapped";
		else
			return namespacemapping[num];
	}

	char *get_mapping_by_numeric(int num)
	{
		static char *mapmapping[]=
		{
	                "key",
        	        "size",
                	"sizeinvert",
	                "port",
        	        "host",
                	"command",
	                "uri"
		};
                if ( num > sizeof(mapmapping)/sizeof(char *) )
                        return "unmapped";
                else
                        return mapmapping[num];
	}
		
		

      int yyerror(char* s) {
	    printf(" %s at '%s' on line %d\n", s, yytext, line_number );return 0;
      }


      int yywrap(){ return 1; }
/*
      int main(int argc, char** argv){
		  init_shellcode();
	    yyparse();
      }

*/
