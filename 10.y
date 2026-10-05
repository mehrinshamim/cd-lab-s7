%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

%token FOR ID NUM LE GE EQ NE INC DEC

%%

program:
      for_stmt
      {
          printf("\nResult: Valid FOR statement syntax\n");
      }
      ;

for_stmt:
      FOR '(' initialization ';' condition ';' increment ')' body
      ;

initialization:
      /* empty */
    | ID '=' expr
    ;

condition:
      /* empty */
    | expr relop expr
    | expr
    ;

increment:
      /* empty */
    | ID '=' expr
    | ID INC
    | ID DEC
    | INC ID
    | DEC ID
    ;

relop:
      '<'
    | '>'
    | LE
    | GE
    | EQ
    | NE
    ;

expr:
      ID
    | NUM
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    ;

body:
      ';'
    | '{' statement_list '}'
    | statement
    ;

statement_list:
      /* empty */
    | statement_list statement
    ;

statement:
      ID '=' expr ';'
    | for_stmt
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr,
            "\nResult: Invalid FOR statement syntax (Error: %s)\n",
            s);
}

int main()
{
    printf("Enter a C 'for' statement to check syntax:\n");
    yyparse();
    return 0;
}