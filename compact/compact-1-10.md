# Compact programs 1–10 (CSL 411 Cycle I and II)

Same idea as `compact/11.c`–`16.c`. **1007 lines → 337 lines.** Same algorithms. Q1–Q3 take **named-state input** like your `1-e-closure-1.c`. Line-by-line explanation: `compact-explained.md`.

## Contents
- [Q1. ε-closure of every state](#q1)
- [Q2. NFA with ε → NFA without ε](#q2)
- [Q3. NFA → DFA (subset construction)](#q3)
- [Q4. Minimize a DFA](#q4)
- [Q5. Lexical analyzer](#q5)
- [Q6. Lex: reject strings containing "mehr"](#q6)
- [Q7. YACC: valid variable name](#q7)
- [Q8. Calculator with LEX and YACC](#q8)
- [Q9. BNF → YACC: build an AST](#q9)
- [Q10. FOR statement syntax check](#q10)
- [Input formats](#input-formats)
- [Checks I ran](#checks-i-ran)

## Size

| Q | Program | Before | After |
|---|---|---|---|
| 1 | ε-closure of every state | 133 | 33 |
| 2 | NFA with ε → NFA without ε | 144 | 55 |
| 3 | NFA → DFA (subset construction) | 124 | 53 |
| 4 | Minimize a DFA | 84 | 33 |
| 5 | Lexical analyzer | 109 | 27 |
| 6 | Lex: reject strings containing "mehr" | 25 | 12 |
| 7 | YACC: valid variable name | 52 | 20 |
| 8 | Calculator with LEX and YACC | 80 | 31 |
| 9 | BNF → YACC: build an AST | 132 | 39 |
| 10 | FOR statement syntax check | 124 | 34 |

Q2 and Q3 grew a little from the earlier compact version because the named input needs code to read names.

<a id="input-formats"></a>
## Input formats

| Program | What you type | Notes |
|---|---|---|
| Q1 | `n`, state names, `m`, alphabet names, `t`, then `t` lines `source input destination` | `e` = ε. Non-ε lines are read and ignored. |
| Q2 | same as Q1, then a count of accepting states, then their names | The first state listed is the start state. `m` does not count `e`. |
| Q3 | same as Q2, but no `e` lines | The first state listed is the start state. |
| Q4 | `n`, `m`, alphabet letters, the table of next-states (one row per state), accepting numbers ending in -1 | Unchanged. Rows are state numbers, not names. |
| Q5 | reads the file `5-input.txt` | Run it in the folder that holds the file. |
| Q6–Q10 | type lines on the keyboard | Q9 reads one line and stops. Q7 and Q10 stop at the first error. |

<a id="q1"></a>
## Q1

**ε-closure of every state.** Named-state input like your `1-e-closure-1.c`. Same DFS.

Build: `gcc 1.c -o 1 && ./1`

```c
// Q1 (named input, same style as 1-e-closure-1.c): epsilon-closure of every state.
// Build: gcc 1.c -o 1      Input: states, alphabets, transitions "source input destination" (e = epsilon)
#include <stdio.h>
#include <string.h>
int n, m, t, e[10][10], v[10];                // e = epsilon matrix, v = visited
char st[10][20], al[10][20], a[20], s[20], b[20];
int id(char nm[][20], int k, char *w) { for (int i = 0; i < k; i++) if (!strcmp(nm[i], w)) return i; return -1; }
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }

int main() {
    int i, j, x, y;
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter the states:\n");
    for (i = 0; i < n; i++) scanf("%s", st[i]);
    printf("Enter number of alphabets: "); scanf("%d", &m);
    printf("Enter the alphabets:\n");
    for (i = 0; i < m; i++) scanf("%s", al[i]);
    printf("Enter number of transitions: "); scanf("%d", &t);
    printf("\nEnter transitions in the form:\nsource input destination\nUse 'e' for epsilon.\n\n");
    for (i = 0; i < t; i++) {
        scanf("%s %s %s", a, s, b);
        x = id(st, n, a); y = id(st, n, b);
        if (x < 0 || y < 0) { puts("Invalid state. Enter this transition again."); i--; continue; }
        if (!strcmp(s, "e")) e[x][y] = 1;     // only epsilon edges matter for the closure
    }
    printf("\n--- Epsilon Closures ---\n");
    for (i = 0; i < n; i++) {
        memset(v, 0, sizeof v); dfs(i);
        printf("Epsilon-Closure(%s) = { ", st[i]);
        for (j = 0; j < n; j++) if (v[j]) printf("%s ", st[j]);
        puts("}");
    }
}
```

Test input:
```
4
q0 q1 q2 q3
2
0 1
12
q0 e q0
q0 0 q1
q0 e q1
q0 1 q2
q0 e q3
q1 0 q0
q1 0 q2
q1 1 q2
q1 2 q3
q2 0 q2
q2 1 q2
q3 e q2
```
(The symbol `2` is not in the alphabet. Q1 ignores every symbol except `e`.)

<a id="q2"></a>
## Q2

**NFA with ε → NFA without ε.** Named-state input. Closure by DFS, like your original.

Build: `gcc 2.c -o 2 && ./2`

```c
// Q2 (named input): NFA with epsilon -> NFA without epsilon.   Build: gcc 2.c -o 2
// Input: states, alphabets (not counting e), transitions "source input destination" (e = epsilon),
//        then the accepting states.  The first state listed is the start state.
#include <stdio.h>
#include <string.h>
int n, m, a, i, j, k, q, p, t, u, t0, x, y;
char st[10][20], al[10][20], s1[20], s2[20], s3[20];
int c[10][10], d[10][10][10], nt[10][10][10], f[10], nf[10];   // c = eps edges, d = old delta, nt = new delta
int v[10], cl[10][10];                                          // v = visited, cl = closure table
int id(char nm[][20], int cnt, char *w) { for (int z = 0; z < cnt; z++) if (!strcmp(nm[z], w)) return z; return -1; }
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (c[s][j] && !v[j]) dfs(j); }

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter the states:\n");
    for (i = 0; i < n; i++) scanf("%s", st[i]);
    printf("Enter number of alphabets (excluding e): "); scanf("%d", &m);
    printf("Enter the alphabets:\n");
    for (i = 0; i < m; i++) scanf("%s", al[i]);
    printf("Enter number of transitions: "); scanf("%d", &t0);
    printf("\nEnter transitions in the form:\nsource input destination\nUse 'e' for epsilon.\n\n");
    while (t0 > 0) {
        scanf("%s %s %s", s1, s2, s3);
        x = id(st, n, s1); y = id(st, n, s3);
        a = strcmp(s2, "e") ? id(al, m, s2) : -2;                  // -2 = epsilon, -1 = unknown symbol
        if (x < 0 || y < 0 || a == -1) { puts("Invalid state or alphabet. Enter this transition again."); continue; }
        if (a == -2) c[x][y] = 1; else d[x][y][a] = 1;
        t0--;
    }
    printf("Enter number of accepting states: "); scanf("%d", &k);
    printf("Enter the accepting states:\n");
    while (k--) { scanf("%s", s1); if ((x = id(st, n, s1)) >= 0) f[x] = 1; }

    for (i = 0; i < n; i++) {                                       // closure of each state by DFS
        memset(v, 0, sizeof v); dfs(i);
        for (j = 0; j < n; j++) cl[i][j] = v[j];
    }
    // new delta(q,a) = closure( delta( closure(q), a ) )
    for (q = 0; q < n; q++) for (a = 0; a < m; a++) for (p = 0; p < n; p++) if (cl[q][p])
        for (t = 0; t < n; t++) if (d[p][t][a]) for (u = 0; u < n; u++) if (cl[t][u]) nt[q][u][a] = 1;
    // q is accepting if its closure contains an accepting state
    for (q = 0; q < n; q++) for (p = 0; p < n; p++) if (cl[q][p] && f[p]) nf[q] = 1;

    printf("\nNFA without ε transitions:\nStates: ");
    for (i = 0; i < n; i++) printf("%s ", st[i]);
    printf("\nTransitions:\n");
    for (q = 0; q < n; q++) for (a = 0; a < m; a++) {
        printf("δ(%s, %s) = { ", st[q], al[a]); k = 0;
        for (t = 0; t < n; t++) if (nt[q][t][a]) printf(k++ ? ", %s" : "%s", st[t]);
        printf(k ? " }\n" : "∅ }\n");
    }
    printf("Accepting states: "); k = 0;
    for (i = 0; i < n; i++) if (nf[i]) printf(k++ ? ", %s" : "%s", st[i]);
    printf(k ? "\n" : "∅\n");
}
```

Test input:
```
5
q0 q1 q2 q3 q4
2
a b
5
q0 e q1
q1 e q2
q2 a q2
q2 a q3
q3 b q4
1
q4
```

<a id="q3"></a>
## Q3

**NFA → DFA (subset construction).** Named-state input. Each NFA-state set is one bitmask int.

Build: `gcc 3.c -o 3 && ./3`

```c
// Q3 (named input): NFA -> DFA (subset construction).   Build: gcc 3.c -o 3
// Input: states, alphabets, transitions "source input destination", then the accepting states.
// The first state listed is the start state.  A set of NFA states is a bitmask: bit j set <=> state j is in it.
#include <stdio.h>
#include <string.h>
int n, m, i, j, k, x, y, fin, t0;             // fin = mask of NFA accepting states
char st[10][20], al[10][20], s1[20], s2[20], s3[20];
int tr[10][10];                               // tr[i][k] = mask of states reached from state i on alphabet k
int dm[1024], dt[1024][10], cnt = 1;          // dm[s] = NFA-set of DFA state s, dt = DFA transitions
int id(char nm[][20], int c, char *w) { for (int z = 0; z < c; z++) if (!strcmp(nm[z], w)) return z; return -1; }

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter the states:\n");
    for (i = 0; i < n; i++) scanf("%s", st[i]);
    printf("Enter number of alphabets: "); scanf("%d", &m);
    printf("Enter the alphabets:\n");
    for (i = 0; i < m; i++) scanf("%s", al[i]);
    printf("Enter number of transitions: "); scanf("%d", &t0);
    printf("\nEnter transitions in the form:\nsource input destination\n\n");
    while (t0 > 0) {
        scanf("%s %s %s", s1, s2, s3);
        x = id(st, n, s1); y = id(st, n, s3); k = id(al, m, s2);
        if (x < 0 || y < 0 || k < 0) { puts("Invalid state or alphabet. Enter this transition again."); continue; }
        tr[x][k] |= 1 << y;                   // set bit y in the move-set of (x, k)
        t0--;
    }
    printf("Enter number of accepting states: "); scanf("%d", &k);
    printf("Enter the accepting states:\n");
    while (k--) { scanf("%s", s1); if ((x = id(st, n, s1)) >= 0) fin |= 1 << x; }

    dm[0] = 1;                                // DFA state 0 = { first state }
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
        for (j = 0; j < n; j++) if (dm[i] >> j & 1) printf("%s ", st[j]);
        printf(dm[i] ? "}\n" : "∅ }\n");
    }
    printf("\nTransitions:\n");
    for (i = 0; i < cnt; i++) for (k = 0; k < m; k++) printf("δ(q%d, %s) = q%d\n", i, al[k], dt[i][k]);
    printf("\nAccepting states: ");
    for (i = 0; i < cnt; i++) if (dm[i] & fin) printf("q%d ", i);
    puts("");
}
```

Test input:
```
5
q0 q1 q2 q3 q4
2
a b
7
q0 a q2
q0 a q3
q1 a q2
q1 a q3
q2 a q2
q2 a q3
q3 b q4
1
q4
```

<a id="q4"></a>
## Q4

**Minimize a DFA.** Same table-filling. One `rep()` helper.

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

<a id="q5"></a>
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

<a id="q6"></a>
## Q6

**Lex: reject strings containing "mehr".** Same two rules. `%option noyywrap`.

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

<a id="q7"></a>
## Q7

**YACC: valid variable name.** Same grammar. `\n|.` returns the char.

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

<a id="q8"></a>
## Q8

**Calculator with LEX and YACC.** Same grammar and precedence. Division check is one line.

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

<a id="q9"></a>
## Q9

**BNF → YACC: build an AST.** Same layered grammar. Union carries the node.

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

<a id="q10"></a>
## Q10

**FOR statement syntax check.** Lexer returns one `RELOP` token. Same for-grammar, no conflicts.

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

- Q1: 300 random inputs, whole output identical to your `1-e-closure-1.c` (prompts included).
- Q2, Q3: 300 random inputs each, same results as the earlier number-input versions (which matched your original matrix programs on 300 random inputs each). Study-guide Example A matches.
- Q4: 300 random DFAs, output identical to your `4-min-dfa.c`.
- Q5: output identical to `5-lex.c` on `5-input.txt`.
- Q6–Q10: same results as your originals on all my test inputs (Q10 on 7 for-statements).
- Q9 drops the "AST Generated Successfully" banner; the tree output is the same.
- Q8: an unknown character (like `$`) now gives Yacc's `syntax error`.
- Built with `flex` and `bison -y`. Your lab machine's `lex`/`yacc` should behave the same.
