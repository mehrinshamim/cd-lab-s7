%{
// Q9 (lex part).  Build: lex 9.l && yacc -d 9.y && gcc lex.yy.c y.tab.c -o 9
#include <string.h>
#include "y.tab.h"
%}
%option noyywrap
%%
[a-zA-Z][a-zA-Z0-9]*   { yylval.s = strdup(yytext); return ID; }
[0-9]+                 { yylval.s = strdup(yytext); return NUM; }
[ \t]                  ;
\n                     return 0;          /* end of input */
.                      return yytext[0];
