# Compact programs 1–10 (CSL 411 Cycle I and II)

Same idea as `compact/11.c`–`16.c`. **919 lines → 303 lines.** Same algorithms, same input/output format. Line-by-line explanation is in `compact-explained.md`.

## Contents
- [Q1. ε-closure](#q1)
- [Q2. NFA with ε → NFA without ε](#q2)
- [Q3. NFA → DFA](#q3)
- [Q4. Minimize DFA](#q4)
- [Q5. Lexical analyzer](#q5)
- [Q6. Lex: reject "mehr"](#q6)
- [Q7. YACC: valid variable](#q7)
- [Q8. Calculator](#q8)
- [Q9. AST from BNF](#q9)
- [Q10. FOR-statement syntax](#q10)
- [Checks I ran](#checks-i-ran)

## Size

| Q | Program | Before | After |
|---|---|---|---|
| 1 | ε-closure | 45 | 17 |
| 2 | NFA with ε → NFA without ε | 144 | 47 |
| 3 | NFA → DFA | 124 | 43 |
| 4 | Minimize DFA | 84 | 33 |
| 5 | Lexical analyzer | 109 | 27 |
| 6 | Lex: reject "mehr" | 25 | 12 |
| 7 | YACC: valid variable | 52 | 20 |
| 8 | Calculator | 80 | 31 |
| 9 | AST from BNF | 132 | 39 |
| 10 | FOR-statement syntax | 124 | 34 |

## Q1

**ε-closure.** Same DFS. Dropped helper noise.

Build: `gcc 1.c -o 1 && ./1`

```c
// Q1: epsilon-closure of every state.  Build: gcc 1.c -o 1
#include <stdio.h>
#include <string.h>
int n, e[10][10], v[10];                      // e = epsilon matrix, v = visited
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter epsilon transition matrix (%dx%d):\n", n, n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) scanf("%d", &e[i][j]);
    for (int i = 0; i < n; i++) {
        memset(v, 0, sizeof v); dfs(i);       // everything reachable by epsilon (incl. i itself)
        printf("Epsilon-Closure(q%d) = { ", i);
        for (int j = 0; j < n; j++) if (v[j]) printf("q%d ", j);
        puts("}");
    }
}
```

Test input:
```
4
0 1 0 0
0 0 1 0
0 0 0 1
0 0 0 0
```

## Q2

**NFA with ε → NFA without ε.** Same DFS closure as your original. Closure loop inline, global `v` instead of a passed array.

Build: `gcc 2.c -o 2 && ./2`

```c
// Q2: NFA with epsilon -> NFA without epsilon.  Build: gcc 2.c -o 2
#include <stdio.h>
#include <string.h>
int n, m, a, i, j, k, q, p, t, u;
int c[10][10], d[10][10][10], nt[10][10][10], f[10], nf[10];   // c = eps / closure, d = old delta, nt = new delta
char sym[10];
int v[10], cl[10][10];                        // v = visited, cl = closure table
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (c[s][j] && !v[j]) dfs(j); }   // c = epsilon edges

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter number of input symbols (excluding ε): "); scanf("%d", &m);
    printf("Enter the symbols (space separated): ");
    for (a = 0; a < m; a++) scanf(" %c", &sym[a]);
    printf("Enter epsilon transition matrix (%d x %d):\n", n, n);
    for (i = 0; i < n; i++) for (j = 0; j < n; j++) scanf("%d", &c[i][j]);
    printf("Enter transition matrices for each symbol (0/1):\n");
    for (a = 0; a < m; a++) {
        printf("Symbol %c:\n", sym[a]);
        for (i = 0; i < n; i++) for (j = 0; j < n; j++) scanf("%d", &d[i][j][a]);
    }
    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) f[i] = 1;

    for (i = 0; i < n; i++) {                                  // closure of each state by DFS
        memset(v, 0, sizeof v); dfs(i);
        for (j = 0; j < n; j++) cl[i][j] = v[j];
    }

    // new delta(q,a) = closure( delta( closure(q), a ) )
    for (q = 0; q < n; q++) for (a = 0; a < m; a++) for (p = 0; p < n; p++) if (cl[q][p])
        for (t = 0; t < n; t++) if (d[p][t][a]) for (u = 0; u < n; u++) if (cl[t][u]) nt[q][u][a] = 1;
    // q is accepting if its closure contains an accepting state
    for (q = 0; q < n; q++) for (p = 0; p < n; p++) if (cl[q][p] && f[p]) nf[q] = 1;

    printf("\nNFA without ε transitions:\nStates: ");
    for (i = 0; i < n; i++) printf("q%d ", i);
    printf("\nTransitions:\n");
    for (q = 0; q < n; q++) for (a = 0; a < m; a++) {
        printf("δ(q%d, %c) = { ", q, sym[a]); k = 0;
        for (t = 0; t < n; t++) if (nt[q][t][a]) printf(k++ ? ", q%d" : "q%d", t);
        printf(k ? " }\n" : "∅ }\n");
    }
    printf("Accepting states: "); k = 0;
    for (i = 0; i < n; i++) if (nf[i]) printf(k++ ? ", q%d" : "q%d", i);
    printf(k ? "\n" : "∅\n");
}
```

Test input:
```
4
2
a b
0 1 0 0
0 0 1 0
0 0 0 0
0 0 0 0
0 0 0 1
0 0 0 0
1 0 0 0
0 0 0 0
0 0 0 0
0 0 0 0
0 0 0 0
0 1 0 0
3 -1
```

## Q3

**NFA → DFA.** Each NFA-state set is one bitmask int. No set-compare helpers.

Build: `gcc 3.c -o 3 && ./3`

```c
// Q3: NFA -> DFA (subset construction).  Build: gcc 3.c -o 3
// A set of NFA states is stored as a bitmask: bit j set  <=>  q_j is in the set.
#include <stdio.h>
int n, m, i, j, k, x, fin;                    // fin = mask of NFA accepting states
int tr[10][10];                               // tr[i][k] = mask of states reached from q_i on symbol k
int dm[1024], dt[1024][10], cnt = 1;          // dm[s] = NFA-set of DFA state s, dt = DFA transitions
char sym[10];

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter number of input symbols: "); scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (k = 0; k < m; k++) scanf(" %c", &sym[k]);
    printf("Enter transition matrices (0/1) for each symbol:\n");
    for (k = 0; k < m; k++) {
        printf("Symbol %c:\n", sym[k]);
        for (i = 0; i < n; i++) for (j = 0; j < n; j++) { scanf("%d", &x); if (x) tr[i][k] |= 1 << j; }
    }
    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) fin |= 1 << i;

    dm[0] = 1;                                // DFA state 0 = { q0 }
    for (int cur = 0; cur < cnt; cur++)       // cnt grows as new subsets are found (acts as a queue)
        for (k = 0; k < m; k++) {
            int s = 0;
            for (i = 0; i < n; i++) if (dm[cur] >> i & 1) s |= tr[i][k];   // union of moves
            for (j = 0; j < cnt && dm[j] != s; j++);                       // already known?
            if (j == cnt) dm[cnt++] = s;
            dt[cur][k] = j;
        }

    printf("\n--- DFA RESULTS ---\nTotal DFA States: %d\n\nDFA States mapping:\n", cnt);
    for (i = 0; i < cnt; i++) {
        printf("q%d = { ", i);
        for (j = 0; j < n; j++) if (dm[i] >> j & 1) printf("q%d ", j);
        printf(dm[i] ? "}\n" : "∅ }\n");
    }
    printf("\nTransitions:\n");
    for (i = 0; i < cnt; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], dt[i][k]);
    printf("\nAccepting states: ");
    for (i = 0; i < cnt; i++) if (dm[i] & fin) printf("q%d ", i);
    puts("");
}
```

Test input:
```
3
2
a b
0 1 1
0 0 1
0 0 0
1 0 0
0 1 0
0 0 1
2 -1
```

## Q4

**Minimize DFA.** Same table-filling. Merged loops, one `rep()` helper.

Build: `gcc 4.c -o 4 && ./4`

```c
// Q4: minimize a DFA (table-filling / marking distinguishable pairs).  Build: gcc 4.c -o 4
#include <stdio.h>
int n, m, i, j, k, ch, cc;
int d[10][10], f[10], x[10][10], cls[10];     // x[i][j] = 1 if states i and j are distinguishable
char sym[10];
int rep(int c) { int r = 0; while (cls[r] != c) r++; return r; }   // first state of class c

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter number of input symbols: "); scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (i = 0; i < m; i++) scanf(" %c", &sym[i]);
    printf("Enter transition table (row = state, col = symbol):\n");
    for (i = 0; i < n; i++) for (j = 0; j < m; j++) scanf("%d", &d[i][j]);
    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) f[i] = 1;

    for (i = 0; i < n; i++) for (j = 0; j < n; j++) x[i][j] = f[i] != f[j];   // accepting vs non-accepting
    do {                                                                       // repeat until no new pair is marked
        ch = 0;
        for (i = 0; i < n; i++) for (j = i + 1; j < n; j++) if (!x[i][j])
            for (k = 0; k < m; k++) if (x[d[i][k]][d[j][k]]) { x[i][j] = x[j][i] = ch = 1; break; }
    } while (ch);

    for (i = 0; i < n; i++) cls[i] = -1;                                       // unmarked pairs = same class
    for (i = 0; i < n; i++) if (cls[i] < 0) { for (j = i; j < n; j++) if (!x[i][j]) cls[j] = cc; cc++; }

    printf("\n--- Minimized DFA ---\nTotal States: %d\n\nTransitions:\n", cc);
    for (i = 0; i < cc; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], cls[d[rep(i)][k]]);
    printf("\nAccepting states: ");
    for (i = 0; i < cc; i++) if (f[rep(i)]) printf("q%d ", i);
    printf("\nStart state: q%d\n", cls[0]);
}
```

Test input:
```
6
2
0 1
1 2
3 4
3 4
5 5
5 5
5 5
3 4 5 -1
```

## Q5

**Lexical analyzer.** `ungetc` instead of `fseek`. `strchr` instead of long `||` chains.

Build: `gcc 5.c -o 5 && ./5`

```c
// Q5: lexical analyzer (skips spaces/tabs/newlines).  Reads 5-input.txt.  Build: gcc 5.c -o 5
#include <stdio.h>
#include <ctype.h>
#include <string.h>
char *kw[] = {"int","float","char","double","if","else","while","for","return","void","break","continue","do","switch","case","default"};
int isKw(char *s) { for (int i = 0; i < 16; i++) if (!strcmp(s, kw[i])) return 1; return 0; }

int main() {
    FILE *fp = fopen("5-input.txt", "r");
    if (!fp) { puts("File not found"); return 1; }
    int c, i; char t[100];
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) continue;                                        // ignore blanks
        if (isalpha(c) || c == '_') {                                    // identifier / keyword
            for (i = 0; isalnum(c) || c == '_'; c = fgetc(fp)) t[i++] = c;
            t[i] = 0; ungetc(c, fp);                                     // push back the char that ended the token
            printf("%s : %s\n", t, isKw(t) ? "Keyword" : "Identifier");
        } else if (isdigit(c)) {                                         // number
            for (i = 0; isdigit(c); c = fgetc(fp)) t[i++] = c;
            t[i] = 0; ungetc(c, fp);
            printf("%s : Number\n", t);
        }
        else if (strchr("+-*/=<>%", c)) printf("%c : Operator\n", c);
        else if (strchr(";,(){}[]", c)) printf("%c : Special Symbol\n", c);
        else printf("%c : Unknown\n", c);
    }
}
```

Test input:
```
(reads 5-input.txt, no typing)
```

## Q6

**Lex: reject "mehr".** Same two rules. `%option noyywrap`.

Build: `lex 6.l && gcc lex.yy.c -o 6 && ./6`

```lex
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
```

Test input:
```
hello
xMehRx
me hr
```

## Q7

**YACC: valid variable.** Same grammar. `\n|.` returns the char.

Build: `lex 7.l && yacc -d 7.y && gcc lex.yy.c y.tab.c -o 7 && ./7`

`7.l`

```lex
%{
// Q7 (lex part).  Build: lex 7.l && yacc -d 7.y && gcc lex.yy.c y.tab.c -o 7
#include "y.tab.h"
%}
%option noyywrap
%%
[a-zA-Z][a-zA-Z0-9]*   return VARIABLE;
[ \t]                  ;
\n|.                   return yytext[0];
```

`7.y`

```yacc
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
```

Test input:
```
abc1
1abc
```

## Q8

**Calculator.** Same grammar and precedence. Division check is one line.

Build: `lex 8.l && yacc -d 8.y && gcc lex.yy.c y.tab.c -o 8 && ./8`

`8.l`

```lex
%{
// Q8 (lex part).  Build: lex 8.l && yacc -d 8.y && gcc lex.yy.c y.tab.c -o 8
#include <stdlib.h>
#include "y.tab.h"
%}
%option noyywrap
%%
[0-9]+     { yylval = atoi(yytext); return NUMBER; }
[ \t]      ;
\n|.       return yytext[0];
```

`8.y`

```yacc
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
```

Test input:
```
2+3*4
(2+3)*4
10/0
```

## Q9

**AST from BNF.** Same layered grammar. Union carries the node; precedence lines dropped (grammar is already layered).

Build: `lex 9.l && yacc -d 9.y && gcc lex.yy.c y.tab.c -o 9 && ./9`

`9.l`

```lex
%{
// Q9 (lex part).  Build: lex 9.l && yacc -d 9.y && gcc lex.yy.c y.tab.c -o 9
#include <string.h>
#include "y.tab.h"
%}
%option noyywrap
%%
[a-zA-Z][a-zA-Z0-9]*   { yylval.s = strdup(yytext); return ID; }
[0-9]+                 { yylval.s = strdup(yytext); return NUM; }
[ \t]                  ;
\n                     return 0;          /* end of input */
.                      return yytext[0];
```

`9.y`

```yacc
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
```

Test input:
```
x = a + b * c
```

## Q10

**FOR-statement syntax.** Lexer returns one `RELOP` token for `< > <= >= == !=`. Same for-grammar.

Build: `lex 10.l && yacc -d 10.y && gcc lex.yy.c y.tab.c -o 10 && ./10`

`10.l`

```lex
%{
// Q10 (lex part).  Build: lex 10.l && yacc -d 10.y && gcc lex.yy.c y.tab.c -o 10
#include "y.tab.h"
%}
%option noyywrap
%%
"for"                      return FOR;
[a-zA-Z_][a-zA-Z0-9_]*     return ID;
[0-9]+                     return NUM;
"<="|">="|"=="|"!="|"<"|">"  return RELOP;
"++"                       return INC;
"--"                       return DEC;
[ \t\n]                    ;
.                          return yytext[0];
```

`10.y`

```yacc
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
```

Test input:
```
for(i=0;i<10;i++){ a=b+1; }
```

## Checks I ran

- Q1–Q5: compact and original give **byte-identical output** on my test inputs.
- Q2, Q3, Q4: also 300 random inputs each. 0 mismatches.
- Q6–Q10: same accept/reject and same results as the originals on all my test inputs (Q10 on 7 for-statements, valid and broken).
- Q9: drops the "AST Generated Successfully" banner line. The tree output is the same.
- Q8: an unknown character (like `$`) now gives yacc's `syntax error`. The original printed its own "Unknown character" message from lex.
- Q1 uses the matrix input (your `1-e-closure.c`), not the named-state `1-e-closure-1.c`.
- Q2 uses DFS for the closure, like your original.
- Not run: I used `flex` and `bison -y` here. Your lab machine's `lex`/`yacc` should behave the same.
