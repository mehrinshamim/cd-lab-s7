%{
// Q6: reject strings containing the first four letters of the name ("mehr", any case).
// Build: lex 6.l && gcc lex.yy.c -o 6
#include <stdio.h>
%}
%option noyywrap
%%
.*[mM][eE][hH][rR].*   puts("Rejected");     /* tie in length with next rule -> first rule wins */
.*                     puts("Accepted");
\n                     ;
%%
int main() { puts("Enter String:"); yylex(); }
