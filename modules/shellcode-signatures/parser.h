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

enum mapping 
{ 
	key, 
	size, 
	sizeinvert, 
	port, 
	host,
	command,
	uri
};

#define MAP_MAX 8
struct shellcode
{
	char *name;
	char *author;
	char *reference;
	char *pattern;
	int pattern_size;
	enum sc_namespace nspace;
	int map_items;
	enum mapping map[MAP_MAX];
	int flags;

	struct shellcode *next;
};

extern struct shellcode *sc_parse_file(const char *);
extern char *sc_get_error();

#endif
