%union 
{
        unsigned int i;
        char* s;
}

%token <s> ALIAS;
%token <s> STRING;
%token <s> EVENT;
%token <s> EQUREL;
%token <i> OPACITY;

%token NEWLINE ON SET SELF TO OBRACE CBRACE OCURLY CCURLY


%{
#include <stdio.h>
#include <string.h>

#include "configfile.h"

extern void cfg_add_rule ( cfg_rule* );
extern void cfg_add_alias ( char*, char* );
extern unsigned int yylineno;

cfg_rule* rule;

void yyerror ( const char *str )
{
	fprintf ( stderr,"Error parsing config file in line %d\n", yylineno );
}
 
int yywrap()
{
        return 1;
} 
  
%}

%%

entries:
       | entries entry ;

entry:
	alias | rule ;

alias:
	ALIAS STRING STRING
	{
		cfg_add_alias ( $2, $3 );
	}
	;

rule: ON condition OCURLY action CCURLY
	{
		cfg_add_rule ( rule );
	}
    | ON condition simpleaction
	{
		cfg_add_rule ( rule );
	}
	;

condition:
	EVENT OBRACE STRING EQUREL STRING CBRACE
	{
		rule = (cfg_rule*) malloc ( sizeof(cfg_rule) );
	
		rule->cond.event = $1;
		rule->cond.property = $3;
		sprintf ( rule->cond.comparison, "%2s", $4 );
		rule->cond.value = $5;
	}
	;

action:
	SET OBRACE STRING EQUREL STRING CBRACE TO OPACITY
	{
		rule->action.property = $3;
		sprintf ( rule->action.comparison, "%2s", $4 );
		rule->action.value = $5;
		rule->action.opacity = $8;
	}
	;
	
	
simpleaction: SET TO OPACITY
		{
			rule->action.property = rule->cond.property;
			sprintf ( rule->action.comparison, "%2s", rule->cond.comparison );
			rule->action.value = rule->cond.value;
			rule->action.opacity = $3;
		}
	    | SET SELF TO OPACITY
		{
			rule->action.property = "__TRANSD_SELF";
			rule->action.opacity = $4;		
		}
	;
	
%%
