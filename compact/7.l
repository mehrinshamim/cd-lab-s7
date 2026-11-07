%{
// Q7 (lex part).  Build: lex 7.l && yacc -d 7.y && gcc lex.yy.c y.tab.c -o 7
#include "y.tab.h"
%}
%option noyywrap
%%
[a-zA-Z][a-zA-Z0-9]*   return VARIABLE;
[ \t]                  ;
\n|.                   return yytext[0];
