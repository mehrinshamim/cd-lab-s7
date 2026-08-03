%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define AST Node Structure */
typedef struct node
{
    char *token;
    struct node *left;
    struct node *right;
} node;

node* mknode(char *token, node *left, node *right);
void print_postorder(node *tree);

int yylex();
void yyerror(const char *s);
%}

%union
{
    char *sval;
    struct node *nval;
}

%token <sval> ID NUM
%type <nval> S E T F

%left '+' '-'
%left '*' '/'

%%

S:
      ID '=' E
      {
          node *id_node = mknode($1, NULL, NULL);
          node *root = mknode("=", id_node, $3);

          printf("\n--- Abstract Syntax Tree Generated Successfully ---\n");
          printf("Post-order Traversal of AST: ");
          print_postorder(root);
          printf("\n");
      }
      ;

E:
      E '+' T      { $$ = mknode("+", $1, $3); }
    | E '-' T      { $$ = mknode("-", $1, $3); }
    | T            { $$ = $1; }
    ;

T:
      T '*' F      { $$ = mknode("*", $1, $3); }
    | T '/' F      { $$ = mknode("/", $1, $3); }
    | F            { $$ = $1; }
    ;

F:
      '(' E ')'    { $$ = $2; }
    | ID           { $$ = mknode($1, NULL, NULL); }
    | NUM          { $$ = mknode($1, NULL, NULL); }
    ;

%%

node* mknode(char *token, node *left, node *right)
{
    node *newnode = (node*)malloc(sizeof(node));

    newnode->token = strdup(token);
    newnode->left = left;
    newnode->right = right;

    return newnode;
}

void print_postorder(node *tree)
{
    if (tree->left)
        print_postorder(tree->left);

    if (tree->right)
        print_postorder(tree->right);

    printf("%s ", tree->token);
}

void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}

int main()
{
    printf("Enter assignment expression (e.g., x = a + b * c):\n");
    yyparse();
    return 0;
}