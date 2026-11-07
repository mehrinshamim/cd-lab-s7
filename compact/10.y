%{
// Q10 (yacc part): syntax check of a C for-statement.
#include <stdio.h>
int yylex(); void yyerror(const char *s) { printf("\nResult: Invalid FOR statement syntax (Error: %s)\n", s); }
%}
%token FOR ID NUM RELOP INC DEC
%left '+' '-'
%left '*' '/'
%%
prog: stmt  { puts("\nResult: Valid FOR statement syntax"); } ;
stmt: FOR '(' init ';' cond ';' inc ')' body ;
init: | ID '=' expr ;
cond: | expr RELOP expr | expr ;
inc:  | ID '=' expr | ID INC | ID DEC | INC ID | DEC ID ;
expr: ID | NUM | expr '+' expr | expr '-' expr | expr '*' expr | expr '/' expr | '(' expr ')' ;
body: ';' | '{' list '}' | one ;
list: | list one ;
one:  ID '=' expr ';' | stmt ;
%%
int main() { puts("Enter a C 'for' statement to check syntax:"); yyparse(); }
