
%{
#include "y.tab.h"
%}

%%

[a-zA-Z][a-zA-Z0-9]*    { return VARIABLE; }
[ \t]                   ;        /* Ignore spaces and tabs */
\n                      { return '\n'; }
.                       { return yytext[0]; }

%%

int yywrap()
{
    return 1;
}