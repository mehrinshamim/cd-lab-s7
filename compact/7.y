%{
// Q7 (yacc part): valid variable = letter followed by letters/digits.
#include <stdio.h>
int yylex(); void yyerror(const char *s) { puts("Result: Invalid Variable"); }
%}
%token VARIABLE
%%
input: | input VARIABLE '\n'  { puts("Result: Valid Variable"); }
       | input '\n' ;
%%
int main() { printf("Enter a variable name: "); yyparse(); }
