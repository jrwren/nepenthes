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
	sc_bindfiletransfer,
	sc_base64

};

enum sc_mapping 
{ 
	sc_key,
	sc_subkey,
	sc_size, 
	sc_sizeinvert, 
	sc_port, 
	sc_host,
	sc_command,
	sc_uri,
	sc_decoder,
	sc_pre,
	sc_post,
	sc_none,
	sc_hostkey,
	sc_portkey

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
extern int sc_free_shellcodes(struct sc_shellcode *s);
extern char *sc_get_error();

extern char *sc_get_namespace_by_numeric(int num);
extern char *sc_get_mapping_by_numeric(int num);


#endif
