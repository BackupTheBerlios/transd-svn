#include "configfile.h"

static slist* cfg_aliases = NULL;
static slist* cfg_rules = NULL;

extern struct transd_options options;

int cfg_parse_config_file ( char* path )
{
	extern FILE* yyin;
	extern void yyparse();

	const char* CFG_FILE = "transd.conf";
	
	char* filename = (char*) malloc ( strlen(CFG_FILE)
		+ strlen(path) + 1 );
	
	sprintf ( filename, "%s/%s", path, CFG_FILE );
	
	if ( options.debug )
	{
		printf ( "Opening configuration file '%s'...\n", filename );
	}
	
	yyin = fopen ( filename, "r" );
	
	if ( yyin == NULL )
		return(-1);
	
	yyparse();

	if ( options.debug )
	{
		printf ( "Aliases list loaded:\n" );
		cfg_print_aliases();

		printf ( "Rules list loaded:\n" );
		cfg_print_rules();
	}
	
	return(0);
}


void cfg_add_rule ( cfg_rule* rule )
{
	/* TODO: add aliases */
	printf ( "Adding rule: ON %s ( %s %s %s ) SET ( %s %s %s ) TO %x\n",
		rule->cond.event, rule->cond.property, rule->cond.comparison,
		rule->cond.value, rule->action.property, rule->action.comparison,
		rule->action.value, rule->action.opacity );	
	
	
	slist_add ( &cfg_rules, rule );
}


void cfg_print_rule ( void* data )
{
	cfg_rule* rule = (cfg_rule*) data;
	
	printf ( "\tON %s ( %s %s %s ) SET ( %s %s %s ) TO %x\n",
		rule->cond.event, rule->cond.property, rule->cond.comparison,
		rule->cond.value, rule->action.property, rule->action.comparison,
		rule->action.value, rule->action.opacity );
	
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


int cfg_check_property ( Display* d, Window w, char* property, char* value )
{
	Atom a;
	int count;
	char** strlist;
	XTextProperty tp;
	int i;
	
	char* name; XFetchName(d, w, &name);
	
	a = XInternAtom ( d, property, False );
	
	
	if ( a == None )
		return(1);
	
	XGetTextProperty ( d, w, &tp, a );
	if ( tp.value == NULL )
		return(1);

	XTextPropertyToStringList ( &tp, &strlist, &count );

	for ( i = count - 1; i ; --i )
		if ( !strcmp ( strlist[i], value ) )
			return(0);
		
	XFree ( strlist );

	return(1);
}


cfg_rule* cfg_get_rule ( Display* d, Window w, char* event )
{
	slist* p = cfg_rules;
	
	if ( p == NULL )
	{
		return(NULL);
	}
	
	while ( p != NULL )
	{
		assert(p->data != NULL);
	
		/* does the event condition match? */
		if ( !strcmp ( ((cfg_rule*)p->data)->cond.event, event ) )
		{
			/* does the value condition match? */
			if ( !cfg_check_property ( d, w, ((cfg_rule*)p->data)->cond.property,
				((cfg_rule*)p->data)->cond.value ) )
				return ( (cfg_rule*)p->data );

		}
		
		next: p = p->next;
	}
	
	return(NULL);
}
