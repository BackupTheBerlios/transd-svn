#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "slist.h"


#define DEBUG(level, ...) if (options.debug >= level) printf(__VA_ARGS__);

struct transd_options
{
	unsigned int debug;
	char* config_file;
};

struct _cfg_base_rule
{
	char* property;
	char comparison[3]; /* eq / ne */
	char* value;	
	char* event;
	unsigned int opacity;
};
typedef struct _cfg_base_rule cfg_base_rule;

struct _cfg_rule
{
	cfg_base_rule cond;
	cfg_base_rule action;
};
typedef struct _cfg_rule cfg_rule;

struct _cfg_alias_entry
{
	char* alias;
	char* property;
};
typedef struct _cfg_alias_entry cfg_alias;


void cfg_alias_add_entry ( cfg_alias* alias );


/* reads the entries from config file */
int cfg_parse_config_file ( char* path );

/* these are called by yacc */
void cfg_add_rule ( cfg_rule* rule );
void cfg_add_alias ( char* property, char* alias );

/* debugging functions */
void cfg_print_rules();
void cfg_print_aliases();

void cfg_print_rule ( void* data );
void cfg_print_alias ( void* data );

cfg_rule* cfg_get_rule ( Display*, Window, char* Event );
int cfg_check_property ( Display*, Window, char* propery, char* value );

#endif
