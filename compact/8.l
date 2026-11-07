%{
// Q8 (lex part).  Build: lex 8.l && yacc -d 8.y && gcc lex.yy.c y.tab.c -o 8
#include <stdlib.h>
#include "y.tab.h"
%}
%option noyywrap
%%
[0-9]+     { yylval = atoi(yytext); return NUMBER; }
[ \t]      ;
\n|.       return yytext[0];
