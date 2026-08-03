%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>

void yyerror(const char *s);
%}

%%

[a-zA-Z][a-zA-Z0-9]* {
    yylval.sval = strdup(yytext);
    return ID;
}

[0-9]+ {
    yylval.sval = strdup(yytext);
    return NUM;
}

[ \t]               ;      /* Ignore spaces and tabs */

\n                   { return 0; }

.                    { return yytext[0]; }

%%

int yywrap()
{
    return 1;
}