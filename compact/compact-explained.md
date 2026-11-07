# 🔍 Compact Q1–Q10: Line-by-Line Explanation

### Companion to `compact-1-10.md` (the code) | Every line explained, with what changed from your originals

---

## 📌 HOW TO USE THIS GUIDE

For each program you get:

1. **The big idea** in a few lines, so you know what the code is trying to do
2. **The code in small parts**, each followed by a line-by-line explanation (`L7` = line 7 of that file)
3. **A dry run** with real output from the program
4. **What changed** from your original file
5. **Common confusions** and a **self-test**

> **Tip:** read the big idea, guess how you would write it, then read the lines.

## Contents

- [Section 0: the C tricks](#section-0)
- [Section 0b: Lex and Yacc in 8 lines](#section-0b)
- [Input formats at a glance](#input-formats)
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
- [One-page revision](#revision)
- [What I ran to check](#checks)

---

<a id="section-0"></a>
# 🧱 SECTION 0: THE C TRICKS YOU NEED

The compact code is short because it uses a few C shortcuts. Learn them once.

## 0.1 A word is an array of letters ending in `'\0'`

```c
char st[10][20];     // 10 names, each up to 19 letters
```
- `st[2]` is the third name. `'\0'` (the number 0) marks where a word ends.
- `scanf("%s", st[i])` reads **one word** (it stops at a space). That is why you type `q0 q1 q2` with spaces.

## 0.2 Comparing words: `strcmp`

- `strcmp(a, b)` returns **0 when the words are equal**.
- So `!strcmp(a, b)` reads as "a equals b". And `strcmp(a, b) ? x : y` gives `x` when they are **different**.

## 0.3 Short forms

| Shortcut | Meaning |
|---|---|
| `cond ? a : b` | if `cond` is true use `a`, else `b` |
| `while (k--)` | runs exactly `k` times (it tests `k`, then lowers it) |
| globals | global arrays and numbers **start at 0**, so no clearing loop is needed |
| `memset(v, 0, sizeof v)` | set the whole array `v` to 0 |
| `puts("x")` | `printf("x\n")` |
| `k++ ? ", %s" : "%s"` | print a comma before every item **except the first** |

## 0.4 Bitmasks (Q3)

A set of small numbers fits in one `int`. Bit j is 1 when j is in the set.

| Want | Write | Example |
|---|---|---|
| number with only bit j on | `1 << j` | `1 << 3` = 8 |
| add j to a set | `s \|= 1 << j` | `{2}` is 4, add 3 → 12 |
| is j in the set? | `s >> j & 1` | `12 >> 3 & 1` = 1 |
| union of two sets | `a \| b` | `4 \| 8` = 12 |
| do two sets overlap? | `a & b` | `12 & 16` = 0 |

## 0.5 Retry on a bad input line

```c
if (x < 0 || y < 0) { puts("..."); i--; continue; }
```
`continue` jumps to the next pass of the `for`, which does `i++`. The `i--` cancels that, so the same slot is asked again.

## 0.6 Push a character back (Q5)

`ungetc(c, fp)` puts the character `c` back into the file, so the next `fgetc` returns it again.

---

<a id="section-0b"></a>
# 🧱 SECTION 0b: LEX AND YACC IN 8 LINES

- **Lex** cuts text into tokens. Each rule is `pattern  action`.
- **Yacc** checks that the tokens follow a grammar.
- `yyparse()` asks `yylex()` for the next token again and again.
- `return NAME;` hands the parser a token. A single character like `'+'` is returned as itself (`return yytext[0];`).
- `yytext` = the text the pattern just matched. `yylval` = a value that travels with the token.
- `yacc -d` writes `y.tab.h`, which holds the token names so the lexer can use them.
- In a Yacc rule, `$$` = the value of the left side. `$1`, `$2`, `$3` = the values of the right-side items in order.
- Lex picks the **longest** match. On a tie, the **earlier** rule wins.

Build every lex+yacc pair like this: `lex N.l && yacc -d N.y && gcc lex.yy.c y.tab.c -o N`

---

<a id="input-formats"></a>
# 📥 INPUT FORMATS AT A GLANCE

| Program | What you type | Notes |
|---|---|---|
| Q1 | `n`, state names, `m`, alphabet names, `t`, then `t` lines `source input destination` | `e` = ε. Non-ε lines are read and ignored. |
| Q2 | same as Q1, then a count of accepting states, then their names | The first state listed is the start state. `m` does not count `e`. |
| Q3 | same as Q2, but no `e` lines | The first state listed is the start state. |
| Q4 | `n`, `m`, alphabet letters, the table of next-states (one row per state), accepting numbers ending in -1 | Unchanged. Rows are state numbers, not names. |
| Q5 | reads the file `5-input.txt` | Run it in the folder that holds the file. |
| Q6–Q10 | type lines on the keyboard | Q9 reads one line and stops. Q7 and Q10 stop at the first error. |

---

<a id="q1"></a>
# 📘 Q1: ε-closure of every state

**Files:** `1.c` | **Build:** `gcc 1.c -o 1`

## 🎯 The big idea

ε edges are **free roads**: you may walk them without reading any input. The **ε-closure** of a state is every state you can reach using only those free roads (the state itself included).

**DFS** (depth-first search) = walk as far as you can, mark every place you visit, never visit a place twice. Start DFS from a state, follow only ε edges, and everything marked is its closure.

## Part 1: Setup: variables and two small functions

`1.c` lines 1–9
```c
 1  // Q1 (named input, same style as 1-e-closure-1.c): epsilon-closure of every state.
 2  // Build: gcc 1.c -o 1      Input: states, alphabets, transitions "source input destination" (e = epsilon)
 3  #include <stdio.h>
 4  #include <string.h>
 5  int n, m, t, e[10][10], v[10];                // e = epsilon matrix, v = visited
 6  char st[10][20], al[10][20], a[20], s[20], b[20];
 7  int id(char nm[][20], int k, char *w) { for (int i = 0; i < k; i++) if (!strcmp(nm[i], w)) return i; return -1; }
 8  void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }
 9  
```

- L1–2: comments. They say what the program does, how to build it, and the input shape (`source input destination`, `e` = ε).
- L3: `stdio.h` gives `printf`, `scanf`, `puts`.
- L4: `string.h` gives `strcmp` (compare words) and `memset` (fill memory).
- L5: `n` = number of states. `m` = number of alphabets. `t` = number of transitions. `e[i][j]` = 1 means an ε edge from state i to state j. `v[s]` = 1 means DFS has visited state s. Globals start at 0.
- L6: `st[10][20]` = names of up to 10 states (each name up to 19 letters). `al` = names of the alphabets. `a`, `s`, `b` = three temporary words used to read one transition: source, symbol, destination.
- L7: `id(nm, k, w)` finds the **number** of the name `w` in a list `nm` that holds `k` names. It checks each one with `strcmp`, which returns **0 when the words are equal**, so `!strcmp(...)` means \"equal\". It returns the index `i`, or `-1` if the name is not in the list.
- L8: `dfs(s)` is the closure walk. It marks `s` visited, then for every state `j`: if there is an ε edge `s→j` and `j` is not visited yet, it calls `dfs(j)`. Marking first is what stops an endless loop when ε edges form a cycle.

## Part 2: Reading the machine: states and alphabets

`1.c` lines 10–19
```c
10  int main() {
11      int i, j, x, y;
12      printf("Enter number of states: "); scanf("%d", &n);
13      printf("Enter the states:\n");
14      for (i = 0; i < n; i++) scanf("%s", st[i]);
15      printf("Enter number of alphabets: "); scanf("%d", &m);
16      printf("Enter the alphabets:\n");
17      for (i = 0; i < m; i++) scanf("%s", al[i]);
18      printf("Enter number of transitions: "); scanf("%d", &t);
19      printf("\nEnter transitions in the form:\nsource input destination\nUse 'e' for epsilon.\n\n");
```

- L10: `main` starts here.
- L11: `i`, `j` = loop counters. `x`, `y` = state numbers for one transition.
- L12: ask for and read the number of states into `n`.
- L13–14: read `n` state names. `%s` reads **one word**, so you type `q0 q1 q2 q3` separated by spaces. `st[i]` is the name of state i, so q0 gets number 0, q1 gets 1, and so on.
- L15: read the number of alphabets into `m`.
- L16–17: read the `m` alphabet names into `al`. This program does not need them for the closure, but it reads them to keep your input style.
- L18: read how many transitions you will type, into `t`.
- L19: print the help text: the form is `source input destination`, and `e` means ε.

## Part 3: Reading the transitions

`1.c` lines 20–25
```c
20      for (i = 0; i < t; i++) {
21          scanf("%s %s %s", a, s, b);
22          x = id(st, n, a); y = id(st, n, b);
23          if (x < 0 || y < 0) { puts("Invalid state. Enter this transition again."); i--; continue; }
24          if (!strcmp(s, "e")) e[x][y] = 1;     // only epsilon edges matter for the closure
25      }
```

- L20: repeat `t` times, one pass per transition.
- L21: read three words: source name into `a`, symbol into `s`, destination name into `b`. Example: `q0 e q1`.
- L22: turn the names into numbers: `x` = number of the source, `y` = number of the destination.
- L23: if either name was not in the state list (`id` gave -1), print a message, do `i--` to undo this pass's count, and `continue` to the next pass. Net effect: you are asked for this transition again.
- L24: if the symbol is exactly `e` (`!strcmp(s, \"e\")` = equal), set `e[x][y] = 1`. Any other symbol is ignored, because the closure only uses ε edges.
- L25: end of the `for`.

## Part 4: Printing every closure

`1.c` lines 26–33
```c
26      printf("\n--- Epsilon Closures ---\n");
27      for (i = 0; i < n; i++) {
28          memset(v, 0, sizeof v); dfs(i);
29          printf("Epsilon-Closure(%s) = { ", st[i]);
30          for (j = 0; j < n; j++) if (v[j]) printf("%s ", st[j]);
31          puts("}");
32      }
33  }
```

- L26: print the heading.
- L27: take each state `i` as the start state.
- L28: `memset(v, 0, sizeof v)` sets the whole `v` array back to 0 (nothing visited). Then `dfs(i)` marks every state reachable from i by ε edges, i itself included.
- L29: print `Epsilon-Closure(` and the name of state i.
- L30: for every state `j`, if it was marked, print its name. The names come out in state order, not in the order DFS visited them.
- L31: `puts(\"}\")` prints the closing brace and a newline.
- L32–33: end of the `for` and of `main`. `main` returns 0 on its own.

## 🧪 Dry run

**Your screenshot input** (4 states, alphabets `0 1`, 12 transitions):
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
Only the lines with `e` matter: **q0→q0, q0→q1, q0→q3, q3→q2**. The rest are ignored.

| Start | DFS walk | Closure printed |
|---|---|---|
| q0 | q0 → q1 → q3 → q2 (DFS visit order) | q0 q1 q2 q3 |
| q1 | no ε edge out of q1 | q1 |
| q2 | no ε edge out of q2 | q2 |
| q3 | q3 → q2 | q2 q3 |

The closure is printed in **state order**, not in visit order.

**Output:**
```
--- Epsilon Closures ---
Epsilon-Closure(q0) = { q0 q1 q2 q3 }
Epsilon-Closure(q1) = { q1 }
Epsilon-Closure(q2) = { q2 }
Epsilon-Closure(q3) = { q2 q3 }
```

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| `#define MAX_STATES`, `MAX_ALPHABETS`, `MAX_TRANSITIONS` | dropped. Sizes are written as `10` and `20` in the arrays |
| names `epsilon`, `visited`, `findState` | `e`, `v`, `id` |
| `findState` and `findAlphabet` (two copies of the same search) | one `id()` that takes the list as an argument. Your `findAlphabet` was never used for the answer, so I dropped it. Alphabets are still read. |
| loop that sets every `epsilon[i][j] = 0` | dropped. Global arrays already start at 0 |
| `for (j...) visited[j] = 0;` | `memset(v, 0, sizeof v)` |
| prompts and output text | **exactly the same**. On 300 random inputs the full output matched your `1-e-closure-1.c`, prompts included |

## ❓ Common confusions

- **Why does q0's closure include q0?** Every state reaches itself for free (zero steps). `dfs(i)` marks `i` first, so it is always in its own closure.
- **Why are non-ε transitions read but ignored?** The closure uses ε edges only. The program still has to read those lines so the input stays in step.
- **Why `i--` and `continue` on a bad name?** `continue` jumps to the next pass, and the `for` adds 1 to `i`. The `i--` cancels that, so the same slot is asked again.

## 🧠 Self-test

Write `id()` first (loop, `strcmp`, return index or -1). Then `dfs()` (mark, loop j, edge and not visited, recurse). Then the reading loop that sets `e[x][y]` only when the symbol is `e`.

**Keep this:** DFS from each state over ε edges only; whatever gets marked is the closure.

---

<a id="q2"></a>
# 📘 Q2: NFA with ε → NFA without ε

**Files:** `2.c` | **Build:** `gcc 2.c -o 2`

## 🎯 The big idea

To remove ε edges, let every state **absorb** what it can do through ε. From state q on symbol `a`: slide along ε for free, take one real `a` edge, then slide along ε again.

**new δ(q, a) = closure( δ( closure(q), a ) )**

(δ = "where do I go". closure = the Q1 result.) A state also becomes **accepting** if its closure contains an accepting state.

## Part 1: Setup

`2.c` lines 1–11
```c
 1  // Q2 (named input): NFA with epsilon -> NFA without epsilon.   Build: gcc 2.c -o 2
 2  // Input: states, alphabets (not counting e), transitions "source input destination" (e = epsilon),
 3  //        then the accepting states.  The first state listed is the start state.
 4  #include <stdio.h>
 5  #include <string.h>
 6  int n, m, a, i, j, k, q, p, t, u, t0, x, y;
 7  char st[10][20], al[10][20], s1[20], s2[20], s3[20];
 8  int c[10][10], d[10][10][10], nt[10][10][10], f[10], nf[10];   // c = eps edges, d = old delta, nt = new delta
 9  int v[10], cl[10][10];                                          // v = visited, cl = closure table
10  int id(char nm[][20], int cnt, char *w) { for (int z = 0; z < cnt; z++) if (!strcmp(nm[z], w)) return z; return -1; }
11  void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (c[s][j] && !v[j]) dfs(j); }
```

- L1–3: comments: what it does, how to build, and the input shape. The **first state you list is the start state**.
- L4–5: `stdio.h` for printing and reading, `string.h` for `strcmp` and `memset`.
- L6: `n` = number of states. `m` = number of alphabets (not counting `e`). `a` = an alphabet number. `i j k q p t u` = loop counters. `t0` = transitions still to read. `x`, `y` = state numbers.
- L7: `st` = state names, `al` = alphabet names, `s1 s2 s3` = three temporary words for one transition.
- L8: `c[i][j]` = 1 if there is an ε edge i→j. `d[i][j][a]` = 1 if state i goes to state j on alphabet a (the **old** δ). `nt` = the **new** δ being built. `f[i]` = 1 if i is accepting. `nf[i]` = 1 if i is accepting in the new machine.
- L9: `v` = DFS visited list. `cl[i][j]` = 1 if state j is in the closure of state i.
- L10: `id()`, same as Q1: the number of a name in a list, or -1.
- L11: `dfs(s)`, same as Q1, but it walks the ε edges stored in `c`.

## Part 2: Reading the header

`2.c` lines 13–21
```c
13  int main() {
14      printf("Enter number of states: "); scanf("%d", &n);
15      printf("Enter the states:\n");
16      for (i = 0; i < n; i++) scanf("%s", st[i]);
17      printf("Enter number of alphabets (excluding e): "); scanf("%d", &m);
18      printf("Enter the alphabets:\n");
19      for (i = 0; i < m; i++) scanf("%s", al[i]);
20      printf("Enter number of transitions: "); scanf("%d", &t0);
21      printf("\nEnter transitions in the form:\nsource input destination\nUse 'e' for epsilon.\n\n");
```

- L13: `main` starts.
- L14: read the number of states.
- L15–16: read the state names.
- L17: read the number of alphabets. Do not count `e`.
- L18–19: read the alphabet names.
- L20: read how many transitions you will type, into `t0`.
- L21: print the help text.

## Part 3: Reading transitions and accepting states

`2.c` lines 22–32
```c
22      while (t0 > 0) {
23          scanf("%s %s %s", s1, s2, s3);
24          x = id(st, n, s1); y = id(st, n, s3);
25          a = strcmp(s2, "e") ? id(al, m, s2) : -2;                  // -2 = epsilon, -1 = unknown symbol
26          if (x < 0 || y < 0 || a == -1) { puts("Invalid state or alphabet. Enter this transition again."); continue; }
27          if (a == -2) c[x][y] = 1; else d[x][y][a] = 1;
28          t0--;
29      }
30      printf("Enter number of accepting states: "); scanf("%d", &k);
31      printf("Enter the accepting states:\n");
32      while (k--) { scanf("%s", s1); if ((x = id(st, n, s1)) >= 0) f[x] = 1; }
```

- L22: keep going while transitions are still owed. `t0` goes down only for a **good** transition.
- L23: read source, symbol, destination as three words.
- L24: convert the source and destination names to numbers `x` and `y`.
- L25: decide what the symbol is. If it is not `e`, `a` = its alphabet number (or -1 if it is not in the list). If it is `e`, `a` = -2. The `? :` is a short if/else: `condition ? value_if_true : value_if_false`. `strcmp(s2, \"e\")` is non-zero when the word is **not** `e`.
- L26: if a state name is unknown (`x` or `y` is -1) or the symbol is unknown (`a == -1`), print a message and `continue`. `t0` was not lowered, so you are asked again.
- L27: if `a == -2`, mark an ε edge `c[x][y] = 1`. Otherwise mark a normal edge `d[x][y][a] = 1`.
- L28: one good transition done, so `t0--`.
- L29: end of the `while`.
- L30: read how many accepting states into `k`.
- L31: print the prompt.
- L32: `while (k--)` runs exactly `k` times. Each time: read a state name into `s1`, find its number, and if it is valid (`>= 0`) set `f[x] = 1`. The `(x = id(...)) >= 0` assigns and tests in one go.

## Part 4: The conversion

`2.c` lines 34–42
```c
34      for (i = 0; i < n; i++) {                                       // closure of each state by DFS
35          memset(v, 0, sizeof v); dfs(i);
36          for (j = 0; j < n; j++) cl[i][j] = v[j];
37      }
38      // new delta(q,a) = closure( delta( closure(q), a ) )
39      for (q = 0; q < n; q++) for (a = 0; a < m; a++) for (p = 0; p < n; p++) if (cl[q][p])
40          for (t = 0; t < n; t++) if (d[p][t][a]) for (u = 0; u < n; u++) if (cl[t][u]) nt[q][u][a] = 1;
41      // q is accepting if its closure contains an accepting state
42      for (q = 0; q < n; q++) for (p = 0; p < n; p++) if (cl[q][p] && f[p]) nf[q] = 1;
```

- L34–37: **closure of every state by DFS.** For each state `i`: clear `v`, run `dfs(i)`, then copy the marked row into `cl[i]`. Now `cl` holds all closures. This is the same idea as Q1, but the answer is stored.
- L38: comment with the formula.
- L39–40: the formula as loops. `q` = start state, `a` = alphabet, `p` = a state in closure(q) (the free ε slide), `t` = a state that `p` reaches on `a` (one real step), `u` = a state in closure(t) (the second free slide). For each such `u`, set `nt[q][u][a] = 1`. Line 39 ends with `if (cl[q][p])` and line 40 carries on inside it.
- L41: comment.
- L42: for each state `q`, look at every `p` in its closure. If `p` is accepting, `q` becomes accepting: `nf[q] = 1`. This also covers q itself, because q is in its own closure.

## Part 5: Printing the new NFA

`2.c` lines 44–55
```c
44      printf("\nNFA without ε transitions:\nStates: ");
45      for (i = 0; i < n; i++) printf("%s ", st[i]);
46      printf("\nTransitions:\n");
47      for (q = 0; q < n; q++) for (a = 0; a < m; a++) {
48          printf("δ(%s, %s) = { ", st[q], al[a]); k = 0;
49          for (t = 0; t < n; t++) if (nt[q][t][a]) printf(k++ ? ", %s" : "%s", st[t]);
50          printf(k ? " }\n" : "∅ }\n");
51      }
52      printf("Accepting states: "); k = 0;
53      for (i = 0; i < n; i++) if (nf[i]) printf(k++ ? ", %s" : "%s", st[i]);
54      printf(k ? "\n" : "∅\n");
55  }
```

- L44: print the heading and `States:`. The `ε` and `∅` symbols are plain UTF-8 text.
- L45: print every state name.
- L46: print `Transitions:`.
- L47: for every state `q` and alphabet `a`.
- L48: print `δ(state, alphabet) = {` and reset `k = 0`. `k` counts how many names have been printed.
- L49: for every state `t` with `nt[q][t][a] = 1`, print its name. `k++ ? \", %s\" : \"%s\"` prints a comma first for every name **except the first** (`k++` is 0 the first time, which is false).
- L50: if nothing was printed (`k` is 0) print `∅ }`, else print ` }`.
- L51: end of the `for`.
- L52: print `Accepting states:` and reset `k`.
- L53: print the names with `nf[i] = 1`, with the same comma trick.
- L54: if none, print `∅`; otherwise just a newline.
- L55: end of `main`.

## 🧪 Dry run

**Example A** (the study guide's machine; language a⁺b). Input:
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
Step 1: the closures (found with the same DFS as Q1):
```
--- Epsilon Closures ---
Epsilon-Closure(q0) = { q0 q1 q2 }
Epsilon-Closure(q1) = { q1 q2 }
Epsilon-Closure(q2) = { q2 }
Epsilon-Closure(q3) = { q3 }
Epsilon-Closure(q4) = { q4 }
```
Step 2: check `δ(q0, a)` by hand. closure(q0) = {q0, q1, q2}. On `a`, only q2 has moves: q2→q2 and q2→q3. closure(q2) = {q2} and closure(q3) = {q3}. So `δ(q0, a) = { q2, q3 }`.

Step 3: `δ(q3, b)`: closure(q3) = {q3}; on `b` q3→q4; closure(q4) = {q4}. So `{ q4 }`.

**Output:**
```
NFA without ε transitions:
States: q0 q1 q2 q3 q4 
Transitions:
δ(q0, a) = { q2, q3 }
δ(q0, b) = { ∅ }
δ(q1, a) = { q2, q3 }
δ(q1, b) = { ∅ }
δ(q2, a) = { q2, q3 }
δ(q2, b) = { ∅ }
δ(q3, a) = { ∅ }
δ(q3, b) = { q4 }
δ(q4, a) = { ∅ }
δ(q4, b) = { ∅ }
Accepting states: q4
```
`q0`, `q1`, `q2` now behave the same. That is a hint of what Q4 would merge.

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| input: `eps` matrix and one 0/1 matrix **per symbol** | named states and `source input destination` lines, same style as your `1-e-closure-1.c` |
| `dfs(int state, int visited[])` and a `computeClosure()` function | global `v` and the closure loop written inline (L34–37). It is still DFS. |
| names `eps`, `trans`, `newTrans`, `accepting`, `newAccepting`, `symbols` | `c`, `d`, `nt`, `f`, `nf`, `al` |
| loops that set `newTrans` and `accepting` to 0 | dropped. Global arrays start at 0 |
| accepting states typed as numbers ending with -1, with a range check | typed as **names** after a count. A wrong name is skipped (`>= 0` test), so there is no out-of-range write. |
| `newAccepting[q] = accepting[q]` first, then a loop with `break` | `nf[q] = 1` if any state in the closure is accepting (L42). Same result, because q is in its own closure. |
| `first` flag with `if (!first) printf(\", \")` | the `k++ ? \", %s\" : \"%s\"` trick |
| output format | same lines, but with state **names** instead of `q0`, `q1` numbers |

## ❓ Common confusions

- **Why is `e` not allowed as an alphabet name?** `e` is how you type ε. A symbol called `e` would be read as ε.
- **Why `closure(q)` first, then the move, then closure again?** From q you may slide for free before the real step, and slide for free after it. Leaving out either slide loses transitions.
- **Why is the first state the start state?** The program has no separate \"start\" prompt. State number 0 is the first name you typed.

## 🧠 Self-test

Say the formula out loud, then write the loops in order: p in closure(q), t from `d`, u in closure(t). Then write the accepting rule: some p in closure(q) is accepting.

**Keep this:** closure(q) → move on `a` → closure again; accepting if the closure touches an accepting state.

---

<a id="q3"></a>
# 📘 Q3: NFA → DFA (subset construction)

**Files:** `3.c` | **Build:** `gcc 3.c -o 3`

## 🎯 The big idea

One DFA state = a **group** of NFA states you could be in at the same time. Start with the group {first state}. For each group and each alphabet, find where **everyone** in the group can go and take the union. If that union is a new group, it becomes a new DFA state. Repeat until no new group appears.

**Bitmask trick.** A group is stored in **one `int`**. Bit j of the number is 1 when state j is in the group. Example: {q2, q3} = bits 2 and 3 = 4 + 8 = **12**.

## Part 1: Setup

`3.c` lines 1–10
```c
 1  // Q3 (named input): NFA -> DFA (subset construction).   Build: gcc 3.c -o 3
 2  // Input: states, alphabets, transitions "source input destination", then the accepting states.
 3  // The first state listed is the start state.  A set of NFA states is a bitmask: bit j set <=> state j is in it.
 4  #include <stdio.h>
 5  #include <string.h>
 6  int n, m, i, j, k, x, y, fin, t0;             // fin = mask of NFA accepting states
 7  char st[10][20], al[10][20], s1[20], s2[20], s3[20];
 8  int tr[10][10];                               // tr[i][k] = mask of states reached from state i on alphabet k
 9  int dm[1024], dt[1024][10], cnt = 1;          // dm[s] = NFA-set of DFA state s, dt = DFA transitions
10  int id(char nm[][20], int c, char *w) { for (int z = 0; z < c; z++) if (!strcmp(nm[z], w)) return z; return -1; }
```

- L1–3: comments. L3 explains the bitmask idea: bit j set means state j is in the set.
- L4–5: `stdio.h` and `string.h` (for `strcmp`).
- L6: `n` states, `m` alphabets. `i j k` = loop counters. `x y` = state numbers. `fin` = a bitmask of the NFA's accepting states. `t0` = transitions left to read.
- L7: names of states and alphabets, plus three temporary words.
- L8: `tr[i][k]` = a bitmask of **all states reachable from state i on alphabet k**. One int replaces a whole row of 0/1 entries.
- L9: `dm[s]` = the NFA-group (bitmask) that DFA state s stands for. `dt[s][k]` = the DFA transition from state s on alphabet k. `cnt` = number of DFA states found so far. It starts at 1 because DFA state 0 exists from the start.
- L10: `id()`, as before (its list-size parameter is called `c` here).

## Part 2: Reading the machine

`3.c` lines 12–30
```c
12  int main() {
13      printf("Enter number of states: "); scanf("%d", &n);
14      printf("Enter the states:\n");
15      for (i = 0; i < n; i++) scanf("%s", st[i]);
16      printf("Enter number of alphabets: "); scanf("%d", &m);
17      printf("Enter the alphabets:\n");
18      for (i = 0; i < m; i++) scanf("%s", al[i]);
19      printf("Enter number of transitions: "); scanf("%d", &t0);
20      printf("\nEnter transitions in the form:\nsource input destination\n\n");
21      while (t0 > 0) {
22          scanf("%s %s %s", s1, s2, s3);
23          x = id(st, n, s1); y = id(st, n, s3); k = id(al, m, s2);
24          if (x < 0 || y < 0 || k < 0) { puts("Invalid state or alphabet. Enter this transition again."); continue; }
25          tr[x][k] |= 1 << y;                   // set bit y in the move-set of (x, k)
26          t0--;
27      }
28      printf("Enter number of accepting states: "); scanf("%d", &k);
29      printf("Enter the accepting states:\n");
30      while (k--) { scanf("%s", s1); if ((x = id(st, n, s1)) >= 0) fin |= 1 << x; }
```

- L12: `main` starts. L13–18: read the number of states and their names, then the number of alphabets and their names, as in Q1.
- L19: read how many transitions will be typed, into `t0`.
- L20: print the help text. There is no ε in this program.
- L21: keep reading while transitions are owed.
- L22: read source, symbol, destination words.
- L23: convert them to numbers: `x` = source, `y` = destination, `k` = alphabet.
- L24: if any name is unknown (-1), print a message and `continue` without lowering `t0`, so you are asked again.
- L25: `1 << y` is the number 1 shifted left y places, which is a number with only bit y on. `|=` turns that bit on in `tr[x][k]`. So `tr[x][k]` now includes state y.
- L26: one good transition done: `t0--`.
- L27: end of the `while`.
- L28: read how many accepting states into `k`.
- L29: print the prompt.
- L30: `while (k--)` runs `k` times. Read a name, find its number into `x`, and if valid turn on bit x in `fin`.

## Part 3: The subset construction

`3.c` lines 32–40
```c
32      dm[0] = 1;                                // DFA state 0 = { first state }
33      for (int cur = 0; cur < cnt; cur++)       // cnt grows as new subsets are found (acts as a queue)
34          for (k = 0; k < m; k++) {
35              int s = 0;
36              for (i = 0; i < n; i++) if (dm[cur] >> i & 1) s |= tr[i][k];   // union of moves
37              for (j = 0; j < cnt && dm[j] != s; j++);                       // already known?
38              if (j == cnt) dm[cnt++] = s;
39              dt[cur][k] = j;
40          }
```

- L32: DFA state 0 is the group {first state} = only bit 0 on = the number 1.
- L33: `cur` walks over DFA states in order. The loop test `cur < cnt` is checked every time, and `cnt` can grow inside the loop. New states therefore get their turn too. This is the \"worklist\" (a queue, like BFS).
- L34: for each alphabet `k`.
- L35: `s` = the new group, starting empty (0).
- L36: for every NFA state `i`: if bit i is on in the current group (`dm[cur] >> i & 1`), add everything `i` reaches on `k` with `|` (union).
- L37: search the groups found so far for one equal to `s`. The loop has an empty body (`;`). It stops at the first `j` where `dm[j] == s`, or runs to `j == cnt` if there is none.
- L38: if `j == cnt`, nothing matched, so `s` is new: store it as DFA state `cnt` and increase `cnt`.
- L39: record that DFA state `cur` on alphabet `k` goes to state `j`.
- L40: end of the `for k`.

## Part 4: Printing the DFA

`3.c` lines 42–53
```c
42      printf("\n--- DFA RESULTS ---\nTotal DFA States: %d\n\nDFA States mapping:\n", cnt);
43      for (i = 0; i < cnt; i++) {
44          printf("q%d = { ", i);
45          for (j = 0; j < n; j++) if (dm[i] >> j & 1) printf("%s ", st[j]);
46          printf(dm[i] ? "}\n" : "∅ }\n");
47      }
48      printf("\nTransitions:\n");
49      for (i = 0; i < cnt; i++) for (k = 0; k < m; k++) printf("δ(q%d, %s) = q%d\n", i, al[k], dt[i][k]);
50      printf("\nAccepting states: ");
51      for (i = 0; i < cnt; i++) if (dm[i] & fin) printf("q%d ", i);
52      puts("");
53  }
```

- L42: print the heading and the number of DFA states.
- L43–47: for each DFA state print its number and its group. L45 prints the name of each NFA state whose bit is on. L46 prints `}` or `∅ }` when the group is empty (`dm[i]` is 0).
- L48: print `Transitions:`.
- L49: for every DFA state `i` and alphabet `k` print `δ(q i, alphabet) = q dt[i][k]`. DFA states are numbered `q0, q1, ...` in the order they were found.
- L50: print `Accepting states:`.
- L51: a DFA state is accepting if its group shares a bit with `fin`: `dm[i] & fin` is not 0.
- L52: `puts(\"\")` prints a newline.
- L53: end of `main`.

## 🧪 Dry run

This is the ε-free NFA from Q2 (Example A). Input:
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
After reading, `tr` holds these bitmasks (all other entries are 0), and `fin` = 16 (only q4 accepts):

| tr entry | Value | Meaning |
|---|---|---|
| q0 on a | 12 | bits 2 and 3 → {q2, q3} |
| q1 on a | 12 | {q2, q3} |
| q2 on a | 12 | {q2, q3} |
| q3 on b | 16 | bit 4 → {q4} |

Now the worklist. A group is shown as its number, then in binary (bit 4 on the left, bit 0 on the right):

| DFA state | Group `dm[cur]` | Alphabet | Union `s` | Goes to | Found |
|---|---|---|---|---|---|
| q0 | 1 (00001) | a | 12 | q1 | new |
| q0 | 1 (00001) | b | 0 | q2 | new |
| q1 | 12 (01100) | a | 12 | q1 | known |
| q1 | 12 (01100) | b | 16 | q3 | new |
| q2 | 0 (00000) | a | 0 | q2 | known |
| q2 | 0 (00000) | b | 0 | q2 | known |
| q3 | 16 (10000) | a | 0 | q2 | known |
| q3 | 16 (10000) | b | 0 | q2 | known |

Final: `dm = [1, 12, 0, 16]`, so the groups are {q0}, {q2,q3}, {} and {q4}. Only `dm[3] & fin` is non-zero, so DFA state 3 accepts.

**Output:**
```
--- DFA RESULTS ---
Total DFA States: 4

DFA States mapping:
q0 = { q0 }
q1 = { q2 q3 }
q2 = { ∅ }
q3 = { q4 }

Transitions:
δ(q0, a) = q1
δ(q0, b) = q2
δ(q1, a) = q1
δ(q1, b) = q3
δ(q2, a) = q2
δ(q2, b) = q2
δ(q3, a) = q2
δ(q3, b) = q2

Accepting states: q3
```

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| `dfaStates[1<<MAX][MAX]`: a 0/1 array for every group | `dm[1024]`: one int per group |
| `isSameState()` and `findDFAState()` (two functions, two loops) | one `for` with `dm[j] != s` (L37) |
| `startState[]` array set to {q0} | `dm[0] = 1` |
| `dfaAccept[]` array filled inside the main loop | not stored. Checked when printing with `dm[i] & fin` |
| 3-D array `trans[i][j][k]` | 2-D `tr[i][k]` holding a bitmask |
| input: 0/1 matrix per symbol, accepting numbers ending in -1 | named states, `source input destination` lines, accepting states as names after a count |
| output | same layout. Group members are printed as state **names**. DFA states are still `q0, q1, ...` |

## ❓ Common confusions

- **Why can a DFA state look like `{ ∅ }`?** It is the empty group: no NFA state is reachable. It is the \"dead\" state and it loops to itself.
- **Why do DFA names `q0, q1..` clash with NFA names?** They are separate numberings. `q1 = { q2 q3 }` means **DFA state 1** is the group of NFA states q2 and q3.
- **Why is the start group only the first state?** This NFA has no ε edges (Q2 removed them), so the start group is just the first state.

## 🧠 Self-test

Write the bit tricks first: turn a bit on `|= 1 << j`, test a bit `>> j & 1`. Then the worklist: `for (cur = 0; cur < cnt; cur++)`, union the moves, search, add if new, record.

**Keep this:** A DFA state is a set of NFA states; take the union of the moves for each alphabet; a new set becomes a new state.

---

<a id="q4"></a>
# 📘 Q4: Minimize a DFA

**Files:** `4.c` | **Build:** `gcc 4.c -o 4`

## 🎯 The big idea

Two states are **different** if some input makes one end in an accepting state and the other not. Start: accepting vs non-accepting are different. Then: if on some alphabet two states land on a pair that is already known to be different, they are different too. Repeat until nothing changes. Pairs never marked are the **same**, so merge them. This is called table-filling.

## Part 1: Setup

`4.c` lines 1–6
```c
 1  // Q4: minimize a DFA (table-filling / marking distinguishable pairs).  Build: gcc 4.c -o 4
 2  #include <stdio.h>
 3  int n, m, i, j, k, ch, cc;
 4  int d[10][10], f[10], x[10][10], cls[10];     // x[i][j] = 1 if states i and j are distinguishable
 5  char sym[10];
 6  int rep(int c) { int r = 0; while (cls[r] != c) r++; return r; }   // first state of class c
```

- L1: comment: what it does and how to build.
- L2: `stdio.h` for printing and reading.
- L3: `n` = number of states. `m` = number of alphabets. `i j k` = loop counters. `ch` = \"something changed this round\". `cc` = number of merged states.
- L4: `d[s][k]` = the state reached from state s on alphabet k (the DFA table). `f[s]` = 1 if s is accepting. `x[i][j]` = 1 means states i and j are **different**. `cls[s]` = the number of the merged state that s belongs to.
- L5: the alphabet letters, one char each.
- L6: `rep(c)` finds the **first old state that belongs to merged state c** (its representative). It scans `r` upward until `cls[r] == c`.

## Part 2: Reading the DFA

`4.c` lines 8–16
```c
 8  int main() {
 9      printf("Enter number of states: "); scanf("%d", &n);
10      printf("Enter number of input symbols: "); scanf("%d", &m);
11      printf("Enter symbols (space separated): ");
12      for (i = 0; i < m; i++) scanf(" %c", &sym[i]);
13      printf("Enter transition table (row = state, col = symbol):\n");
14      for (i = 0; i < n; i++) for (j = 0; j < m; j++) scanf("%d", &d[i][j]);
15      printf("Enter accepting states (enter -1 to stop): ");
16      while (scanf("%d", &i) && i != -1) f[i] = 1;
```

- L8: `main` starts.
- L9–10: read the number of states and the number of alphabets.
- L11–12: read the alphabet letters. The space in `\" %c\"` skips whitespace before each letter.
- L13–14: read the table: row = state, column = alphabet. Each number is the next state. Example row `1 2` means: on the first alphabet go to q1, on the second go to q2.
- L15–16: read accepting state numbers until you type -1. `scanf` returns 1 when it read a number, so the loop continues while that is true and the number is not -1. Each number sets `f[i] = 1`.

## Part 3: Marking the different pairs

`4.c` lines 18–23
```c
18      for (i = 0; i < n; i++) for (j = 0; j < n; j++) x[i][j] = f[i] != f[j];   // accepting vs non-accepting
19      do {                                                                       // repeat until no new pair is marked
20          ch = 0;
21          for (i = 0; i < n; i++) for (j = i + 1; j < n; j++) if (!x[i][j])
22              for (k = 0; k < m; k++) if (x[d[i][k]][d[j][k]]) { x[i][j] = x[j][i] = ch = 1; break; }
23      } while (ch);
```

- L18: first marks. `f[i] != f[j]` is 1 when exactly one of the two states is accepting. That is the base case: those pairs are different.
- L19: start the repeat loop (`do ... while`).
- L20: assume nothing changes this round: `ch = 0`.
- L21: look at every pair `i < j` that is not marked yet (`!x[i][j]`).
- L22: for each alphabet `k`, look at where the two states go: `d[i][k]` and `d[j][k]`. If that pair is already marked, mark `(i, j)` too (both `x[i][j]` and `x[j][i]`), set `ch = 1`, and `break` out of the alphabet loop for this pair.
- L23: if anything changed (`ch`), run another round. A new mark can unlock other pairs, so we repeat until a full round changes nothing.

## Part 4: Grouping the same states

`4.c` lines 25–26
```c
25      for (i = 0; i < n; i++) cls[i] = -1;                                       // unmarked pairs = same class
26      for (i = 0; i < n; i++) if (cls[i] < 0) { for (j = i; j < n; j++) if (!x[i][j]) cls[j] = cc; cc++; }
```

- L25: nobody has a merged number yet: set every `cls[i] = -1`.
- L26: for each state `i` with no number yet: give `i` the number `cc`, and give the same number to every `j >= i` that is **not marked different** from `i`. Then `cc++` for the next group. At the end `cc` is the number of merged states.

## Part 5: Printing the minimized DFA

`4.c` lines 28–33
```c
28      printf("\n--- Minimized DFA ---\nTotal States: %d\n\nTransitions:\n", cc);
29      for (i = 0; i < cc; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], cls[d[rep(i)][k]]);
30      printf("\nAccepting states: ");
31      for (i = 0; i < cc; i++) if (f[rep(i)]) printf("q%d ", i);
32      printf("\nStart state: q%d\n", cls[0]);
33  }
```

- L28: print the heading and the number of merged states.
- L29: for each merged state `i` and alphabet `k`: take its representative `rep(i)`, see where it goes (`d[rep(i)][k]`), then convert that old state to its merged number with `cls[...]`.
- L30–31: a merged state is accepting if its representative is accepting.
- L32: the start state is the merged number of old state 0: `cls[0]`.
- L33: end of `main`.

## 🧪 Dry run

A DFA with 6 states, alphabets `0 1`, accepting q3, q4, q5. Input:
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
Table: q0→(q1,q2), q1→(q3,q4), q2→(q3,q4), q3→(q5,q5), q4→(q5,q5), q5→(q5,q5).

- **Base case (L18):** accepting vs non-accepting is different. That marks 9 pairs: each of q0, q1, q2 against each of q3, q4, q5.
- **Round 1:** (q0,q1): on `0`, q0→q1 and q1→q3, and (q1,q3) is marked, so mark (q0,q1). (q0,q2) is marked the same way. **(q1,q2) stays unmarked:** both go to q3 on `0` and to q4 on `1`, and (q3,q3), (q4,q4) are never different. Among q3, q4, q5 every pair goes to (q5,q5), so nothing marks them.
- **Round 2:** nothing new, so `ch` stays 0 and the loop ends.
- **Unmarked pairs:** (q1,q2), (q3,q4), (q3,q5), (q4,q5). So the groups are {q0}, {q1,q2}, {q3,q4,q5}: **3 states**.

**Output:**
```
--- Minimized DFA ---
Total States: 3

Transitions:
δ(q0, 0) = q1
δ(q0, 1) = q1
δ(q1, 0) = q2
δ(q1, 1) = q2
δ(q2, 0) = q2
δ(q2, 1) = q2

Accepting states: q2 
Start state: q0
```

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| names `delta`, `accepting`, `dist`, `class`, `classCount`, `changed` | `d`, `f`, `x`, `cls`, `cc`, `ch` |
| `int rep = 0; while (class[rep] != i) rep++;` written twice | one function `rep()` (L6) |
| nested `if` and braces around the marking loops | same logic with the braces removed (L21–22) |
| accepting states typed with `while (scanf(...) && i != -1)` | unchanged |
| input and output text | unchanged. Q4 still takes a table of next-states |

## ❓ Common confusions

- **Why only `j > i` in the marking loop?** The table is symmetric. Pair (1,2) and pair (2,1) are the same pair, so one half is enough. The code still sets both halves.
- **Why repeat the rounds?** A pair can only be marked once the pair it depends on is marked. Dependencies can chain, so one pass is not always enough.
- **Does this remove unreachable states?** No. Your original did not either. It only merges states that behave the same.

## 🧠 Self-test

Write the base case (`f[i] != f[j]`), then the repeat loop (mark when the next-state pair is marked), then the grouping loop (`!x[i][j]` gets the same number).

**Keep this:** Mark different pairs, spread the marks until stable, merge the unmarked pairs.

---

<a id="q5"></a>
# 📘 Q5: Lexical analyzer

**Files:** `5.c` | **Build:** `gcc 5.c -o 5`

## 🎯 The big idea

Read one character. It decides what kind of token starts here. Keep reading while the characters still fit that kind. The first character that does **not** fit is **pushed back** so the next token can use it.

## Part 1: Setup and keyword check

`5.c` lines 1–6
```c
 1  // Q5: lexical analyzer (skips spaces/tabs/newlines).  Reads 5-input.txt.  Build: gcc 5.c -o 5
 2  #include <stdio.h>
 3  #include <ctype.h>
 4  #include <string.h>
 5  char *kw[] = {"int","float","char","double","if","else","while","for","return","void","break","continue","do","switch","case","default"};
 6  int isKw(char *s) { for (int i = 0; i < 16; i++) if (!strcmp(s, kw[i])) return 1; return 0; }
```

- L1: comment: what it does, that it reads `5-input.txt`, and how to build.
- L2: `stdio.h` for files and printing. L3: `ctype.h` for `isalpha`, `isdigit`, `isalnum`, `isspace`. L4: `string.h` for `strcmp` and `strchr`.
- L5: `kw` = an array of the 16 keyword strings.
- L6: `isKw(s)` compares the word `s` with each of the 16 keywords using `strcmp` (0 means equal). It returns 1 if one matches, else 0. If you add a keyword, change the `16`.

## Part 2: Opening the file and skipping blanks

`5.c` lines 8–13
```c
 8  int main() {
 9      FILE *fp = fopen("5-input.txt", "r");
10      if (!fp) { puts("File not found"); return 1; }
11      int c, i; char t[100];
12      while ((c = fgetc(fp)) != EOF) {
13          if (isspace(c)) continue;                                        // ignore blanks
```

- L8: `main` starts.
- L9: open `5-input.txt` for reading. `fp` is the file handle.
- L10: if the file could not be opened (`fp` is NULL), print a message and stop with `return 1` (non-zero = error).
- L11: `c` = the current character. It is an **`int`** so it can hold `EOF` (end of file, which is -1). `i` = position in the token. `t` = the token text being built.
- L12: read characters one by one until end of file.
- L13: `isspace(c)` is true for space, tab and newline (and a few more). Skip them with `continue`. This is the \"ignore redundant spaces, tabs and newlines\" part of the question.

## Part 3: Identifiers, keywords and numbers

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
- L15: copy characters into `t` while they are letters, digits or `_`. The `for` reads the next character at the end of each pass. It stops on the first character that does not fit; that character is left in `c`.
- L16: end the word with 0 (`t[i] = 0`). `ungetc(c, fp)` puts the extra character back into the file so it will be read again. Without this, the character after the word would be lost.
- L17: print the word and `Keyword` or `Identifier` depending on `isKw`.
- L18: otherwise, if the character is a digit, a number starts.
- L19–20: same pattern with `isdigit`: copy digits, end the string, push back the extra character.
- L21: print the number and `Number`.
- L22: end of the number branch.

## Part 4: Operators, symbols and everything else

`5.c` lines 23–27
```c
23          else if (strchr("+-*/=<>%", c)) printf("%c : Operator\n", c);
24          else if (strchr(";,(){}[]", c)) printf("%c : Special Symbol\n", c);
25          else printf("%c : Unknown\n", c);
26      }
27  }
```

- L23: `strchr(\"+-*/=<>%\", c)` is non-zero when `c` is one of those characters. Then it is an operator.
- L24: the same test for the special symbols `; , ( ) { } [ ]`.
- L25: anything else is printed as `Unknown`.
- L26: end of the `while` loop.
- L27: end of `main`.

## 🧪 Dry run

Input file `5-input.txt`:
```
int a=25;
float num=100;
if(a>10)
{
    a=a+1;
}
```
Walk through `a=25;`: `a` is a letter, so collect `a`. The next character `=` does not fit, so it is pushed back. Print `a : Identifier`. Then `=` is an operator. Then `2` is a digit: collect `25`, push back `;`, print `25 : Number`. Then `;` is a special symbol.

**Output (first 8 lines of 24):**
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

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| `char ch;` | `int c;`. `EOF` is -1. Where `char` is unsigned, `ch != EOF` can never be false and the loop would not end. |
| `fseek(fp, -1, SEEK_CUR)` guarded by `if (ch != EOF)` | `ungetc(c, fp)`. It does nothing at EOF, so no guard is needed. |
| `isKeyword` with a local array and a `sizeof` count | global `kw[]` and a fixed 16 |
| `ch == ' ' \|\| ch == '\t' \|\| ch == '\n'` | `isspace(c)`. It also skips `\r`. |
| long `\|\|` chains for operators and symbols | `strchr(\"...\", c)` |
| file missing: `return 0` | `return 1` |
| `fclose(fp)` at the end | dropped. The program ends right after, and the system closes the file. |

## ❓ Common confusions

- **Why push the extra character back?** To know a word has ended you must read one character past it. That character may start the next token (like `=` after `a`). `ungetc` gives it back.
- **Why is `c` an `int` and not a `char`?** `EOF` is -1. A `char` cannot reliably hold it.
- **Why check `isalpha` before `isdigit`?** A token that starts with a letter is a name even if digits follow (`a1`). A token that starts with a digit is a number.

## 🧠 Self-test

Write the outer loop with `isspace`. Then one branch: collect while the characters fit, end with 0, `ungetc`, print. Copy the same shape for digits.

**Keep this:** The first character picks the token type; read while it fits; push back the one that does not.

---

<a id="q6"></a>
# 📘 Q6: Lex: reject strings containing "mehr"

**Files:** `6.l` | **Build:** `lex 6.l && gcc lex.yy.c -o 6`

## 🎯 The big idea

Lex reads a line and tries every rule. Rule 1 matches any line that **contains** m-e-h-r in any capitals. Rule 2 matches any line at all. Both can match the whole line, so the **tie** is broken by order: the **first** rule wins. That is why the Rejected rule is written first.

## Part 1: The top of the file

`6.l` lines 1–7
```lex
 1  %{
 2  // Q6: reject strings containing the first four letters of the name ("mehr", any case).
 3  // Build: lex 6.l && gcc lex.yy.c -o 6
 4  #include <stdio.h>
 5  %}
 6  %option noyywrap
 7  %%
```

- L1: `%{` starts a block of plain C code that Lex copies into the program.
- L2–3: comments: the task, and the build command.
- L4: `stdio.h` for `puts`.
- L5: `%}` ends the C block.
- L6: `%option noyywrap` tells Lex \"stop at the end of the input\". It replaces writing `int yywrap() { return 1; }`.
- L7: `%%` separates the sections. The rules start after it.

## Part 2: The rules and main

`6.l` lines 8–12
```lex
 8  .*[mM][eE][hH][rR].*   puts("Rejected");     /* tie in length with next rule -> first rule wins */
 9  .*                     puts("Accepted");
10  \n                     ;
11  %%
12  int main() { puts("Enter String:"); yylex(); }
```

- L8: **Rule 1.** `.*` = any characters (`.` is any character except newline, `*` is zero or more). `[mM][eE][hH][rR]` = m, e, h, r in that order, each in either case, **touching each other**. Then `.*` again. So: any line that contains \"mehr\". The action prints `Rejected`.
- L9: **Rule 2.** `.*` = any line. The action prints `Accepted`.
- L10: **Rule 3.** `\n` with an empty action `;`. It swallows the newline so nothing extra is printed.
- L11: `%%` ends the rules. The C code after it is copied as is.
- L12: `main` prints the prompt and calls `yylex()`, which keeps matching rules until the input ends.
- **Why Rule 1 wins when \"mehr\" is present:** both rules match the whole line, so they tie on length. Lex picks the **earlier** rule. When \"mehr\" is absent, Rule 1 does not match at all, so Rule 2 runs.
- **Why a blank line prints nothing:** on an empty line Rule 2 matches 0 characters but Rule 3 matches 1 (`\n`). Lex takes the **longest** match, so Rule 3 runs.

## 🧪 Dry run

| Input line | Contains m-e-h-r touching (any case)? | Output |
|---|---|---|
| I love compiler design | no | (no output) |
| Mehrin wrote this lab | yes | (no output) |
| xMEHRx | yes | (no output) |
| comeHeRe now | no | (no output) |
| me hr | no | (no output) |

A blank line prints nothing (Rule 3 swallows it).

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| `int yywrap() { return 1; }` | `%option noyywrap` |
| actions `{ printf(\"Rejected\\n\"); }` | `puts(\"Rejected\")` |
| `main` with `return 0` | no `return 0` |
| the three rules | exactly the same, in the same order |

## ❓ Common confusions

- **Why does `comeHeRe` not match?** The four letters must touch. The letters in `comeHeRe` are c-o-m-e-H-e-R-e. After `m-e` comes `H-e`, not `h-r`. So there is no contiguous m-e-h-r.
- **What if I swap Rule 1 and Rule 2?** Everything would print Accepted. Rule 2 would always win the tie.

## 🧠 Self-test

Write the two rules in the right order and say why. Then the empty-action newline rule.

**Keep this:** Longest match wins; ties go to the first rule, so the \"mehr\" rule must come first.

---

<a id="q7"></a>
# 📘 Q7: YACC: valid variable name

**Files:** `7.l`, `7.y` | **Build:** `lex 7.l && yacc -d 7.y && gcc lex.yy.c y.tab.c -o 7`

## 🎯 The big idea

Lex turns \"a letter followed by letters or digits\" into one **VARIABLE** token. Yacc then accepts a line only if it is exactly one VARIABLE followed by a newline. Anything else (like a name that starts with a digit) arrives as a stray character, and Yacc reports an error.

## Part 1: 7.l: the lexer

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

- L1–5: the C block. It includes `y.tab.h`, the file Yacc makes. It holds the token name `VARIABLE`, so the lexer can return it. `%option noyywrap` stops Lex at end of input.
- L6: `%%` starts the rules.
- L7: `[a-zA-Z][a-zA-Z0-9]*` = one letter, then any number of letters or digits. The action `return VARIABLE;` hands that token to the parser.
- L8: `[ \t]` = a space or tab. Empty action, so blanks are skipped.
- L9: `\n|.` means newline **or** any other single character. The action `return yytext[0];` returns that character itself (`yytext` is the text just matched). So a newline comes back as `'\n'`, and a stray `1` comes back as `'1'`.

## Part 2: 7.y: the grammar

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

- L1–5: the C block. L3 includes `stdio.h`. L4 declares `yylex` and defines `yyerror`, which Yacc calls on a syntax error. Here it prints `Result: Invalid Variable`.
- L6: `%token VARIABLE` declares the token name.
- L7: `%%` starts the grammar rules.
- L8: `input:` is the start rule. It is **empty** (the first `|` has nothing before it), or `input VARIABLE '\n'`, which means \"more input, then a variable, then a newline\". The action prints `Result: Valid Variable`.
- L9: or `input '\n'`, a blank line. It is accepted silently.
- L10: `%%` ends the rules.
- L11: `main` prints the prompt and calls `yyparse()`, which asks `yylex()` for tokens and checks them against the grammar.

## 🧪 Dry run

| Input | Output |
|---|---|
| abc1 | Result: Valid Variable |
| 1abc | Result: Invalid Variable |
| x | Result: Valid Variable |
| ab_c | Result: Invalid Variable |

After the first error the parser stops, so each input above was run on its own.

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| two rules `input` and `line` | folded into one rule `input` (same language) |
| lexer rules `\n { return '\n'; }` and `. { return yytext[0]; }` | one rule `\n\|.` |
| `int yywrap() { return 1; }` | `%option noyywrap` |
| separate prototype lines for `yyerror` and `yylex` | one line |

## ❓ Common confusions

- **Why does `123abc` give Invalid?** The lexer cannot start a VARIABLE with `1`, so it returns the character `'1'`. The grammar has no rule that starts with `'1'`, so Yacc calls `yyerror`.
- **Why does `abc1` work but `ab_c` not?** `_` is not in `[a-zA-Z0-9]`. The lexer returns VARIABLE for `ab`, then the stray `_`, which the grammar rejects.
- **Why does it stop after an error?** There is no error-recovery rule, so `yyparse` gives up after the first error.

## 🧠 Self-test

Write the lexer rule for the word, then the grammar `input: | input VARIABLE '\n' | input '\n';`.

**Keep this:** Lex finds the word; Yacc checks the shape of the line.

---

<a id="q8"></a>
# 📘 Q8: Calculator with LEX and YACC

**Files:** `8.l`, `8.y` | **Build:** `lex 8.l && yacc -d 8.y && gcc lex.yy.c y.tab.c -o 8`

## 🎯 The big idea

Lex turns text into NUMBER tokens (with the number as a value) and operator characters. Yacc grammar `expr` computes the value as it matches, with actions like `$$ = $1 + $3`. The `%left` lines decide that `*` goes before `+`.

## Part 1: 8.l: the lexer

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

- L1–5: the C block. `stdlib.h` is for `atoi` (text to int). `y.tab.h` holds the token name `NUMBER`.
- L6: `%option noyywrap` stops Lex at end of input.
- L7: `%%` starts the rules.
- L8: one or more digits: `yylval = atoi(yytext)` stores the **number** in `yylval` (the value that travels with the token), then `return NUMBER`. `yylval` is declared by Yacc as an `int`, which is all this program needs.
- L9: skip spaces and tabs.
- L10: `\n|.` returns any other character as itself: `+ - * / ( )` and the newline.

## Part 2: 8.y: the grammar and the actions

`8.y` lines 1–21
```yacc
 1  %{
 2  // Q8 (yacc part): calculator.  + - * / ( ) and unary minus.
 3  #include <stdio.h>
 4  int yylex(); void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
 5  %}
 6  %token NUMBER
 7  %left '+' '-'
 8  %left '*' '/'
 9  %right UMINUS
10  %%
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

- L1–5: the C block. `yyerror` prints `Error: ...` to the error stream.
- L6: `%token NUMBER` declares the token.
- L7–9: **precedence.** Lines further down bind **tighter**. `+ -` are lowest, `* /` higher, `UMINUS` (unary minus) highest. `%left` means left to right, so `8-3-2` is `(8-3)-2`. `%right` means right to left.
- L10: `%%` starts the rules.
- L11: `calc` is the start rule. It is empty, or `calc expr '\n'`, which means \"more input, an expression, a newline\". The action prints the value: `$2` is the value of `expr` (the second item).
- L12: or `calc '\n'`: a blank line is fine.
- L13: `expr: NUMBER` has no action. Yacc's default `$$ = $1` copies the number's value up.
- L14–16: `expr '+' expr`: `$1` = the left value, `$2` = the `+` itself, `$3` = the right value. So `$$ = $1 + $3` is the result for the whole thing. `-` and `*` work the same way.
- L17: division. If `$3` is not 0, divide. Otherwise call `yyerror` and give 0. The check is in the action because `5/0` is a perfectly **valid shape**; only its value is a problem.
- L18: `( expr )`: the value is whatever is inside, `$2`.
- L19: `'-' expr %prec UMINUS` is unary minus. `%prec UMINUS` gives this rule the highest precedence. Without it the rule would use the precedence of `-`, which is low, and `-2*3` could parse wrongly.
- L20: `%%` ends the rules.
- L21: `main` prints the prompt and calls `yyparse()`.

## 🧪 Dry run

| Input | Output |
|---|---|
| 3 + 4 * 2 | Result: 11 |
| (2+3)*4 | Result: 20 |
| -5+2 | Result: -3 |
| 7/2 | Result: 3 |
| 10/0 | Error: Division by zero  then  Result: 0 |

`10/0` prints the error message first (to the error stream), then a result of 0.

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| `line: '\n' \| expr '\n' {...}` under `calculation` | folded into `calc` |
| `NUMBER { $$ = $1; }` | no action (the default does the same) |
| division written as a multi-line if/else block | one line (L17) |
| lexer: `[-+*/()] { return yytext[0]; }` and `. { yyerror(\"Unknown character\"); }` | `\n\|.` returns the character. **An unknown character like `$` is now Yacc's `syntax error`** instead of your own \"Unknown character\" message. |
| `extern int yylval;` and `extern void yyerror...` in the `.l` file | dropped. They were not needed there. |

## ❓ Common confusions

- **Why integer answers like `7/2 = 3`?** The values are C `int`s, so division drops the fraction.
- **Why is `-5+2` equal to -3 and not -7?** Unary minus has the highest precedence, so it is `(-5)+2`.
- **Where does `$$` go?** It is the value of the rule's left side. The rule above it can read it as its `$1`, `$2` or `$3`.

## 🧠 Self-test

Write the precedence block from memory (lowest first). Then one rule: `expr '+' expr { $$ = $1 + $3; }`. Then the `%prec UMINUS` rule.

**Keep this:** `$$ = $1 op $3`; the `%left` lines (lowest first) make `*` happen before `+`.

---

<a id="q9"></a>
# 📘 Q9: BNF → YACC: build an AST

**Files:** `9.l`, `9.y` | **Build:** `lex 9.l && yacc -d 9.y && gcc lex.yy.c y.tab.c -o 9`

## 🎯 The big idea

The grammar is **layered**, so `*` and `/` (inside T) group before `+` and `-` (inside E):

`S → id = E`, `E → E+T | E-T | T`, `T → T*F | T/F | F`, `F → (E) | id | num`

Each time a rule matches, build a **tree node**. Names and numbers become leaves. Each operator becomes a parent of the two trees it joins. Finally print the tree in **post-order**: left, right, then the node itself.

## Part 1: 9.l: the lexer

`9.l` lines 1–12
```lex
 1  %{
 2  // Q9 (lex part).  Build: lex 9.l && yacc -d 9.y && gcc lex.yy.c y.tab.c -o 9
 3  #include <string.h>
 4  #include "y.tab.h"
 5  %}
 6  %option noyywrap
 7  %%
 8  [a-zA-Z][a-zA-Z0-9]*   { yylval.s = strdup(yytext); return ID; }
 9  [0-9]+                 { yylval.s = strdup(yytext); return NUM; }
10  [ \t]                  ;
11  \n                     return 0;          /* end of input */
12  .                      return yytext[0];
```

- L1–5: the C block. `string.h` is for `strdup`. `y.tab.h` holds `ID` and `NUM`.
- L6: `%option noyywrap` stops Lex at end of input.
- L7: `%%` starts the rules.
- L8: a name: `strdup(yytext)` makes a **copy** of the text (the original `yytext` is overwritten by the next match). The copy goes into `yylval.s`, the string field of the value union. Return `ID`.
- L9: a number: same, but return `NUM`.
- L10: skip spaces and tabs.
- L11: a newline returns 0. Yacc reads 0 as \"end of input\", so it stops after one line.
- L12: any other character is returned as itself.

## Part 2: 9.y: the tree tools and declarations

`9.y` lines 1–14
```yacc
 1  %{
 2  // Q9 (yacc part): BNF -> YACC, builds an AST and prints it in post-order.
 3  //   S -> id = E     E -> E+T | E-T | T     T -> T*F | T/F | F     F -> (E) | id | num
 4  #include <stdio.h>
 5  #include <stdlib.h>
 6  #include <string.h>
 7  typedef struct node { char *t; struct node *l, *r; } node;
 8  node *mk(char *t, node *l, node *r) { node *n = malloc(sizeof *n); n->t = strdup(t); n->l = l; n->r = r; return n; }
 9  void post(node *n) { if (!n) return; post(n->l); post(n->r); printf("%s ", n->t); }   // left, right, root
10  int yylex(); void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
11  %}
12  %union { char *s; struct node *n; }
13  %token <s> ID NUM
14  %type <n> E T F
```

- L1–3: comments, including the grammar.
- L4–6: `stdio.h` (printing), `stdlib.h` (`malloc`), `string.h` (`strdup`).
- L7: a `node` holds the token text `t` and pointers `l`, `r` to its left and right children. A leaf has both `NULL`.
- L8: `mk(t, l, r)` builds a node: `malloc` gets memory, `strdup` copies the text, the children are set, and the pointer is returned.
- L9: `post(n)` prints in post-order. `if (!n) return` stops at an empty child. Then: print the left tree, the right tree, then this node's text.
- L10: declares `yylex` and defines `yyerror`.
- L11: `%}` ends the C block.
- L12: `%union` says every value is **either** a string `s` **or** a node pointer `n`. It says `struct node *` because the lexer also reads this and does not know the `typedef`.
- L13: `%token <s> ID NUM`: the tokens `ID` and `NUM` carry the string field `s`.
- L14: `%type <n> E T F`: the rules `E`, `T`, `F` carry the node field `n`.

## Part 3: 9.y: the rules

`9.y` lines 15–27
```yacc
15  %%
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
26  %%
27  int main() { puts("Enter assignment expression (e.g., x = a + b * c):"); yyparse(); }
```

- L15: `%%` starts the rules.
- L16: `S: ID '=' E`: `$1` = the name, `$3` = the finished tree for E. The action prints a label, makes a leaf for the name, makes an `=` node with that leaf on the left and `$3` on the right, prints it with `post`, then a newline.
- L17–18: `E '+' T` and `E '-' T`: make a `+` or `-` node whose children are the two sub-trees `$1` and `$3`.
- L19: `| T ;` no action: the default `$$ = $1` passes the tree up.
- L20–21: `T '*' F` and `T '/' F`: make a `*` or `/` node.
- L22: `| F ;` passes the tree up.
- L23: `'(' E ')'`: brackets add **no node**. Pass the inside tree up (`$2`).
- L24–25: a name or a number becomes a **leaf**: `mk($1, 0, 0)`.
- L26: `%%` ends the rules.
- L27: `main` prints the prompt and calls `yyparse()`.

## 🧪 Dry run

Input `x = a + b * c`. The tree built bottom-up:
```
      =
     / \
    x   +
       / \
      a   *
         / \
        b   c
```
Post-order = left, right, then the node. So: `x`, then (`a`, then (`b`, `c`, `*`), `+`), then `=`.

| Input | Output |
|---|---|
| x = a + b * c | Post-order Traversal of AST: x a b c * + = |
| y = (a+b)*c-4/d | Post-order Traversal of AST: y a b + c * 4 d / - = |

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| struct fields `token`, `left`, `right`; functions `mknode`, `print_postorder` | `t`, `l`, `r`; `mk`, `post` |
| `print_postorder` checked `if (tree->left)` before each call | one `if (!n) return` at the top |
| function prototypes before the grammar | dropped. Each function is defined before it is used. |
| `%union { char *sval; struct node *nval; }` | `{ char *s; struct node *n; }` |
| `%type <nval> S E T F` | `<n> E T F` (S returns no value) |
| `%left '+' '-'` and `%left '*' '/'` | dropped. The layered grammar already has no ambiguity. |
| temporary variables `id_node`, `root` | inlined into one line |
| the banner line `--- Abstract Syntax Tree Generated Successfully ---` | removed. The tree output is identical. |

## ❓ Common confusions

- **Why do `*` and `/` get a different rule (T) from `+` and `-` (E)?** The deeper rule finishes first. `b*c` is grouped inside T before E can add it to `a`.
- **Why `strdup` in the lexer?** `yytext` changes on the next match. The copy keeps the name alive for the tree.
- **Why no node for brackets?** Brackets only change **grouping**. The tree shape already holds the grouping.

## 🧠 Self-test

Draw the tree for `x = a + b * c`, then write `mk`, `post`, and the rule `E: E '+' T { $$ = mk(\"+\", $1, $3); }`.

**Keep this:** Each rule builds a node from its parts; layering E / T / F gives `*` priority; post-order prints the tree.

---

<a id="q10"></a>
# 📘 Q10: FOR statement syntax check

**Files:** `10.l`, `10.y` | **Build:** `lex 10.l && yacc -d 10.y && gcc lex.yy.c y.tab.c -o 10`

## 🎯 The big idea

Lex turns the text into tokens. Yacc only checks the **order**: `for ( init ; cond ; inc ) body`. Each of init, cond and inc may be **empty**, because `for(;;)` is legal C. A `for` can also appear inside a body, so loops can nest.

## Part 1: 10.l: the lexer

`10.l` lines 1–14
```lex
 1  %{
 2  // Q10 (lex part).  Build: lex 10.l && yacc -d 10.y && gcc lex.yy.c y.tab.c -o 10
 3  #include "y.tab.h"
 4  %}
 5  %option noyywrap
 6  %%
 7  "for"                      return FOR;
 8  [a-zA-Z_][a-zA-Z0-9_]*     return ID;
 9  [0-9]+                     return NUM;
10  "<="|">="|"=="|"!="|"<"|">"  return RELOP;
11  "++"                       return INC;
12  "--"                       return DEC;
13  [ \t\n]                    ;
14  .                          return yytext[0];
```

- L1–4: the C block with `y.tab.h` (the token names). L5: `%option noyywrap`. L6: `%%` starts the rules.
- L7: `\"for\"` returns `FOR`. It is listed **before** the name rule. Both match three letters, a tie, so the earlier rule wins. That is why `for` is a keyword and not a name.
- L8: a name (letters, digits, `_`) returns `ID`.
- L9: digits return `NUM`.
- L10: all six comparison operators `<= >= == != < >` return one token, `RELOP`. Lex takes the longest match, so `<=` is read as one token and not `<` then `=`.
- L11: `++` returns `INC`. L12: `--` returns `DEC`.
- L13: spaces, tabs and newlines are skipped.
- L14: any other character (`( ) ; { } = + - * /`) is returned as itself.

## Part 2: 10.y: the grammar

`10.y` lines 1–20
```yacc
 1  %{
 2  // Q10 (yacc part): syntax check of a C for-statement.
 3  #include <stdio.h>
 4  int yylex(); void yyerror(const char *s) { printf("\nResult: Invalid FOR statement syntax (Error: %s)\n", s); }
 5  %}
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

- L1–5: the C block. `yyerror` prints `Result: Invalid FOR statement syntax` with Yacc's message.
- L6: the token names.
- L7–8: `%left` lines for `+ - * /`. They settle an ambiguity in `expr` (without them `a+b+c` could be grouped two ways and Yacc warns about conflicts).
- L9: `%%` starts the rules.
- L10: `prog` is one `stmt`. If it matches, print `Valid`.
- L11: `stmt: FOR '(' init ';' cond ';' inc ')' body` is the shape of a `for`.
- L12: `init` is empty, or `ID '=' expr`, like `i=0`.
- L13: `cond` is empty, or `expr RELOP expr`, or just `expr`.
- L14: `inc` is empty, or `ID '=' expr`, `ID INC` (`i++`), `ID DEC`, `INC ID` (`++i`), `DEC ID`.
- L15: `expr` is a name, a number, two exprs joined by an operator, or `( expr )`.
- L16: `body` is `;` (empty), `{ list }`, or one statement.
- L17: `list` is empty, or `list one`: zero or more statements.
- L18: `one` (a statement) is `ID '=' expr ';'` or another `stmt`. The second choice makes nested `for` loops work.
- L19: `%%` ends the rules.
- L20: `main` prints the prompt and calls `yyparse()`.

## 🧪 Dry run

| Input | Result |
|---|---|
| for(i=0;i<10;i++){ a=b+1; } | Valid FOR statement syntax |
| for(i=0;i<10;i++) a=b+1; | Valid FOR statement syntax |
| for(;;); | Valid FOR statement syntax |
| for(i=0;i<=n;i=i+1){ for(j=0;j<n;--j) x=y; } | Valid FOR statement syntax |
| for(i=0;i<10;i++ | Invalid FOR statement syntax (Error: syntax error) |
| for(i=0;i<10;i++){ a=b+1 } | Invalid FOR statement syntax (Error: syntax error) |
| for i=0;i<10;i++ a=1; | Invalid FOR statement syntax (Error: syntax error) |

## 🔁 What changed from your original

| Your original | Compact |
|---|---|
| tokens `LE GE EQ NE` and a `relop` rule `'<' \| '>' \| LE \| GE \| EQ \| NE` | the lexer returns one `RELOP` for all six; the `relop` rule is gone |
| rule names `program`, `for_stmt`, `initialization`, `condition`, `increment`, `statement_list`, `statement` | `prog`, `stmt`, `init`, `cond`, `inc`, `list`, `one` |
| no precedence lines. **Bison reported 16 shift/reduce conflicts** on `expr` | `%left` lines added. **0 conflicts.** |
| `yyerror` printed to `stderr` | prints to normal output, so the message appears next to the result |
| `#include <stdlib.h>`, `#include <string.h>` and a `yywrap` function | dropped or replaced by `%option noyywrap` |

## ❓ Common confusions

- **Why does `for(;;);` work?** `init`, `cond` and `inc` all have an empty choice, and `body` can be just `;`.
- **Why is a missing `;` inside the braces an error?** `one: ID '=' expr ';'` needs that `;`.
- **Why is the empty choice the most common exam mistake?** People forget it. Real C allows each of the three parts to be blank.

## 🧠 Self-test

Write `stmt` first. Then give `init`, `cond`, `inc` an empty choice each. Then `body` and the nested `one: ... | stmt`.

**Keep this:** Same for-grammar; empty init/cond/inc is allowed; a nested `for` is a statement.

---

<a id="revision"></a>
# 🗂️ ONE-PAGE REVISION: WHAT TO SAY IN A VIVA

| Q | One sentence you can say confidently |
|---|---|
| 1 | "For each state I run DFS over the ε edges only; the marked states are its closure." |
| 2 | "New δ(q,a) is closure of the move on `a` from closure(q). A state accepts if its closure has an accepting state." |
| 3 | "A DFA state is a set of NFA states, stored as a bitmask. I take the union of the moves per alphabet and add the set if it is new." |
| 4 | "I mark pairs that differ, repeat until nothing changes, and merge the unmarked pairs." |
| 5 | "The first character picks the token kind; I read while it fits and push back the one that does not." |
| 6 | "Two rules; the longer or earlier match wins, so the `mehr` rule is first." |
| 7 | "Lex returns VARIABLE for letter-then-letters-or-digits; Yacc accepts VARIABLE then newline." |
| 8 | "`$$ = $1 op $3` per rule; `%left` lines (lowest first) give `*` priority over `+`; `%prec UMINUS` for unary minus." |
| 9 | "Each rule builds a node from its children; the layered grammar gives precedence; post-order prints the tree." |
| 10 | "`for ( init ; cond ; inc ) body` with each of the three allowed to be empty; a `for` can be a statement, so loops nest." |


## 🔑 Lines to understand without thinking

```c
if (!strcmp(s, "e")) e[x][y] = 1;        // Q1: the symbol is epsilon
tr[x][k] |= 1 << y;                      // Q3: add state y to the move-set
if (dm[cur] >> i & 1) s |= tr[i][k];     // Q3: i is in the group, so add its moves
x[i][j] = x[j][i] = ch = 1;              // Q4: mark the pair, remember a change
ungetc(c, fp);                           // Q5: give the extra character back
$$ = $1 + $3;                            // Q8: value of the rule
$$ = mk("+", $1, $3);                    // Q9: build a tree node
```

---

<a id="checks"></a>
# ✅ WHAT I RAN TO CHECK

- **Q1:** on 300 random inputs the **entire output, prompts included**, matched your `1-e-closure-1.c`. Your screenshot input gives the same closures you got.
- **Q2, Q3:** on 300 random inputs each, the results matched the earlier number-input versions, which in turn matched your original matrix programs on 300 random inputs each. Example A gives the same results as the study guide.
- **Q4:** 300 random DFAs gave output identical to your `4-min-dfa.c`.
- **Q5:** output identical to `5-lex.c` on `5-input.txt`.
- **Q6–Q10:** same accept/reject or same results as your originals on all my test inputs (Q10: 7 for-statements, valid and broken).
- **Numbers in the dry runs** (bitmasks, rounds, closures) were computed by code and checked against the program's own output.
- **Built with** `flex` and `bison -y` here. Your lab machine's `lex` and `yacc` should behave the same.

**Differences you should know about**

- Q9 does not print the "AST Generated Successfully" banner. The tree output is the same.
- Q8: an unknown character like `$` now gives Yacc's `syntax error` instead of your own "Unknown character" message.
- Q10: your original had 16 shift/reduce conflicts; the compact grammar has none.
- The study guide's Q6 dry-run table has an unfinished row for `comeHeRe`. The real answer is **Accepted** (run above).
