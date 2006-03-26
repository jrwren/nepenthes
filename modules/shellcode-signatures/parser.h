/* $Id$ */
#ifndef PARSER_H 
#define PARSER_H

enum sc_namespace
{ 
	sc_xor,
	sc_linkxor,
	sc_konstanzxor,
	sc_leimbachxor,
	sc_connectbackshell,
	sc_connectbackfiletransfer,
	sc_bindshell,
	sc_execute,
	sc_download,
	sc_url,
	sc_link,
	sc_blink
};

enum sc_mapping 
{ 
	sc_key, 
	sc_size, 
	sc_sizeinvert, 
	sc_port, 
	sc_host,
	sc_command,
	sc_uri,
	sc_pcre,
	sc_pre,
	sc_post,
	sc_none
};

#define MAP_MAX 8
struct sc_shellcode
{
	char *name;
	char *author;
	char *reference;
	char *pattern;
	int pattern_size;
	enum sc_namespace nspace;
	int map_items;
	enum sc_mapping map[MAP_MAX];
	int flags;

	struct sc_shellcode *next;
};

extern struct sc_shellcode *sc_parse_file(const char *);
extern char *sc_get_error();

extern char *sc_get_namespace_by_numeric(int num);
extern char *sc_get_mapping_by_numeric(int num);


#endif
