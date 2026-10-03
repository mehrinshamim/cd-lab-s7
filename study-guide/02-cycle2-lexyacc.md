# Cycle II — Lex & Yacc Programs (5–10)

Read Part B of `00-foundations.md` first. Each section below says exactly
which subsection it's applying.

## Table of Contents

- [Program 5 — hand-written lexical analyzer](#program-5)
- [Program 6 — reject strings containing a substring](#program-6)
- [Program 7 — validate a variable name (Yacc)](#program-7)
- [Program 8 — calculator (Lex + Yacc)](#program-8)
- [Program 9 — BNF → Yacc, build an AST](#program-9)
- [Program 10 — check C `for` statement syntax](#program-10)

---

<a id="program-5"></a>
## Program 5 — `5-lex.c`: hand-written lexical analyzer

**Problem statement:** *Design and implement a lexical analyzer for a given
language using C; it should ignore redundant spaces, tabs and newlines.*

**Concept used:** §B.1–B.2, but written **by hand** (no Lex tool) so you can
see what a generated lexer is doing under the hood.

### Algorithm
Read the file one character at a time (`fgetc`). For the character just
read, decide its category by what kind of character it is, then **keep
consuming** more characters of the *same* kind until a different kind of
character appears (longest match, done manually):
- letter/`_` first → keep consuming letters/digits/`_` → identifier or
  keyword (check against a fixed keyword list).
- digit first → keep consuming digits → number.
- one of `+-*/=<>%` → single-character operator token.
- one of `;,(){}[]` → single-character special-symbol token.
- space/tab/newline → skip, don't emit any token.

### Annotated code (key part)
```c
while ((ch = fgetc(fp)) != EOF) {
    if (ch == ' ' || ch == '\t' || ch == '\n') continue;   // ignore whitespace

    if (isalpha(ch) || ch == '_') {                          // identifier/keyword
        i = 0; token[i++] = ch;
        while ((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '_'))
            token[i++] = ch;                                  // keep extending — "longest match"
        token[i] = '\0';
        if (ch != EOF) fseek(fp, -1, SEEK_CUR);                // put back the non-matching char!
        printf(isKeyword(token) ? "%s : Keyword\n" : "%s : Identifier\n", token);
    }
    else if (isdigit(ch)) { /* same pattern for numbers */ }
    else if (/* operator chars */)      printf("%c : Operator\n", ch);
    else if (/* bracket/punct chars */) printf("%c : Special Symbol\n", ch);
    else                                 printf("%c : Unknown\n", ch);
}
```
**Callouts:**
- `fseek(fp, -1, SEEK_CUR)` after the inner `while` is the critical,
  easy-to-forget line: the inner loop over-reads *one* character (the
  delimiter that ended the token) to know *when to stop* — that character
  must be pushed back onto the stream, or it gets silently swallowed and the
  next token starts one character short. (This is exactly the problem a
  generated Lex scanner solves for you automatically — Lex buffers input and
  only "commits" past a position once a rule has fully matched.)
- `isKeyword` is a linear search through a fixed C array of keyword strings
  — fine for a handful of keywords; a real compiler would use a hash table.

### Dry run
Input file `5-input.txt`:
```
int a=25;
float num=100;
if(a>10)
{
    a=a+1;
}
```
**Output:**
```
int : Keyword
a : Identifier
= : Operator
25 : Number
; : Special Symbol
float : Keyword
num : Identifier
= : Operator
100 : Number
; : Special Symbol
if : Keyword
( : Special Symbol
a : Identifier
> : Operator
10 : Number
) : Special Symbol
{ : Special Symbol
a : Identifier
= : Operator
a : Identifier
+ : Operator
1 : Number
; : Special Symbol
} : Special Symbol
```

---

<a id="program-6"></a>
## Program 6 — `6-substr.l`: reject strings containing a substring

**Problem statement:** *Write a lex program to recognize all strings which
do not contain the first four characters of your name as a substring.*
(Name here: "Mehrin" → substring "Mehr", checked case-insensitively.)

**Concept used:** §B.2–B.3, plus the **tie-break rule** (earlier rule wins
when match lengths are equal) — this program only works *because* of that
rule.

### Annotated code
```lex
%{
#include <stdio.h>
%}
%%
.*[mM][eE][hH][rR].*      { printf("Rejected\n"); }
.*                        { printf("Accepted\n"); }
\n                        ;
%%
int yywrap() { return 1; }
int main() { printf("Enter String:\n"); yylex(); return 0; }
```
**Callouts:**
- Both rules can match the **entire line** when "Mehr" (any case) is
  present anywhere in it — `.*[mM][eE][hH][rR].*` and the plain `.*` both
  stretch to match the whole input in that case. Since lengths tie, **Lex
  picks whichever rule is written first** — this is exactly §B.2's
  "earlier rule wins on a tie," and it's *why* the reject rule must be
  listed before the plain accept rule, not after.
- `\n ;` (empty action) consumes the trailing newline without printing
  anything for it, so you don't get a spurious extra match per line.

### Dry run
| Input | Does it contain "mehr" (any case)? | Output |
|---|---|---|
| `I love compiler design` | no | `Accepted` |
| `Mehrin wrote this lab` | yes (`Mehr`) | `Rejected` |
| `comeHeRe now` | yes — scattered case-insensitive `[mM][eE][hH][rR]`... wait, check carefully: needs the *exact sequence* m-e-h-r (any case), contiguous | depends — see note below |

**Note on the table's last row:** the pattern requires the four letters
**consecutively** (`m`,`e`,`h`,`r` in that order, each may be any case) —
not scattered. `comeHeRe` does **not** contain the contiguous sequence
m-e-h-r, so it would actually be `Accepted`. Good thing to double check by
hand in a viva: trace the literal character sequence, don't just eyeball it.

---

<a id="program-7"></a>
## Program 7 — `7-variable.l` / `7-variable.y`: validate a variable name

**Problem statement:** *Write a YACC program to recognize a valid variable
which starts with a letter followed by any number of letters or digits.*

**Concept used:** §B.4–B.5 — the simplest possible Lex+Yacc pair, good for
seeing the wiring with nothing else going on.

### Annotated code
```lex
/* 7-variable.l */
%{
#include "y.tab.h"
%}
%%
[a-zA-Z][a-zA-Z0-9]*    { return VARIABLE; }
[ \t]                   ;
\n                      { return '\n'; }
.                       { return yytext[0]; }
%%
int yywrap() { return 1; }
```
```yacc
/* 7-variable.y */
%{
#include <stdio.h>
void yyerror(const char *s);
int yylex();
%}
%token VARIABLE
%%
input: /* empty */ | input line ;
line:  '\n' | VARIABLE '\n' { printf("Result: Valid Variable\n"); } ;
%%
void yyerror(const char *s) { printf("Result: Invalid Variable\n"); }
int main() { printf("Enter a variable name: "); yyparse(); return 0; }
```
**Callouts:**
- The lexer's **last** rule, `. { return yytext[0]; }`, is a catch-all: any
  single character not matched by an earlier rule (e.g. a digit appearing
  *first*, or a symbol like `$`) gets returned as its own raw character
  code. The grammar's `line` rule only accepts `'\n'` or `VARIABLE '\n'` —
  so if that raw character ever shows up where a `line` was expected, it
  can't match any alternative, and Yacc calls `yyerror()`.
- `yyerror` here doesn't abort anything dramatic — it just prints "Invalid
  Variable" and returns; `yyparse()` then usually keeps going or exits
  depending on error recovery (in this grammar, with no `error` recovery
  rule, Yacc will stop parsing that input and `yyparse()` returns non-zero).

### Dry run
| Input | Lexer tokens | Grammar match | Output |
|---|---|---|---|
| `abc123\n` | `VARIABLE` `'\n'` | matches `VARIABLE '\n'` | `Result: Valid Variable` |
| `123abc\n` | `'1'` (raw char, digit doesn't match the letter-first rule) ... | doesn't match `line` | `Result: Invalid Variable` |
| `\n` (blank) | `'\n'` | matches `line: '\n'` | *(nothing printed — matches silently, no action)* |

---

<a id="program-8"></a>
## Program 8 — `8-calc.l` / `8-calc.y`: calculator using LEX + YACC

**Problem statement:** *Implementation of Calculator using LEX and YACC.*

**Concept used:** §B.6 (semantic values via `yylval`/`$$`/`$1`) + §B.7
(precedence/associativity/`%prec`).

### Annotated code
```lex
/* 8-calc.l */
%{
#include "y.tab.h"
extern int yylval;
extern void yyerror(const char *s);
%}
%%
[0-9]+     { yylval = atoi(yytext); return NUMBER; }   /* carries a VALUE */
[ \t]      ;
\n         { return '\n'; }
[-+*/()]   { return yytext[0]; }
.          { yyerror("Unknown character"); }
%%
int yywrap() { return 1; }
```
```yacc
/* 8-calc.y */
%token NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS
%%
calculation: /* empty */ | calculation line ;
line: '\n' | expr '\n' { printf("Result: %d\n\n", $1); } ;
expr:
      NUMBER                 { $$ = $1; }
    | expr '+' expr          { $$ = $1 + $3; }
    | expr '-' expr          { $$ = $1 - $3; }
    | expr '*' expr          { $$ = $1 * $3; }
    | expr '/' expr          { if ($3==0) { yyerror("Division by zero"); $$=0; }
                                 else $$ = $1 / $3; }
    | '(' expr ')'           { $$ = $2; }
    | '-' expr %prec UMINUS  { $$ = -$2; }
    ;
```
**Callouts:**
- `yylval` here is a plain global `int` (no `%union` needed, since every
  token/nonterminal carries the same type: a number) — contrast this with
  Program 9, which *does* need a `%union` because it carries mixed types.
  `[0-9]+ { yylval = atoi(yytext); return NUMBER; }` is the lexer handing a
  *value*, not just a token type, over to the parser.
- `'-' expr %prec UMINUS` — without the `%prec UMINUS` override, this rule
  would take the precedence of its *last terminal*, which is `'-'` (low
  precedence) — making `-2*3` potentially parse wrong. `%prec UMINUS`
  forces this specific rule to use the very high precedence declared for
  the fake `UMINUS` token instead.
- Division by zero is handled as a **semantic** check (`if ($3==0)`) inside
  the action, not a **syntax** rule — division by zero is syntactically a
  perfectly valid `expr '/' expr`, so it has to be caught at evaluation
  time, not parse time.

### Dry run
| Input | Parse / evaluation | Output |
|---|---|---|
| `3 + 4 * 2` | `*` binds tighter than `+` → `3 + (4*2)` = `3+8` | `Result: 11` |
| `-3*4` | `%prec UMINUS` makes unary `-` bind first → `(-3)*4` | `Result: -12` |
| `5/0` | `$3==0` triggers `yyerror("Division by zero")`, `$$=0` | prints error to stderr, then `Result: 0` |

---

<a id="program-9"></a>
## Program 9 — `9-ast.l` / `9-ast.y`: BNF → YACC, build an AST

**Problem statement:** *Convert the BNF rules into YACC form and write code
to generate an abstract syntax tree.*

**Concept used:** §B.6's `%union`/`%type` in full — this is the one program
in the set where tokens and nonterminals carry *structured* values (tree
node pointers), not plain numbers.

### Annotated code
```yacc
/* 9-ast.y — key parts */
typedef struct node {
    char *token;
    struct node *left, *right;
} node;

%union {
    char *sval;         /* for lexer tokens: identifier/number text    */
    struct node *nval;  /* for grammar nonterminals: a built tree node */
}
%token <sval> ID NUM
%type  <nval> S E T F
%left '+' '-'
%left '*' '/'
%%
S: ID '=' E {
      node *id_node = mknode($1, NULL, NULL);
      node *root = mknode("=", id_node, $3);
      print_postorder(root);
   } ;
E: E '+' T { $$ = mknode("+", $1, $3); }
 | E '-' T { $$ = mknode("-", $1, $3); }
 | T       { $$ = $1; } ;
T: T '*' F { $$ = mknode("*", $1, $3); }
 | T '/' F { $$ = mknode("/", $1, $3); }
 | F       { $$ = $1; } ;
F: '(' E ')' { $$ = $2; }
 | ID        { $$ = mknode($1, NULL, NULL); }
 | NUM       { $$ = mknode($1, NULL, NULL); } ;
```
**Callouts:**
- `%token <sval> ID NUM` tells Yacc: whenever an `ID` or `NUM` token's value
  is accessed as `$n`, read it out of the `sval` field of the union.
  `%type <nval> S E T F` does the same for these grammar nonterminals, but
  the `nval` field. This is exactly how one `$1`/`$$` mechanism safely
  carries *two different C types* through the same grammar.
- Every grammar rule builds a **new tree node whose children are the
  already-built subtrees** of the pieces it's made of (`mknode("+", $1,
  $3)`) — this is the general pattern for building any AST in Yacc:
  terminals (`ID`, `NUM`) become **leaf** nodes, and every reduction wraps
  its right-hand-side values into a new **internal** node.
- The grammar's shape (`E` for `+`/`-`, `T` for `*`/`/`, `F` for atoms/
  parens) *is* precedence encoded structurally, as an alternative to
  `%left`/`%right` declarations — this is the classic "layered grammar"
  technique: lower-precedence operators get the outermost nonterminal.

### Dry run
Input: `x = a + b * c`

Parse tree built bottom-up:
```
F(a)=leaf"a"   F(b)=leaf"b"   F(c)=leaf"c"
T(b*c) = mknode("*", leaf"b", leaf"c")
E(a + b*c) = mknode("+", leaf"a", T(b*c))
S: root = mknode("=", leaf"x", E(a+b*c))
```
```
            =
          /   \
         x      +
              /   \
             a      *
                   / \
                  b   c
```
`print_postorder` visits left subtree, then right subtree, then prints its
own token — classic **left, right, node** order:

**Output:**
```
--- Abstract Syntax Tree Generated Successfully ---
Post-order Traversal of AST: x a b c * + =
```
(This is exactly the postfix form of `x = a + b*c`: operands `a b c`,
then `*` once both its operands are ready, then `+`, then `=` last since
it's the root.)

---

<a id="program-10"></a>
## Program 10 — `10-for_check.l` / `10-for_check.y`: check C `for` syntax

**Problem statement:** *Write a YACC program to check the syntax of the
FOR statement in C.*

**Concept used:** §B.4 — a grammar used purely to **accept/reject shape**,
with no semantic values at all (contrast with Programs 8 and 9).

### Annotated code
```yacc
/* 10-for_check.y — key parts */
%token FOR ID NUM LE GE EQ NE INC DEC
%%
program: for_stmt { printf("\nResult: Valid FOR statement syntax\n"); } ;

for_stmt: FOR '(' initialization ';' condition ';' increment ')' body ;

initialization: /* empty */ | ID '=' expr ;
condition:      /* empty */ | expr relop expr | expr ;
increment:      /* empty */ | ID '=' expr | ID INC | ID DEC | INC ID | DEC ID ;
relop:          '<' | '>' | LE | GE | EQ | NE ;
expr:           ID | NUM | expr '+' expr | expr '-' expr
              | expr '*' expr | expr '/' expr | '(' expr ')' ;
body:           ';' | '{' statement_list '}' | statement ;
statement_list: /* empty */ | statement_list statement ;
statement:      ID '=' expr ';' | for_stmt ;
%%
void yyerror(const char *s) {
    fprintf(stderr, "\nResult: Invalid FOR statement syntax (Error: %s)\n", s);
}
```
**Callouts:**
- Every production from `initialization`, `condition`, and `increment`
  includes an **empty alternative** — this is deliberately modeling that a
  real C `for` allows any of the three clauses to be left blank
  (`for(;;)` is valid C). Forgetting the empty alternative is the single
  most common mistake when writing this grammar from scratch in an exam.
- `for_stmt` appears inside `statement`, which is reachable from `body` —
  this makes the grammar **recursive**, so nested `for` loops
  (`for(...) { for(...) {...} }`) are accepted without any special-casing.
- This grammar never builds a value or checks types — it's purely
  structural. That's the point of a *syntax* checker versus Program 8/9,
  which also compute/build something.

### Dry run
**Valid input:** `for(i=0;i<10;i++){i=i+1;}`
```
Tokens: FOR ( ID = NUM ; ID < NUM ; ID INC ) { ID = ID + NUM ; }
initialization: ID '=' expr   -> i=0        ✓
condition:      expr relop expr -> i < 10   ✓
increment:      ID INC        -> i++        ✓
body:           '{' statement_list '}'  (statement_list -> statement -> ID '=' expr ';')  ✓
```
**Output:** `Result: Valid FOR statement syntax`

**Invalid input:** `for(i=0;i<10)` (missing the second `;` and the
increment/body entirely)
```
Parser expects ';' after the condition "i<10" but finds ')' instead
-> grammar rule for_stmt cannot match -> yyerror() is called
```
**Output:** `Result: Invalid FOR statement syntax (Error: syntax error)`

---

**Next:** `03-viva-questions.md` for a consolidated question bank covering
all 10 programs, for quick last-minute revision.
