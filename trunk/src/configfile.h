#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "slist.h"

/*
 * rec. paths: /etc/transd ~/.transd
 */

struct _cfg_rule_entry
{
	char* property;
	char* value;
	char* event;
	unsigned int opacity;
};
typedef struct _cfg_rule_entry cfg_rule;

struct _cfg_alias_entry
{
	char* alias;
	char* property;
};
typedef struct _cfg_alias_entry cfg_alias;


void cfg_alias_add_entry ( cfg_alias* alias );


/* reads the entries from config file */
int cfg_parse_config_file ( char* path );

/* these two are called by yacc */
void cfg_add_rule ( char* property, char* value, char* event, unsigned int opacity );
void cfg_add_alias ( char* property, char* alias );

/* debugging functions */
void cfg_print_rules();
void cfg_print_aliases();

void cfg_print_rule ( void* data );
void cfg_print_alias ( void* data );

/* FIXME: cfg_get_rules */

#endif
