%{
// Q10 (lex part).  Build: lex 10.l && yacc -d 10.y && gcc lex.yy.c y.tab.c -o 10
#include "y.tab.h"
%}
%option noyywrap
%%
"for"                      return FOR;
[a-zA-Z_][a-zA-Z0-9_]*     return ID;
[0-9]+                     return NUM;
"<="|">="|"=="|"!="|"<"|">"  return RELOP;
"++"                       return INC;
"--"                       return DEC;
[ \t\n]                    ;
.                          return yytext[0];
