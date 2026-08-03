%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

%token NUMBER

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

calculation:
      /* empty */
    | calculation line
    ;

line:
      '\n'
    | expr '\n'          { printf("Result: %d\n\n", $1); }
    ;

expr:
      NUMBER                 { $$ = $1; }
    | expr '+' expr          { $$ = $1 + $3; }
    | expr '-' expr          { $$ = $1 - $3; }
    | expr '*' expr          { $$ = $1 * $3; }
    | expr '/' expr
      {
          if ($3 == 0)
          {
              yyerror("Division by zero");
              $$ = 0;
          }
          else
          {
              $$ = $1 / $3;
          }
      }
    | '(' expr ')'           { $$ = $2; }
    | '-' expr %prec UMINUS  { $$ = -$2; }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}

int main()
{
    printf("Enter expressions to calculate:\n");
    yyparse();
    return 0;
}
