# Compact Q1–Q10: line by line, and what changed

Line numbers (L5 = line 5) match the files in `compact/`. Each program has: the idea, the code in pieces with each line explained, a table of what I cut or changed from your original, and a one-line **Keep this**.

## Contents
- [Which ones use DFS](#which-ones-use-dfs)
- [How lex and yacc fit together](#how-lex-and-yacc-fit-together)
- [Q1 ε-closure](#q1-ε-closure) · [Q2 NFA with ε → NFA](#q2-nfa-with-ε--nfa) · [Q3 NFA → DFA](#q3-nfa--dfa) · [Q4 Minimize DFA](#q4-minimize-dfa) · [Q5 Lexical analyzer](#q5-lexical-analyzer)
- [Q6 Lex "mehr"](#q6-lex-mehr) · [Q7 Variable](#q7-variable) · [Q8 Calculator](#q8-calculator) · [Q9 AST](#q9-ast) · [Q10 FOR syntax](#q10-for-syntax)
- [What I ran to check](#what-i-ran-to-check)

## Which ones use DFS

| Q | Uses DFS? |
|---|---|
| 1 | **Yes.** `dfs()` walks the ε edges. |
| 2 | **Yes.** Same `dfs()` finds each state's closure. (My first compact version used Warshall; I switched it back to DFS to match your original.) |
| 3 | No. It is a **worklist**: a growing list of new DFA states, handled in order (BFS-like). |
| 4 | No. Repeated sweeps over a table until nothing changes. |
| 5 | No. Reads characters one at a time. |
| 6–10 | No. Lex and yacc generate the scanning and parsing code. |

## How lex and yacc fit together

- **Lex** cuts text into tokens. Each rule is `pattern  action`.
- **Yacc** checks that the tokens follow the grammar.
- `yyparse()` asks `yylex()` for the next token again and again.
- `return NAME;` hands the parser a token. A single character like `'+'` is returned as itself.
- `yytext` = the text the pattern just matched. `yylval` = a value passed along with the token.
- `yacc -d` writes `y.tab.h` holding the token names, so the lexer can use them.
- In a yacc rule, `$$` = the value of the left side. `$1`, `$2`, `$3` = values of the right-side items, in order.
- Lex picks the **longest** match. If two rules tie, the **earlier** rule wins.

## Q1 ε-closure

**Picture.** ε edges are one-way roads you can take for free. The closure of a state is every state you can reach using only those roads (including the start). DFS = walk as far as you can, mark each place you visit, never visit twice.

`1.c` lines 4–5
```c
 4  int n, e[10][10], v[10];                      // e = epsilon matrix, v = visited
 5  void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }
```
- L4: `n` = number of states. `e[i][j]` = 1 means an ε edge from qi to qj. `v[s]` = 1 means DFS already visited state s.
- L5: `dfs(s)` marks s visited. Then for every state j: if there is an edge s→j and j is not visited yet, go to j. That recursion is the DFS.

`1.c` lines 8–10
```c
 8      printf("Enter number of states: "); scanf("%d", &n);
 9      printf("Enter epsilon transition matrix (%dx%d):\n", n, n);
10      for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) scanf("%d", &e[i][j]);
```
- L8–9: read `n`, then the n×n 0/1 matrix of ε edges.
- L10: store each number in `e`.

`1.c` lines 11–16
```c
11      for (int i = 0; i < n; i++) {
12          memset(v, 0, sizeof v); dfs(i);       // everything reachable by epsilon (incl. i itself)
13          printf("Epsilon-Closure(q%d) = { ", i);
14          for (int j = 0; j < n; j++) if (v[j]) printf("q%d ", j);
15          puts("}");
16      }
```
- L11: take each state `i` as the start.
- L12: `memset` sets all of `v` to 0 (fresh start). `dfs(i)` marks everything reachable from i, including i itself.
- L13–15: print every marked state. Those are the closure.

**Example (your `1-automata.txt`):** ε edges q0→q1, q1→q2, q2→q3 (the `a` and `b` lines are not ε, so they are ignored). Output I got:
```
Epsilon-Closure(q0) = { q0 q1 q2 q3 }
Epsilon-Closure(q1) = { q1 q2 q3 }
Epsilon-Closure(q2) = { q2 q3 }
Epsilon-Closure(q3) = { q3 }
```

**What changed from `1-e-closure.c`**

| Original | Compact |
|---|---|
| globals `eps`, `visited` | `e`, `v` (just shorter names) |
| inner loop `for (j…) visited[j] = 0;` | `memset(v, 0, sizeof v)`. Needs `#include <string.h>`. |
| `int i, j;` at top of `main` | declared inside each `for` |
| `printf("}\n"); … return 0;` | `puts("}")`; no `return 0` (main returns 0 by itself) |
| blank lines, braces | removed |

**Keep this:** DFS from each state over ε edges; whatever gets marked is the closure.

## Q2 NFA with ε → NFA

**Picture.** To remove ε edges, let every state *absorb* what it can do through ε. From state q on symbol a: first slide along ε for free, then take one `a` edge, then slide along ε again. Rule:

**new δ(q, a) = closure( δ( closure(q), a ) )**

(δ = "where do I go" function. closure = the Q1 result.) A state also becomes accepting if its closure contains an accepting state.

`2.c` lines 4–8
```c
 4  int n, m, a, i, j, k, q, p, t, u;
 5  int c[10][10], d[10][10][10], nt[10][10][10], f[10], nf[10];   // c = eps / closure, d = old delta, nt = new delta
 6  char sym[10];
 7  int v[10], cl[10][10];                        // v = visited, cl = closure table
 8  void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (c[s][j] && !v[j]) dfs(j); }   // c = epsilon edges
```
- L4: `n` states, `m` symbols. `a` = symbol number. `q p t u` are state counters used in the big loop later. `i j k` are spare counters.
- L5: `c[i][j]` = ε matrix. `d[i][j][a]` = 1 if qi goes to qj on symbol a (old δ). `nt` = the new δ being built. `f` = accepting states, `nf` = new accepting states.
- L7: `v` = DFS visited list. `cl[i][j]` = 1 if qj is in the closure of qi.
- L8: same `dfs` as Q1, but it reads `c`.

`2.c` lines 11–23
```c
11      printf("Enter number of states: "); scanf("%d", &n);
12      printf("Enter number of input symbols (excluding ε): "); scanf("%d", &m);
13      printf("Enter the symbols (space separated): ");
14      for (a = 0; a < m; a++) scanf(" %c", &sym[a]);
15      printf("Enter epsilon transition matrix (%d x %d):\n", n, n);
16      for (i = 0; i < n; i++) for (j = 0; j < n; j++) scanf("%d", &c[i][j]);
17      printf("Enter transition matrices for each symbol (0/1):\n");
18      for (a = 0; a < m; a++) {
19          printf("Symbol %c:\n", sym[a]);
20          for (i = 0; i < n; i++) for (j = 0; j < n; j++) scanf("%d", &d[i][j][a]);
21      }
22      printf("Enter accepting states (enter -1 to stop): ");
23      while (scanf("%d", &i) && i != -1) f[i] = 1;
```
- L11–14: read `n`, `m`, and the symbol letters.
- L15–16: read the ε matrix into `c`.
- L17–21: for each symbol, read one n×n matrix into `d`.
- L22–23: read accepting states until you type -1, and mark them in `f`.

`2.c` lines 25–28
```c
25      for (i = 0; i < n; i++) {                                  // closure of each state by DFS
26          memset(v, 0, sizeof v); dfs(i);
27          for (j = 0; j < n; j++) cl[i][j] = v[j];
28      }
```
- L25: for every state i…
- L26: …clear `v` and run DFS from i.
- L27: copy the result row into `cl[i]`. Now `cl` holds all closures.

`2.c` lines 30–34
```c
30      // new delta(q,a) = closure( delta( closure(q), a ) )
31      for (q = 0; q < n; q++) for (a = 0; a < m; a++) for (p = 0; p < n; p++) if (cl[q][p])
32          for (t = 0; t < n; t++) if (d[p][t][a]) for (u = 0; u < n; u++) if (cl[t][u]) nt[q][u][a] = 1;
33      // q is accepting if its closure contains an accepting state
34      for (q = 0; q < n; q++) for (p = 0; p < n; p++) if (cl[q][p] && f[p]) nf[q] = 1;
```
- L31–32: the rule above, as loops. `q` = start state, `a` = symbol, `p` = a state in closure(q), `t` = a state p reaches on a, `u` = a state in closure(t). Then mark `nt[q][u][a] = 1`.
- L34: q is accepting if some state `p` in its closure is accepting.

`2.c` lines 36–46
```c
36      printf("\nNFA without ε transitions:\nStates: ");
37      for (i = 0; i < n; i++) printf("q%d ", i);
38      printf("\nTransitions:\n");
39      for (q = 0; q < n; q++) for (a = 0; a < m; a++) {
40          printf("δ(q%d, %c) = { ", q, sym[a]); k = 0;
41          for (t = 0; t < n; t++) if (nt[q][t][a]) printf(k++ ? ", q%d" : "q%d", t);
42          printf(k ? " }\n" : "∅ }\n");
43      }
44      printf("Accepting states: "); k = 0;
45      for (i = 0; i < n; i++) if (nf[i]) printf(k++ ? ", q%d" : "q%d", i);
46      printf(k ? "\n" : "∅\n");
```
- L37: list states.
- L39–43: for each state and symbol print `δ(q, a) = { … }`. `k` counts how many items printed. `printf(k++ ? ", q%d" : "q%d", t)` prints a comma before every item except the first. L42 prints `∅` if nothing was printed.
- L44–46: same trick for the accepting list.

**Example I ran:** 4 states. ε: q0→q1, q1→q2. On `a`: q0→q3, q2→q0. On `b`: q3→q1. Accepting: q3.
Check `δ(q0, a)`: closure(q0) = {q0,q1,q2}. On `a` they reach q3 and q0. closure(q3) ∪ closure(q0) = {q0,q1,q2,q3}. The program printed `{ q0, q1, q2, q3 }`. ✓

**What changed from `2-nfa-enfa.c`**

| Original | Compact |
|---|---|
| `dfs(int state, int visited[])` + a `computeClosure()` function that copies into `closure` | global `v` and the closure loop written inline (L25–28) into `cl`. Still DFS. |
| names `eps`, `trans`, `newTrans`, `accepting`, `newAccepting`, `symbols` | `c`, `d`, `nt`, `f`, `nf`, `sym` |
| loops that set `newTrans` and `accepting` to 0 | dropped. Global arrays already start at 0. |
| accepting input: `while(1)` plus `if (state >= 0 && state < n)` | `while (scanf(…) && i != -1) f[i] = 1;` **No range check now.** Typing a state ≥ 10 would write outside the array. |
| `newAccepting[q] = accepting[q]`, then a loop with `break` | `nf[q] = 1` if any closure state is accepting. Same result, because the closure of q includes q. |
| `first` flag with `if (!first) printf(", ")` | the `k++ ? … : …` trick |
| 5 nested loops for new δ | kept as is (L31–32), just written on two lines |

**Keep this:** closure(q) → move on a → closure again; accepting if the closure touches an accepting state.

## Q3 NFA → DFA

**Picture.** One DFA state = a *group* of NFA states you could be in at the same time. Start with the group {q0}. For each group and each symbol, find where *everyone* in the group can go, and take the union. If that group is new, make it a new DFA state. Repeat until no new groups appear.

**Bitmask trick.** A group is stored in one `int`. Bit j is 1 when qj is in the group. {q1,q2} = bits 1 and 2 = binary `110` = 6. Union of two groups = `|`. Test "is qi in the group" = `group >> i & 1`.

`3.c` lines 4–7
```c
 4  int n, m, i, j, k, x, fin;                    // fin = mask of NFA accepting states
 5  int tr[10][10];                               // tr[i][k] = mask of states reached from q_i on symbol k
 6  int dm[1024], dt[1024][10], cnt = 1;          // dm[s] = NFA-set of DFA state s, dt = DFA transitions
 7  char sym[10];
```
- L4: `n` states, `m` symbols. `fin` = bitmask of accepting NFA states.
- L5: `tr[i][k]` = bitmask of all states reachable from qi on symbol k. One int replaces a whole matrix row.
- L6: `dm[s]` = the group (bitmask) that DFA state s stands for. `dt[s][k]` = DFA transition. `cnt` = DFA states found so far, starts at 1 (state 0).
- L7: the symbol letters.

`3.c` lines 10–20
```c
10      printf("Enter number of states: "); scanf("%d", &n);
11      printf("Enter number of input symbols: "); scanf("%d", &m);
12      printf("Enter symbols (space separated): ");
13      for (k = 0; k < m; k++) scanf(" %c", &sym[k]);
14      printf("Enter transition matrices (0/1) for each symbol:\n");
15      for (k = 0; k < m; k++) {
16          printf("Symbol %c:\n", sym[k]);
17          for (i = 0; i < n; i++) for (j = 0; j < n; j++) { scanf("%d", &x); if (x) tr[i][k] |= 1 << j; }
18      }
19      printf("Enter accepting states (enter -1 to stop): ");
20      while (scanf("%d", &i) && i != -1) fin |= 1 << i;
```
- L10–13: read `n`, `m`, symbols.
- L15–18: read each matrix. L17: if the entry is 1, turn on bit j of `tr[i][k]` (`1 << j` is a 1 shifted left j places).
- L19–20: read accepting states, turn on their bits in `fin`.

`3.c` lines 22–30
```c
22      dm[0] = 1;                                // DFA state 0 = { q0 }
23      for (int cur = 0; cur < cnt; cur++)       // cnt grows as new subsets are found (acts as a queue)
24          for (k = 0; k < m; k++) {
25              int s = 0;
26              for (i = 0; i < n; i++) if (dm[cur] >> i & 1) s |= tr[i][k];   // union of moves
27              for (j = 0; j < cnt && dm[j] != s; j++);                       // already known?
28              if (j == cnt) dm[cnt++] = s;
29              dt[cur][k] = j;
30          }
```
- L22: DFA state 0 = {q0} = only bit 0 on = value 1.
- L23: `cur` goes through DFA states in order. `cnt` grows while we loop, so new states get their turn too. That is the worklist.
- L24: for each symbol k.
- L25–26: `s` starts empty. For every NFA state i inside the current group, add `tr[i][k]` into `s` (union of moves).
- L27: look for `s` among the groups already found. The loop stops at the first match; if it runs to `cnt`, `s` is new.
- L28: new group → store it as DFA state `cnt`, then `cnt++`.
- L29: record that `cur` on symbol k goes to state `j`.

`3.c` lines 32–42
```c
32      printf("\n--- DFA RESULTS ---\nTotal DFA States: %d\n\nDFA States mapping:\n", cnt);
33      for (i = 0; i < cnt; i++) {
34          printf("q%d = { ", i);
35          for (j = 0; j < n; j++) if (dm[i] >> j & 1) printf("q%d ", j);
36          printf(dm[i] ? "}\n" : "∅ }\n");
37      }
38      printf("\nTransitions:\n");
39      for (i = 0; i < cnt; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], dt[i][k]);
40      printf("\nAccepting states: ");
41      for (i = 0; i < cnt; i++) if (dm[i] & fin) printf("q%d ", i);
42      puts("");
```
- L33–37: for each DFA state print its members. Empty group prints `∅`.
- L39: print all DFA transitions.
- L41: a DFA state is accepting if its group shares a bit with `fin`: `dm[i] & fin` is not 0.

**Example I ran:** 3 states. `a`: q0→q1, q0→q2, q1→q2. `b`: each state loops to itself. Accepting: q2.
```
q0 = { q0 }     q1 = { q1 q2 }     q2 = { q2 }     q3 = { ∅ }
Accepting states: q1 q2
```
Check: from {q0} on `a` → {q1,q2} (that is DFA q1). From {q1,q2} on `a` → q1→q2, q2→nothing, so {q2}. From {q2} on `a` → nothing = ∅ (a dead state). ✓

**What changed from `3-nfa-dfa.c`**

| Original | Compact |
|---|---|
| `dfaStates[1<<MAX][MAX]`: a 0/1 array for every group | `dm[1024]`: one int per group |
| `isSameState()` + `findDFAState()` (two functions, two loops) | one `for` with `dm[j] != s` (L27) |
| `startState[]` array set to {q0} | `dm[0] = 1` |
| `dfaAccept[]` array filled inside the main loop | not stored; checked when printing with `dm[i] & fin` |
| 3-D `trans[i][j][k]` | 2-D `tr[i][k]` holding a bitmask |
| accepting-state range check | dropped (same warning as Q2) |

**Keep this:** a DFA state is a set of NFA states; from each set take the union of the moves; new set → new state.

## Q4 Minimize DFA

**Picture (table-filling).** Two states are *different* if some input makes one end in an accepting state and the other not. Start: accepting vs non-accepting are different. Then: if on some symbol two states land on a pair that is already known different, they are different too. Repeat until nothing changes. Pairs never marked are the same, so merge them.

`4.c` lines 3–6
```c
 3  int n, m, i, j, k, ch, cc;
 4  int d[10][10], f[10], x[10][10], cls[10];     // x[i][j] = 1 if states i and j are distinguishable
 5  char sym[10];
 6  int rep(int c) { int r = 0; while (cls[r] != c) r++; return r; }   // first state of class c
```
- L3: `ch` = "something changed this round". `cc` = number of merged states.
- L4: `d[s][k]` = next state of s on symbol k. `f[s]` = 1 if s is accepting. `x[i][j]` = 1 means i and j are different. `cls[s]` = the merged-state number of s.
- L6: `rep(c)` = the first old state that belongs to merged state c (its representative).

`4.c` lines 9–16
```c
 9      printf("Enter number of states: "); scanf("%d", &n);
10      printf("Enter number of input symbols: "); scanf("%d", &m);
11      printf("Enter symbols (space separated): ");
12      for (i = 0; i < m; i++) scanf(" %c", &sym[i]);
13      printf("Enter transition table (row = state, col = symbol):\n");
14      for (i = 0; i < n; i++) for (j = 0; j < m; j++) scanf("%d", &d[i][j]);
15      printf("Enter accepting states (enter -1 to stop): ");
16      while (scanf("%d", &i) && i != -1) f[i] = 1;
```
- L9–12: read `n`, `m`, symbols.
- L13–14: read the transition table (row = state, column = symbol).
- L15–16: read accepting states.

`4.c` lines 18–23
```c
18      for (i = 0; i < n; i++) for (j = 0; j < n; j++) x[i][j] = f[i] != f[j];   // accepting vs non-accepting
19      do {                                                                       // repeat until no new pair is marked
20          ch = 0;
21          for (i = 0; i < n; i++) for (j = i + 1; j < n; j++) if (!x[i][j])
22              for (k = 0; k < m; k++) if (x[d[i][k]][d[j][k]]) { x[i][j] = x[j][i] = ch = 1; break; }
23      } while (ch);
```
- L18: first marks. `f[i] != f[j]` is 1 when exactly one of them is accepting.
- L19–20: start a round with `ch = 0`.
- L21: look at every pair i<j that is not marked yet.
- L22: for each symbol, if their next states are a marked pair, mark this pair too (both `x[i][j]` and `x[j][i]`), set `ch = 1`, stop checking this pair (`break`).
- L23: if anything changed, do another round.

`4.c` lines 25–26
```c
25      for (i = 0; i < n; i++) cls[i] = -1;                                       // unmarked pairs = same class
26      for (i = 0; i < n; i++) if (cls[i] < 0) { for (j = i; j < n; j++) if (!x[i][j]) cls[j] = cc; cc++; }
```
- L25: nobody has a merged number yet (-1).
- L26: for each state i without a number: give i the number `cc`. Every j ≥ i that is not different from i gets the same number. Then `cc++`.

`4.c` lines 28–32
```c
28      printf("\n--- Minimized DFA ---\nTotal States: %d\n\nTransitions:\n", cc);
29      for (i = 0; i < cc; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], cls[d[rep(i)][k]]);
30      printf("\nAccepting states: ");
31      for (i = 0; i < cc; i++) if (f[rep(i)]) printf("q%d ", i);
32      printf("\nStart state: q%d\n", cls[0]);
```
- L29: for each merged state i and symbol k: take its representative `rep(i)`, find where that goes (`d[rep(i)][k]`), then convert that old state to its merged number with `cls[…]`.
- L31: a merged state is accepting if its representative is.
- L32: the start state is the merged number of old q0.

**Example I ran:** 6 states, symbols 0 1. Moves: q0→(q1,q2), q1→(q3,q4), q2→(q3,q4), q3,q4,q5→(q5,q5). Accepting: q3,q4,q5. Result: **3 states**. q1 and q2 merge, and q3,q4,q5 merge.
```
Total States: 3          Accepting states: q2          Start state: q0
```
(Like your original, it does not remove unreachable states.)

**What changed from `4-min-dfa.c`**

| Original | Compact |
|---|---|
| `delta`, `accepting`, `dist`, `class`, `classCount` | `d`, `f`, `x`, `cls`, `cc` |
| the loop `int rep = 0; while (class[rep] != i) rep++;` written twice | one function `rep()` (L6) |
| `for (…) { if (…) { for (…) { if (…) { … } } } }` | same logic with the braces removed |
| `if (class[i] == -1) { … }` | same, one line (L26) |

**Keep this:** mark different pairs, spread the marks until stable, merge the unmarked pairs.

## Q5 Lexical analyzer

**Picture.** Read one character. It decides what kind of token starts here. Keep reading while characters still fit that kind. The first character that does not fit is *pushed back* so the next token can use it.

`5.c` lines 5–6
```c
 5  char *kw[] = {"int","float","char","double","if","else","while","for","return","void","break","continue","do","switch","case","default"};
 6  int isKw(char *s) { for (int i = 0; i < 16; i++) if (!strcmp(s, kw[i])) return 1; return 0; }
```
- L5: the keyword list. L6: `isKw` checks if a word equals any of the 16 keywords (`strcmp` returns 0 on equal). If you add a keyword, change the `16`.

`5.c` lines 9–13
```c
 9      FILE *fp = fopen("5-input.txt", "r");
10      if (!fp) { puts("File not found"); return 1; }
11      int c, i; char t[100];
12      while ((c = fgetc(fp)) != EOF) {
13          if (isspace(c)) continue;                                        // ignore blanks
```
- L9–10: open `5-input.txt`; stop with a message if missing.
- L11: `c` = current character. **It is an `int`** so it can hold `EOF`. `t` = the token text being built.
- L12: read characters until end of file.
- L13: `isspace` is true for space, tab, newline. Skip them.

`5.c` lines 14–22
```c
14          if (isalpha(c) || c == '_') {                                    // identifier / keyword
15              for (i = 0; isalnum(c) || c == '_'; c = fgetc(fp)) t[i++] = c;
16              t[i] = 0; ungetc(c, fp);                                     // push back the char that ended the token
17              printf("%s : %s\n", t, isKw(t) ? "Keyword" : "Identifier");
18          } else if (isdigit(c)) {                                         // number
19              for (i = 0; isdigit(c); c = fgetc(fp)) t[i++] = c;
20              t[i] = 0; ungetc(c, fp);
21              printf("%s : Number\n", t);
22          }
```
- L14: a letter or `_` starts an identifier or keyword.
- L15: copy characters into `t` while they are letters, digits or `_`. Then read the next one. The loop ends on the first character that does not fit; it is left in `c`.
- L16: end the string with 0. `ungetc(c, fp)` puts that extra character back into the file stream.
- L17: print `Keyword` or `Identifier`.
- L18–21: same idea for numbers: a digit starts it, digits continue it.

`5.c` lines 23–25
```c
23          else if (strchr("+-*/=<>%", c)) printf("%c : Operator\n", c);
24          else if (strchr(";,(){}[]", c)) printf("%c : Special Symbol\n", c);
25          else printf("%c : Unknown\n", c);
```
- L23: `strchr("+-*/=<>%", c)` is true if `c` is one of those characters, so it is an operator.
- L24: same for special symbols `; , ( ) { } [ ]`.
- L25: anything else is Unknown.

Output for your `5-input.txt` (first lines):
```
int : Keyword
a : Identifier
= : Operator
25 : Number
; : Special Symbol
float : Keyword
num : Identifier
= : Operator
```

**What changed from `5-lex.c`**

| Original | Compact |
|---|---|
| `char ch;` | `int c;` EOF is -1. On machines where `char` is unsigned, the original `ch != EOF` test can never be false, so the loop would never end. |
| `fseek(fp, -1, SEEK_CUR)` guarded by `if (ch != EOF)` | `ungetc(c, fp)`. It does nothing when `c` is EOF, so no guard needed. |
| `isKeyword` with a local array and `sizeof` to count | global `kw[]` and a fixed 16 |
| `ch == ' ' \|\| ch == '\t' \|\| ch == '\n'` | `isspace(c)` (also skips `\r` and a few others) |
| long `\|\|` chains for operators and symbols | `strchr("…", c)` |
| file missing: `return 0` | `return 1` (non-zero = error) |
| identifier loop `while ((ch = fgetc(fp)) != EOF && …) token[i++] = ch;` | same job as `for (…; c = fgetc(fp)) t[i++] = c;` |

**Keep this:** first character picks the token type; read while it fits; push back the one that doesn't.

## Q6 Lex "mehr"

**Picture.** One pass over each line. If the line contains m-e-h-r in any capitals, print Rejected. Otherwise Accepted.

`6.l` lines 1–12
```lex
 1  %{
 2  // Q6: reject strings containing the first four letters of the name ("mehr", any case).
 3  // Build: lex 6.l && gcc lex.yy.c -o 6
 4  #include <stdio.h>
 5  %}
 6  %option noyywrap
 7  %%
 8  .*[mM][eE][hH][rR].*   puts("Rejected");     /* tie in length with next rule -> first rule wins */
 9  .*                     puts("Accepted");
10  \n                     ;
11  %%
12  int main() { puts("Enter String:"); yylex(); }
```
- L1–5: C code copied as is into the generated program. L6: `%option noyywrap` says "stop at end of input". It replaces `int yywrap() { return 1; }`.
- L8: `.*` any characters (`.` = any char except newline). `[mM][eE][hH][rR]` = m, e, h, r in either case. Then `.*` again. So: any line that contains "mehr". Action: print Rejected.
- L9: `.*` any line at all. Action: print Accepted.
- Why L8 wins when "mehr" is present: both rules match the whole line, so they tie on length, and the **earlier rule wins**. If there is no "mehr", L8 does not match and L9 does.
- L10: `\n` with an empty action swallows the newline. On an empty line, `.*` matches 0 characters but `\n` matches 1 (longer), so `\n` wins.
- L12: `main` prints the prompt and calls `yylex()`. It scans line after line until end of input.

**Example I ran:** `hello`→Accepted, `xMehRx`→Rejected, `abc mehr def`→Rejected, `me hr`→Accepted, `MEHR`→Rejected.

**What changed from `6-substr.l`**

| Original | Compact |
|---|---|
| `int yywrap() { return 1; }` | `%option noyywrap` |
| actions `{ printf("Rejected\n"); }` | `puts("Rejected")` |
| blank lines | removed. The three rules are exactly the same. |

**Keep this:** longest match wins, ties go to the first rule, so the "mehr" rule has to come first.

## Q7 Variable

**Picture.** Lex turns `letter then letters/digits` into one VARIABLE token. Yacc accepts a line only if it is exactly one VARIABLE followed by a newline.

`7.l` lines 1–9
```lex
 1  %{
 2  // Q7 (lex part).  Build: lex 7.l && yacc -d 7.y && gcc lex.yy.c y.tab.c -o 7
 3  #include "y.tab.h"
 4  %}
 5  %option noyywrap
 6  %%
 7  [a-zA-Z][a-zA-Z0-9]*   return VARIABLE;
 8  [ \t]                  ;
 9  \n|.                   return yytext[0];
```
- L3: include `y.tab.h` to get the name `VARIABLE`.
- L7: a letter followed by any letters or digits → return the VARIABLE token.
- L8: skip spaces and tabs.
- L9: any other character, including newline, is returned as itself. So `1` comes out as the character `'1'`, which the grammar does not expect → error.

`7.y` lines 1–11
```yacc
 1  %{
 2  // Q7 (yacc part): valid variable = letter followed by letters/digits.
 3  #include <stdio.h>
 4  int yylex(); void yyerror(const char *s) { puts("Result: Invalid Variable"); }
 5  %}
 6  %token VARIABLE
 7  %%
 8  input: | input VARIABLE '\n'  { puts("Result: Valid Variable"); }
 9         | input '\n' ;
10  %%
11  int main() { printf("Enter a variable name: "); yyparse(); }
```
- L4: `yylex` is declared. `yyerror` runs on any syntax error and prints Invalid.
- L6: declare the token VARIABLE.
- L8: `input` is a list of lines. Empty, or more lines. One line = `VARIABLE '\n'`, and its action prints Valid.
- L9: a blank line is also allowed.
- L11: `main` prints the prompt and calls `yyparse()`.

**Example I ran:** `abc1` → Valid Variable. `1abc` → Invalid Variable. After an error yacc stops (as in your original), so later lines are not read.

**What changed from `7-variable.l/.y`**

| Original | Compact |
|---|---|
| grammar `input: … \| input line;  line: '\n' \| VARIABLE '\n'` | `line` folded into `input`. Same language. |
| lexer: `\n { return '\n'; }` and `. { return yytext[0]; }` | one rule `\n\|.` |
| `yywrap` function | `%option noyywrap` |
| separate prototype lines for `yyerror` and `yylex` | one line |

**Keep this:** lex finds the word, yacc checks the shape of the line.

## Q8 Calculator

**Picture.** Lex turns text into NUMBER tokens and operator characters. Yacc grammar `expr` builds the value as it matches, using `$$ = $1 + $3` style actions.

`8.l` lines 1–10
```lex
 1  %{
 2  // Q8 (lex part).  Build: lex 8.l && yacc -d 8.y && gcc lex.yy.c y.tab.c -o 8
 3  #include <stdlib.h>
 4  #include "y.tab.h"
 5  %}
 6  %option noyywrap
 7  %%
 8  [0-9]+     { yylval = atoi(yytext); return NUMBER; }
 9  [ \t]      ;
10  \n|.       return yytext[0];
```
- L3: `stdlib.h` is needed for `atoi` (text → int).
- L8: digits → store the number in `yylval` (the token's value), return NUMBER.
- L9: skip blanks.
- L10: any other character (`+ - * / ( )` and newline) is returned as itself.

`8.y` lines 6–9
```yacc
 6  %token NUMBER
 7  %left '+' '-'
 8  %left '*' '/'
 9  %right UMINUS
```
- L7–9: precedence. **Lines further down bind tighter.** `+ -` lowest, `* /` higher, `UMINUS` (unary minus) highest. `%left` = left to right, so `8-3-2` is `(8-3)-2`.

`8.y` lines 11–21
```yacc
11  calc: | calc expr '\n'  { printf("Result: %d\n\n", $2); }
12        | calc '\n' ;
13  expr: NUMBER
14      | expr '+' expr  { $$ = $1 + $3; }
15      | expr '-' expr  { $$ = $1 - $3; }
16      | expr '*' expr  { $$ = $1 * $3; }
17      | expr '/' expr  { if ($3) $$ = $1 / $3; else { yyerror("Division by zero"); $$ = 0; } }
18      | '(' expr ')'   { $$ = $2; }
19      | '-' expr %prec UMINUS  { $$ = -$2; } ;
20  %%
21  int main() { puts("Enter expressions to calculate:"); yyparse(); }
```
- L11–12: `calc` is a list of lines. A line `expr '\n'` prints `$2`, the value of `expr`. A blank line is fine.
- L13: `NUMBER` has no action. Yacc's default `$$ = $1` copies the value.
- L14–16: in `expr '+' expr`, `$1` = left value, `$2` = the `+`, `$3` = right value. So `$$ = $1 + $3`.
- L17: division. If `$3` is not 0 divide, else call `yyerror` and give 0.
- L18: parentheses: value is whatever is inside (`$2`).
- L19: `'-' expr %prec UMINUS` is unary minus. `%prec UMINUS` gives that rule the highest precedence.
- L21: `main` prints a prompt and calls `yyparse()`.

**Example I ran:** `2+3*4`=14, `(2+3)*4`=20, `-5+2`=-3, `7/2`=3 (integer division), `10/0` → `Error: Division by zero`, result 0.

**What changed from `8-calc.l/.y`**

| Original | Compact |
|---|---|
| `line: '\n' \| expr '\n' {…}` under `calculation` | folded into `calc` |
| `NUMBER { $$ = $1; }` | no action (default does it) |
| division written as a multi-line if/else block | one line (L17) |
| lexer: `[-+*/()] { return yytext[0]; }` and `. { yyerror("Unknown character"); }` | `\n\|.` returns the character. **An unknown character like `$` is now a yacc "syntax error"** instead of your own "Unknown character" message. |
| `extern int yylval; extern void yyerror…` and `stdlib.h` in the .y file | dropped. `stdlib.h` moved into the .l file where `atoi` is used. |

**Keep this:** `$$ = $1 op $3`; the `%left` lines (lowest first) decide `*` before `+`.

## Q9 AST

**Picture.** The grammar is layered, so `*` and `/` (inside T) are grouped before `+` and `-` (inside E):
`S → id = E`, `E → E+T | E-T | T`, `T → T*F | T/F | F`, `F → (E) | id | num`.
Each time a rule matches, build a tree node. For `x = a + b * c`:
```
      =
     / \
    x   +
       / \
      a   *
         / \
        b   c
```
Post-order (left, right, then the node itself) prints `x a b c * + =`.

`9.l` lines 8–12
```lex
 8  [a-zA-Z][a-zA-Z0-9]*   { yylval.s = strdup(yytext); return ID; }
 9  [0-9]+                 { yylval.s = strdup(yytext); return NUM; }
10  [ \t]                  ;
11  \n                     return 0;          /* end of input */
12  .                      return yytext[0];
```
- L8–9: identifiers and numbers: `strdup(yytext)` makes a copy of the text (yytext gets overwritten by the next match), kept in `yylval.s`. Return ID or NUM.
- L11: newline returns 0. Yacc reads 0 as "end of input", so it finishes after one line.
- L12: any other character is returned as itself.

`9.y` lines 7–14
```yacc
 7  typedef struct node { char *t; struct node *l, *r; } node;
 8  node *mk(char *t, node *l, node *r) { node *n = malloc(sizeof *n); n->t = strdup(t); n->l = l; n->r = r; return n; }
 9  void post(node *n) { if (!n) return; post(n->l); post(n->r); printf("%s ", n->t); }   // left, right, root
10  int yylex(); void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
11  %}
12  %union { char *s; struct node *n; }
13  %token <s> ID NUM
14  %type <n> E T F
```
- L7: a node holds the token text `t` and pointers `l`, `r` to children.
- L8: `mk(text, left, right)` allocates a node, copies the text, sets children, returns the pointer.
- L9: `post(n)` = post-order: left subtree, right subtree, then print this node. `if (!n) return` stops at an empty child.
- L12: `%union` = each token or rule value is either a string `s` or a node pointer `n`. It says `struct node *` (not `node`) because the lexer also reads `y.tab.h` and does not know the typedef.
- L13: ID and NUM carry a string (`<s>`). L14: E, T, F carry a node pointer (`<n>`).

`9.y` lines 16–25
```yacc
16  S: ID '=' E  { printf("Post-order Traversal of AST: "); post(mk("=", mk($1, 0, 0), $3)); puts(""); } ;
17  E: E '+' T   { $$ = mk("+", $1, $3); }
18   | E '-' T   { $$ = mk("-", $1, $3); }
19   | T ;
20  T: T '*' F   { $$ = mk("*", $1, $3); }
21   | T '/' F   { $$ = mk("/", $1, $3); }
22   | F ;
23  F: '(' E ')' { $$ = $2; }
24   | ID        { $$ = mk($1, 0, 0); }
25   | NUM       { $$ = mk($1, 0, 0); } ;
```
- L16: for `id = E`: `$1` = the id text, `$3` = the tree built for E. Make a leaf for the id, make an `=` node with the leaf on the left and `$3` on the right, print it in post-order.
- L17–18, L20–21: make an operator node with the two sub-trees as children.
- L19, L22: `| T` and `| F` have no action. Default `$$ = $1` passes the tree up.
- L23: parentheses add no node: pass the inner tree up (`$2`).
- L24–25: an id or number becomes a leaf node.

**Examples I ran:** `x = a + b * c` → `x a b c * + =`. `y = (a+b)*c-4/d` → `y a b + c * 4 d / - =`.

**What changed from `9-ast.l/.y`**

| Original | Compact |
|---|---|
| struct fields `token`, `left`, `right`; functions `mknode`, `print_postorder` | `t`, `l`, `r`; `mk`, `post` |
| `print_postorder` checked `if (tree->left)` before each recursive call | one `if (!n) return` at the top |
| function prototypes before the grammar | dropped. Functions are defined before they are used. |
| `%union { char *sval; struct node *nval; }` | `{ char *s; struct node *n; }` |
| `%type <nval> S E T F` | `<n> E T F` (S returns no value) |
| `%left '+' '-'` and `%left '*' '/'` | dropped. The layered grammar already has no ambiguity. |
| temp variables `id_node`, `root`; banner line `--- Abstract Syntax Tree Generated Successfully ---` | inlined; banner removed. The tree output is identical. |

**Keep this:** each rule builds a node from its parts; layering E/T/F gives `*` priority; post-order prints the tree.

## Q10 FOR syntax

**Picture.** Lex turns the text into tokens. Yacc only checks the order: `for ( init ; cond ; inc ) body`, where init, cond and inc may each be empty.

`10.l` lines 7–14
```lex
 7  "for"                      return FOR;
 8  [a-zA-Z_][a-zA-Z0-9_]*     return ID;
 9  [0-9]+                     return NUM;
10  "<="|">="|"=="|"!="|"<"|">"  return RELOP;
11  "++"                       return INC;
12  "--"                       return DEC;
13  [ \t\n]                    ;
14  .                          return yytext[0];
```
- L7: `"for"` is listed before the identifier rule. Both match 3 letters, a tie, so the earlier rule wins and `for` is a keyword.
- L8–9: identifiers and numbers.
- L10: all six comparison operators return one token, RELOP. Lex takes the longest match, so `<=` is read as one token and not as `<` then `=`.
- L11–12: `++` is INC, `--` is DEC.
- L13: ignore spaces, tabs, newlines.
- L14: any other character (`( ) ; { } = + - * /`) is returned as itself.

`10.y` lines 6–20
```yacc
 6  %token FOR ID NUM RELOP INC DEC
 7  %left '+' '-'
 8  %left '*' '/'
 9  %%
10  prog: stmt  { puts("\nResult: Valid FOR statement syntax"); } ;
11  stmt: FOR '(' init ';' cond ';' inc ')' body ;
12  init: | ID '=' expr ;
13  cond: | expr RELOP expr | expr ;
14  inc:  | ID '=' expr | ID INC | ID DEC | INC ID | DEC ID ;
15  expr: ID | NUM | expr '+' expr | expr '-' expr | expr '*' expr | expr '/' expr | '(' expr ')' ;
16  body: ';' | '{' list '}' | one ;
17  list: | list one ;
18  one:  ID '=' expr ';' | stmt ;
19  %%
20  int main() { puts("Enter a C 'for' statement to check syntax:"); yyparse(); }
```
- L6: declared tokens. L7–8: `%left` lines remove an ambiguity in `expr` (without them `a+b+c` could be grouped two ways, and yacc complains).
- L10: `prog` is one `stmt`; if it matches, print Valid.
- L11: `for ( init ; cond ; inc ) body`.
- L12: init is empty or `i = expr`.
- L13: cond is empty, `expr RELOP expr`, or just `expr`.
- L14: inc is empty, `i = expr`, `i++`, `i--`, `++i` or `--i`.
- L15: `expr` is an id, number, two exprs joined by an operator, or `( expr )`.
- L16: body is `;` (empty), `{ list }`, or one statement.
- L17: `list` is zero or more statements.
- L18: a statement is `id = expr ;` or another for (nested loops).
- L4: on a syntax error `yyerror` prints Invalid with yacc's message.

**Examples I ran:**

| Input | Result |
|---|---|
| `for(i=0;i<10;i++){ a=b+1; }` | Valid |
| `for(i=0;i<10;i++) a=b+1;` | Valid |
| `for(;;);` | Valid |
| `for(i=0;i<10;i++` | Invalid |
| `for(i=0;i<10;i++){ a=b+1 }` (missing `;`) | Invalid |
| `for i=0;i<10;i++ a=1;` (no brackets) | Invalid |

**What changed from `10-for_check.l/.y`**

| Original | Compact |
|---|---|
| tokens `LE GE EQ NE` and a `relop` rule with `'<' \| '>' \| LE \| GE \| EQ \| NE` | lexer returns one RELOP for all six; the `relop` rule is gone |
| rule names `program`, `for_stmt`, `initialization`, `condition`, `increment`, `statement_list`, `statement` | `prog`, `stmt`, `init`, `cond`, `inc`, `list`, `one` |
| no precedence lines. **Bison reported 16 shift/reduce conflicts** on `expr` (it still ran by choosing shift). | `%left` lines added. **0 conflicts.** |
| `yyerror` printed to `stderr` | prints to normal output, so the message shows next to the result |
| `#include <string.h>` and `yywrap` function | dropped / `%option noyywrap` |

**Keep this:** same for-grammar; empty init/cond/inc is allowed; nested for is a statement.

## What I ran to check

- Q1–Q5: compact and original gave byte-identical output on my test inputs. Q2, Q3, Q4 also matched on 300 random inputs each.
- Q6–Q10: same accept/reject or same results as your originals. Q10 was tested on 7 for-statements, valid and broken.
- Built with `flex` and `bison -y` here. Your lab machine's `lex` and `yacc` should behave the same.
- Everything in the "what changed" tables is from reading your original files side by side with the compact ones.
