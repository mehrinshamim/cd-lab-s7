%{
#include "y.tab.h"
extern int yylval;
extern void yyerror(const char *s);
%}

%%

[0-9]+              { yylval = atoi(yytext); return NUMBER; }
[ \t]               ;      /* Ignore spaces and tabs */
\n                  { return '\n'; }
[-+*/()]            { return yytext[0]; }
.                   { yyerror("Unknown character"); }

%%

int yywrap()
{
    return 1;
}