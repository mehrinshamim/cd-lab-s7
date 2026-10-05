%{
#include <stdio.h>

void yyerror(const char *s);
int yylex();
%}

%token VARIABLE

%%

input:
      /* empty */
    | input line
    ;

line:
      '\n'
    | VARIABLE '\n'      { printf("Result: Valid Variable\n"); }
    ;

%%

void yyerror(const char *s)
{
    printf("Result: Invalid Variable\n");
}

int main()
{
    printf("Enter a variable name: ");
    yyparse();
    return 0;
}