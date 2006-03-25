#include <string.h>
#include <memory.h>

#define MAP_MAX 8




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

struct shellcode
{
	char *name;
	char *author;
	char *reference;
	char *pattern;
	int pattern_size;
	enum sc_namespace sc_namespace;
	int map_items;
	enum mapping map[MAP_MAX];
	int flags;

	struct shellcode *next;
};

extern struct shellcode *shellcodes;


void string_reset();
char *string_get_buffer();
int string_get_len();
struct shellcode *init_shellcode();
char *get_namespace_by_numeric(int num);
char *get_mapping_by_numeric(int num);

