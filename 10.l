%{
#include "y.tab.h"
#include <string.h>

void yyerror(const char *s);
%}

%%

"for"                      { return FOR; }
[a-zA-Z_][a-zA-Z0-9_]*     { return ID; }
[0-9]+                     { return NUM; }

"<="                       { return LE; }
">="                       { return GE; }
"=="                       { return EQ; }
"!="                       { return NE; }

"++"                       { return INC; }
"--"                       { return DEC; }

[ \t\n]                    ;      /* Ignore whitespace */

.                           { return yytext[0]; }

%%

int yywrap()
{
    return 1;
}