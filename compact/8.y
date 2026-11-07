%{
// Q8 (yacc part): calculator.  + - * / ( ) and unary minus.
#include <stdio.h>
int yylex(); void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
%}
%token NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS
%%
calc: | calc expr '\n'  { printf("Result: %d\n\n", $2); }
      | calc '\n' ;
expr: NUMBER
    | expr '+' expr  { $$ = $1 + $3; }
    | expr '-' expr  { $$ = $1 - $3; }
    | expr '*' expr  { $$ = $1 * $3; }
    | expr '/' expr  { if ($3) $$ = $1 / $3; else { yyerror("Division by zero"); $$ = 0; } }
    | '(' expr ')'   { $$ = $2; }
    | '-' expr %prec UMINUS  { $$ = -$2; } ;
%%
int main() { puts("Enter expressions to calculate:"); yyparse(); }
