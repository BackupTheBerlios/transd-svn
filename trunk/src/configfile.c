#include "configfile.h"

const char* CFG_FILE = "transd.conf";

extern FILE* yyin;
extern void yyparse();

static slist* cfg_aliases = NULL;
static slist* cfg_rules = NULL;


int cfg_parse_config_file ( char* path )
{
	char* filename = (char*) malloc ( strlen(CFG_FILE)
		+ strlen(path) + 1 );
	
	sprintf ( filename, "%s/%s", path, CFG_FILE );
	
	//if ( debug-flag )
	//{
		printf ( "Opening configuration file '%s'...\n", filename );
	//}
	
	yyin = fopen ( filename, "r" );
	
	yyparse();

	//if ( debug-flag)
	//{
		printf ( "Aliases list loaded:\n" );
		cfg_print_aliases();

		printf ( "Rules list loaded:\n" );
		cfg_print_rules();
	//}
	
	return(0);
}


void cfg_add_rule ( char* property, char* value,
	char* event, unsigned int opacity )
{
	cfg_rule* rule = malloc(sizeof(cfg_rule));
	rule->property = property;
	rule->value = value;
	rule->event = event;
	rule->opacity = opacity;
	
	slist_add ( &cfg_rules, rule ); 
	
	return;
}


void cfg_print_rule ( void* data )
{
	cfg_rule* rule = (cfg_rule*) data;
	
	printf ( "\t%s %s %s %x\n", rule->property, rule->value,
		rule->event, rule->opacity );
	
	return;
}


void cfg_print_rules()
{
	slist_print ( cfg_rules, cfg_print_rule );
	
	return;
}


void cfg_add_alias ( char* property, char* alias )
{
	cfg_alias* _alias = malloc(sizeof(cfg_alias));
		
	_alias->alias = alias;
	_alias->property = property;
	
	slist_add ( &cfg_aliases, _alias ); 
	
	return;
}


void cfg_print_aliases()
{
	slist_print ( cfg_aliases, cfg_print_alias );
	
	return;
}


void cfg_print_alias ( void* data )
{
	cfg_alias* alias = (cfg_alias*) data;
	
	printf ( "\t%s -> %s\n", alias->alias, alias->property );
	
	return;
}
