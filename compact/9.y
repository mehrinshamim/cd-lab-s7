%{
// Q9 (yacc part): BNF -> YACC, builds an AST and prints it in post-order.
//   S -> id = E     E -> E+T | E-T | T     T -> T*F | T/F | F     F -> (E) | id | num
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct node { char *t; struct node *l, *r; } node;
node *mk(char *t, node *l, node *r) { node *n = malloc(sizeof *n); n->t = strdup(t); n->l = l; n->r = r; return n; }
void post(node *n) { if (!n) return; post(n->l); post(n->r); printf("%s ", n->t); }   // left, right, root
int yylex(); void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
%}
%union { char *s; struct node *n; }
%token <s> ID NUM
%type <n> E T F
%%
S: ID '=' E  { printf("Post-order Traversal of AST: "); post(mk("=", mk($1, 0, 0), $3)); puts(""); } ;
E: E '+' T   { $$ = mk("+", $1, $3); }
 | E '-' T   { $$ = mk("-", $1, $3); }
 | T ;
T: T '*' F   { $$ = mk("*", $1, $3); }
 | T '/' F   { $$ = mk("/", $1, $3); }
 | F ;
F: '(' E ')' { $$ = $2; }
 | ID        { $$ = mk($1, 0, 0); }
 | NUM       { $$ = mk($1, 0, 0); } ;
%%
int main() { puts("Enter assignment expression (e.g., x = a + b * c):"); yyparse(); }
