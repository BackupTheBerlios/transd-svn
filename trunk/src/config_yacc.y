%union 
{
        unsigned int i;
        char* s;
}

%token <s> ALIAS;
%token <s> STRING;
%token <s> EVENT;
%token <i> OPACITY;
%token NEWLINE


%{
#include <stdio.h>
#include <string.h>

extern void cfg_add_rule ( char*, char*, char*, unsigned int );
extern void cfg_add_alias ( char*, char* );
extern unsigned int yylineno;

void yyerror ( const char *str )
{
	fprintf ( stderr,"Error parsing config file in line %d\n", yylineno - 2 );
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
	alias | simplerule

alias:
	ALIAS STRING STRING
	{
		cfg_add_alias ( $2, $3 )
	}
	;

simplerule:
	STRING STRING EVENT OPACITY
	{
		cfg_add_rule ( $1, $2, $3, $4 );
	}
	;

complexrule:
	

%%
