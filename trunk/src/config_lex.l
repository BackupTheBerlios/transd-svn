%{
#include "y.tab.h"

extern unsigned int OPAQUE;

%}


%%


^[ \t]*\n		/* empty line */++yylineno;
^#.*\n			/* comment */ ++yylineno;
Alias			return ALIAS;
Enter|Leave		yylval.s = strdup(yytext); return EVENT;
[a-zA-Z_]+[a-zA-Z_0-9]	yylval.s = strdup(yytext); return STRING;

(0*(\.[0-9]+)*)|1|1.0	yylval.i = ( atof(yytext) * OPAQUE); return OPACITY;
([0-9]{0,1}[0-9]+|100)%	yylval.i = ( ( atof(yytext) / 100 ) * OPAQUE); return OPACITY;
0x([0-9a-fA-F]){8}	yylval.i = ( atof(yytext) ); return OPACITY;

[ \t]+			;
\n			++yylineno;


%%
