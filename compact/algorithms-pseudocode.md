# Algorithms and Pseudocode: Q1–Q10 (CSL 411 Cycle I and II)

Matches the compact programs `1.c` to `10.y`. Each program has: **Algorithm** (plain steps), **Pseudocode**, and a **Keep this** line.

## Contents
- [How to read the pseudocode](#how-to-read)
- [Q1 ε-closure](#q1) · [Q2 NFA with ε → NFA](#q2) · [Q3 NFA → DFA](#q3) · [Q4 Minimize DFA](#q4) · [Q5 Lexical analyzer](#q5)
- [Q6 Lex "mehr"](#q6) · [Q7 Variable](#q7) · [Q8 Calculator](#q8) · [Q9 AST](#q9) · [Q10 FOR syntax](#q10)
- [Cheat sheet](#cheat)
- [**PART 2: Long exam-style algorithms (30 marks)**](#part2)

<a id="how-to-read"></a>
## How to read the pseudocode

| Word | Meaning |
|---|---|
| `INDEX(list, name)` | position of `name` in `list`, or -1 if it is not there |
| `ε` / `e` | the empty move. You type `e` in the input |
| `δ(q, a)` | the states you reach from `q` on alphabet `a` |
| `closure(q)` | all states reachable from `q` using only ε edges (q included) |
| `{ ... }` | a set |
| Lex rule | `pattern → action` |
| Yacc rule | `left : right-side` with `$1 $2 $3` = values of the right-side items |

---

<a id="q1"></a>
# Q1. ε-closure of every state

## Algorithm
1. Read the state names, the alphabet names, and the number of transitions.
2. For each transition `source symbol destination`: find the state numbers. If a name is unknown, ask for that transition again.
3. If the symbol is `e`, record an ε edge from source to destination. Ignore other symbols.
4. For each state `i`: clear the visited list, run DFS from `i` over the ε edges, then print every visited state.

## Pseudocode
```
FUNCTION DFS(s)
    visited[s] ← 1
    FOR j FROM 0 TO n-1
        IF eps[s][j] = 1 AND visited[j] = 0 THEN
            DFS(j)

MAIN
    READ n, state names, m, alphabet names, t
    FOR each of t transitions
        READ src, sym, dst
        x ← INDEX(states, src);  y ← INDEX(states, dst)
        IF x = -1 OR y = -1 THEN  ask this transition again
        IF sym = "e" THEN  eps[x][y] ← 1
    FOR i FROM 0 TO n-1
        CLEAR visited
        DFS(i)
        PRINT "Epsilon-Closure(" name[i] ") = {" every state j with visited[j] = 1 "}"
```

**Keep this:** DFS from each state over ε edges only; the marked states are the closure.

---

<a id="q2"></a>
# Q2. NFA with ε → NFA without ε

## Algorithm
1. Read states, alphabets (not counting `e`), transitions and accepting states. An `e` transition is an ε edge. Any other is a normal edge on that alphabet.
2. Compute `closure(i)` for every state `i` by DFS over the ε edges.
3. For every state `q` and alphabet `a`, build the new move:
   **newδ(q, a) = closure( δ( closure(q), a ) )**
   (slide along ε, take one real `a` step, slide along ε again).
4. A state is accepting in the new machine if its closure contains an accepting state.
5. Print the new transitions and accepting states.

## Pseudocode
```
READ n, state names, m, alphabet names, t
FOR each of t transitions
    READ src, sym, dst
    x ← INDEX(states, src);  y ← INDEX(states, dst)
    IF sym = "e" THEN  eps[x][y] ← 1
    ELSE a ← INDEX(alphabets, sym);  delta[x][y][a] ← 1
    (if a name or symbol is unknown: ask again)
READ accepting states  →  accept[ ] flags

FOR i FROM 0 TO n-1                         // closures
    CLEAR visited;  DFS over eps from i
    closure[i] ← visited

FOR q IN states
  FOR a IN alphabets
    FOR p IN states WHERE closure[q][p]     // slide along ε
      FOR t IN states WHERE delta[p][t][a]  // one real step
        FOR u IN states WHERE closure[t][u] // slide along ε
            newDelta[q][u][a] ← 1

FOR q IN states
    newAccept[q] ← 1 IF ANY p WITH closure[q][p] AND accept[p]

PRINT states
FOR q, a:  PRINT "δ(q, a) = {" every u with newDelta[q][u][a] "}"      // ∅ if none
PRINT accepting states: every q with newAccept[q]
```

**Keep this:** closure → move → closure; accepting if the closure touches an accepting state.

---

<a id="q3"></a>
# Q3. NFA → DFA (subset construction)

## Algorithm
1. Read states, alphabets, transitions and accepting states. There are no ε edges.
2. A **group** of NFA states is stored as one number (a bitmask): bit j is 1 when state j is in the group.
3. For each state `i` and alphabet `k`, build `tr[i][k]` = the group of states reachable from `i` on `k`.
4. DFA state 0 = the group {first state}.
5. Take DFA states in order (the list grows as you go). For each alphabet `k`: take the **union** of `tr[i][k]` over every state `i` in the group. Look for that union in the list. If it is not there, add it as a new DFA state. Record the transition.
6. A DFA state is accepting if its group shares a state with the accepting set.
7. Print the DFA states (with their groups), the transitions and the accepting states.

## Pseudocode
```
READ n, state names, m, alphabet names, t
FOR each of t transitions
    READ src, sym, dst
    x ← INDEX(states, src);  y ← INDEX(states, dst);  k ← INDEX(alphabets, sym)
    tr[x][k] ← tr[x][k] OR (1 << y)           // add state y to the set
READ accepting states:  fin ← fin OR (1 << x) for each

group[0] ← 1                                   // {first state}
count ← 1
cur ← 0
WHILE cur < count
    FOR k IN alphabets
        s ← 0
        FOR i FROM 0 TO n-1
            IF (group[cur] >> i) AND 1 THEN  s ← s OR tr[i][k]
        j ← position of s in group[0 .. count-1]
        IF s is not found THEN
            group[count] ← s;  j ← count;  count ← count + 1
        dfaTrans[cur][k] ← j
    cur ← cur + 1

PRINT count
FOR each DFA state i:  PRINT i and the names of the NFA states in group[i]   // ∅ if group = 0
FOR i, k:  PRINT "δ(q i, alphabet k) = q dfaTrans[i][k]"
PRINT every i WHERE (group[i] AND fin) ≠ 0     // accepting
```

**Keep this:** a DFA state is a set of NFA states; take the union of the moves for each alphabet; a new set becomes a new state.

---

<a id="q4"></a>
# Q4. Minimize a DFA

## Algorithm (table-filling)
1. Read the number of states and alphabets, the table of next-states, and the accepting states.
2. Mark a pair of states as **different** if exactly one of them is accepting.
3. Repeat: for every unmarked pair `(i, j)`, look at each alphabet. If `i` and `j` move to a pair that is already marked, mark `(i, j)`. Stop when a whole round marks nothing new.
4. Group the states: every unmarked pair is the same group. Number the groups in order of first appearance.
5. For each group, take its first state (the representative). Its transitions, with targets renamed to group numbers, are the group's transitions. A group accepts if its representative accepts. The start state is the group of old state 0.

## Pseudocode
```
READ n, m, alphabet letters
READ next[i][k] FOR every state i and alphabet k
READ accepting states  →  accept[ ]

FOR i, j:  diff[i][j] ← (accept[i] ≠ accept[j])

REPEAT
    changed ← 0
    FOR i FROM 0 TO n-1
      FOR j FROM i+1 TO n-1
        IF diff[i][j] = 0 THEN
          FOR k IN alphabets
            IF diff[ next[i][k] ][ next[j][k] ] = 1 THEN
                diff[i][j] ← 1;  diff[j][i] ← 1;  changed ← 1
                BREAK
UNTIL changed = 0

groups ← 0
FOR i FROM 0 TO n-1:  cls[i] ← -1
FOR i FROM 0 TO n-1
    IF cls[i] = -1 THEN
        FOR j FROM i TO n-1
            IF diff[i][j] = 0 THEN  cls[j] ← groups
        groups ← groups + 1

PRINT groups
FOR each group g
    rep ← first state with cls[rep] = g
    FOR k IN alphabets:  PRINT "δ(q g, k) = q cls[ next[rep][k] ]"
PRINT every group g WHERE accept[rep of g]
PRINT start state: cls[0]
```

**Keep this:** mark different pairs, spread the marks until stable, merge the unmarked pairs.

---

<a id="q5"></a>
# Q5. Lexical analyzer

## Algorithm
1. Open `5-input.txt`. If it cannot be opened, print a message and stop.
2. Read one character `c` at a time until the end of the file.
3. If `c` is a space, tab or newline, skip it.
4. If `c` is a letter or `_`: keep reading while characters are letters, digits or `_`. Push the extra character back. If the word is in the keyword list, print **Keyword**; otherwise **Identifier**.
5. If `c` is a digit: keep reading digits. Push the extra character back. Print **Number**.
6. If `c` is one of `+ - * / = < > %`: print **Operator**.
7. If `c` is one of `; , ( ) { } [ ]`: print **Special Symbol**.
8. Otherwise print **Unknown**.

## Pseudocode
```
OPEN file "5-input.txt"   (IF it fails: PRINT "File not found"; STOP)
WHILE (c ← READ CHAR) ≠ EOF
    IF c IS space OR tab OR newline THEN  CONTINUE
    IF c IS letter OR c = "_" THEN
        word ← ""
        WHILE c IS letter OR digit OR "_"
            word ← word + c;  c ← READ CHAR
        PUSH BACK c
        IF word IN keywords THEN PRINT word " : Keyword"
        ELSE PRINT word " : Identifier"
    ELSE IF c IS digit THEN
        num ← ""
        WHILE c IS digit
            num ← num + c;  c ← READ CHAR
        PUSH BACK c
        PRINT num " : Number"
    ELSE IF c IN "+-*/=<>%" THEN  PRINT c " : Operator"
    ELSE IF c IN ";,(){}[]" THEN  PRINT c " : Special Symbol"
    ELSE PRINT c " : Unknown"
```

**Keep this:** the first character picks the token type; read while it fits; push back the one that does not.

---

<a id="q6"></a>
# Q6. Lex: reject strings containing "mehr"

## Algorithm
1. Lex reads the input line by line and tries every rule on the text.
2. Lex takes the **longest** match. If two rules match the same length, the **first rule** wins.
3. Rule 1 matches any line that contains `m e h r` touching, in any capitals. It prints Rejected.
4. Rule 2 matches any line. It prints Accepted. Rule 1 is listed first so it wins the tie.
5. Rule 3 swallows the newline so nothing extra is printed.

## Pseudocode
```
RULES (in this order)
    ANYTHING  "m" "e" "h" "r" (any case)  ANYTHING   →  PRINT "Rejected"
    ANYTHING                                          →  PRINT "Accepted"
    NEWLINE                                           →  do nothing

MAIN
    PRINT "Enter String:"
    RUN lexer until input ends
```

**Keep this:** longest match wins; ties go to the first rule, so the "mehr" rule comes first.

---

<a id="q7"></a>
# Q7. YACC: valid variable name

## Algorithm
1. **Lex part:** a letter followed by any letters or digits becomes one token `VARIABLE`. Spaces and tabs are skipped. Any other character (including newline) is returned as itself.
2. **Yacc part:** the input is zero or more lines. A line is either `VARIABLE` then newline (print Valid), or just a newline.
3. Anything else cannot match the grammar, so Yacc calls the error routine, which prints Invalid.

## Pseudocode
```
LEX RULES
    letter (letter OR digit)*   →  RETURN VARIABLE
    space OR tab                →  skip
    newline OR any other char   →  RETURN that character

YACC GRAMMAR
    input :  (empty)
          |  input VARIABLE NEWLINE      { PRINT "Result: Valid Variable" }
          |  input NEWLINE

ON SYNTAX ERROR
    PRINT "Result: Invalid Variable"
```

**Keep this:** Lex finds the word; Yacc checks the shape of the line.

---

<a id="q8"></a>
# Q8. Calculator with LEX and YACC

## Algorithm
1. **Lex part:** digits become the token `NUMBER`, and the number's value goes in `yylval`. Spaces are skipped. Any other character (operators, brackets, newline) is returned as itself.
2. **Yacc part:** declare precedence, lowest first: `+ -`, then `* /`, then unary minus.
3. Each `expr` rule computes its value from its parts. When a full line `expr NEWLINE` is matched, print the value.
4. Division by zero is checked inside the action.

## Pseudocode
```
LEX RULES
    digits         →  yylval ← number value;  RETURN NUMBER
    space OR tab   →  skip
    other char     →  RETURN that character

PRECEDENCE (lowest to highest)
    LEFT  "+" "-"
    LEFT  "*" "/"
    RIGHT UMINUS

YACC GRAMMAR
    calc :  (empty)
         |  calc expr NEWLINE        { PRINT "Result: " $2 }
         |  calc NEWLINE

    expr :  NUMBER                   { $$ ← $1 }
         |  expr "+" expr            { $$ ← $1 + $3 }
         |  expr "-" expr            { $$ ← $1 - $3 }
         |  expr "*" expr            { $$ ← $1 * $3 }
         |  expr "/" expr            { IF $3 = 0 THEN  PRINT error "Division by zero";  $$ ← 0
                                       ELSE  $$ ← $1 / $3 }
         |  "(" expr ")"             { $$ ← $2 }
         |  "-" expr  (precedence UMINUS)   { $$ ← - $2 }
```

**Keep this:** `$$ = $1 op $3`; the precedence lines (lowest first) make `*` happen before `+`.

---

<a id="q9"></a>
# Q9. BNF → YACC: build an AST

## Algorithm
1. Grammar (layered, so `* /` group before `+ -`):
   `S → id = E`, `E → E+T | E-T | T`, `T → T*F | T/F | F`, `F → (E) | id | num`.
2. **Lex part:** a name returns `ID` and a number returns `NUM`. Both carry a copy of their text. A newline returns 0 (end of input).
3. A tree node holds a text and a left and right child.
4. Each rule builds a node:
   - a name or number → a **leaf** (no children);
   - `E + T`, `E - T`, `T * F`, `T / F` → a node for the operator with the two sub-trees as children;
   - `( E )` → pass the inside tree up unchanged;
   - `id = E` → a `=` node with the name on the left and the tree on the right. Then print it in post-order.
5. **Post-order:** print the left tree, then the right tree, then the node itself.

## Pseudocode
```
NODE = (text, left, right)
FUNCTION MK(text, left, right)  →  new node with a copy of text
FUNCTION POST(n)
    IF n = NULL THEN RETURN
    POST(n.left);  POST(n.right);  PRINT n.text

LEX RULES
    letter (letter OR digit)*   →  yylval ← COPY(text);  RETURN ID
    digits                      →  yylval ← COPY(text);  RETURN NUM
    space OR tab                →  skip
    newline                     →  RETURN 0
    other char                  →  RETURN that character

YACC GRAMMAR
    S :  ID "=" E        { root ← MK("=", MK($1, NULL, NULL), $3);  POST(root) }
    E :  E "+" T         { $$ ← MK("+", $1, $3) }
      |  E "-" T         { $$ ← MK("-", $1, $3) }
      |  T               { $$ ← $1 }
    T :  T "*" F         { $$ ← MK("*", $1, $3) }
      |  T "/" F         { $$ ← MK("/", $1, $3) }
      |  F               { $$ ← $1 }
    F :  "(" E ")"       { $$ ← $2 }
      |  ID              { $$ ← MK($1, NULL, NULL) }
      |  NUM             { $$ ← MK($1, NULL, NULL) }
```

**Keep this:** each rule builds a node from its parts; layering E / T / F gives `*` priority; post-order prints the tree.

---

<a id="q10"></a>
# Q10. FOR statement syntax check

## Algorithm
1. **Lex part:** `for` → `FOR` (listed before the name rule so it wins the tie). Names → `ID`. Digits → `NUM`. The six comparison operators `< > <= >= == !=` → one token `RELOP`. `++` → `INC`. `--` → `DEC`. Whitespace is skipped. Other characters are returned as themselves.
2. **Yacc part:** a `for` statement is `for ( init ; cond ; inc ) body`.
3. `init`, `cond` and `inc` can each be **empty**.
4. A `body` is `;`, or `{ statements }`, or one statement.
5. A statement is `id = expr ;` or another `for` statement. This is what lets loops nest.
6. If the whole input matches, print Valid. Otherwise the error routine prints Invalid.

## Pseudocode
```
LEX RULES
    "for"                       →  RETURN FOR
    letter/_ (letter/digit/_)*  →  RETURN ID
    digits                      →  RETURN NUM
    "<=" OR ">=" OR "==" OR "!=" OR "<" OR ">"  →  RETURN RELOP
    "++" →  RETURN INC          "--" →  RETURN DEC
    whitespace                  →  skip
    other char                  →  RETURN that character

YACC GRAMMAR
    prog :  stmt                          { PRINT "Valid FOR statement syntax" }
    stmt :  FOR "(" init ";" cond ";" inc ")" body
    init :  (empty)  |  ID "=" expr
    cond :  (empty)  |  expr RELOP expr  |  expr
    inc  :  (empty)  |  ID "=" expr  |  ID INC  |  ID DEC  |  INC ID  |  DEC ID
    expr :  ID | NUM | expr "+" expr | expr "-" expr | expr "*" expr | expr "/" expr | "(" expr ")"
    body :  ";"  |  "{" list "}"  |  one
    list :  (empty)  |  list one
    one  :  ID "=" expr ";"  |  stmt

ON SYNTAX ERROR
    PRINT "Invalid FOR statement syntax"
```

**Keep this:** same for-grammar; empty init / cond / inc is allowed; a nested `for` is a statement.

---

<a id="cheat"></a>
# Cheat sheet

| Q | Core idea in one line |
|---|---|
| 1 | DFS over ε edges from each state |
| 2 | newδ(q,a) = closure(δ(closure(q), a)); accepting if closure touches an accepting state |
| 3 | DFA state = set of NFA states (bitmask); union of moves; add if new |
| 4 | mark different pairs; repeat until stable; merge unmarked pairs |
| 5 | first character picks the token; read while it fits; push back the extra one |
| 6 | longest match wins; tie → first rule |
| 7 | Lex: VARIABLE token. Yacc: `VARIABLE '\n'` |
| 8 | `$$ = $1 op $3`; `%left` lowest first; `%prec UMINUS` |
| 9 | each rule builds a node; post-order prints |
| 10 | `for ( init ; cond ; inc ) body`; any of the three may be empty |


---

<a id="part2"></a>
# PART 2: Long exam-style algorithms (for 30 marks)

How to write it in the exam: **Aim → Algorithm (all steps) → Program → Input and Output → Result.**
Copy the steps below in order. Every step is one line. Write `Start` first and `Stop` last.
For Q6–Q10 the algorithm has two parts: the **Lex part** and the **Yacc part** (Q6 has Lex only).

---

## Q1. Epsilon closure of all states

**Aim:** Find the ε-closure of every state of an NFA with ε moves.
**Definition:** ε-closure(q) = the set of states reachable from q using only ε moves (q is always included).

**Algorithm**
1. Start.
2. Declare `n` (number of states), `m` (number of alphabets), `t` (number of transitions), the matrix `e[n][n]` for ε edges, the list `visited[n]`, and name lists for states and alphabets.
3. Read `n`. Read the `n` state names. Number them 0, 1, 2 … in the order typed.
4. Read `m`. Read the `m` alphabet names.
5. Read `t`, the number of transitions.
6. Set every `e[i][j]` to 0.
7. Repeat for each of the `t` transitions:
   1. Read `source`, `input`, `destination` (for example `q0 e q1`).
   2. Find the state number `x` of `source` and `y` of `destination`.
   3. If `x` or `y` is not found, print "invalid state", discard this transition and read it again.
   4. If `input` is `e`, set `e[x][y] = 1`.
   5. If `input` is any other alphabet, ignore it (it is not an ε move).
8. For each state `i` from 0 to n-1, repeat steps 9 to 13.
9. Set all of `visited[]` to 0.
10. Call `DFS(i)`.
11. Print "Epsilon-Closure(name of i) = {".
12. For each state `j` from 0 to n-1: if `visited[j] = 1`, print name of `j`.
13. Print "}".
14. Stop.

**Function DFS(s)**
1. Set `visited[s] = 1`.
2. For each state `j` from 0 to n-1:
   if `e[s][j] = 1` and `visited[j] = 0`, call `DFS(j)`.
3. Return.

**Why `i` is in its own closure:** `DFS(i)` marks `i` first, so a state always reaches itself.

---

## Q2. NFA with ε moves to NFA without ε moves

**Aim:** Remove all ε moves from an NFA.
**Formula:** newδ(q, a) = ε-closure( δ( ε-closure(q), a ) ).
Meaning: go along ε moves, take one real `a` move, go along ε moves again.

**Algorithm**
1. Start.
2. Declare `n`, `m`, `t`, the ε matrix `c[n][n]`, the move table `d[n][n][m]` (1 if state x goes to state y on alphabet a), the closure matrix `cl[n][n]`, the accepting list `f[n]`, the new move table `nt[n][n][m]` and the new accepting list `nf[n]`.
3. Read the `n` state names and the `m` alphabet names. Do not count `e` as an alphabet.
4. Read `t`, then read each transition as `source input destination`.
5. Find the state numbers `x` and `y`. If `input` is `e`, set `c[x][y] = 1`.
6. If `input` is an alphabet with number `a`, set `d[x][y][a] = 1`.
7. If any name or alphabet is unknown, ask for that transition again.
8. Read the number of accepting states, then read their names. Set `f[state] = 1` for each.
9. **Find closures.** For each state `i`:
   1. Set `visited[]` to 0 and call `DFS(i)` over the ε matrix `c`.
   2. Copy `visited[]` into row `i` of `cl`. So `cl[i][p] = 1` means p is in the closure of i.
10. **Build new moves.** For each state `q` and each alphabet `a`:
    1. For each state `p` with `cl[q][p] = 1` (slide along ε):
    2. For each state `t` with `d[p][t][a] = 1` (take one real step):
    3. For each state `u` with `cl[t][u] = 1` (slide along ε again):
    4. Set `nt[q][u][a] = 1`.
11. **Find new accepting states.** For each state `q`: set `nf[q] = 1` if some state `p` has `cl[q][p] = 1` and `f[p] = 1`.
12. Print the state names.
13. For each state `q` and alphabet `a`: print `δ(q, a) = {` all `u` with `nt[q][u][a] = 1` `}`. Print `∅` if there is none.
14. Print the new accepting states (all `q` with `nf[q] = 1`).
15. Stop.

---

## Q3. NFA to DFA (subset construction)

**Aim:** Convert an NFA (without ε) to an equivalent DFA.
**Idea:** One DFA state = a set of NFA states. Store a set as one integer (bitmask): bit `j` is 1 when state `j` is in the set. `1<<j` makes a set with only state `j`. `|` is union. `(x >> i) & 1` tests state `i`.

**Algorithm**
1. Start.
2. Declare `n`, `m`, `t`, `tr[n][m]` (set reached from state i on alphabet k), `fin` (set of accepting states), `dm[]` (list of DFA states, each a set), `dt[][]` (DFA move table) and `cnt` (number of DFA states).
3. Read the state names, the alphabet names and the number of transitions.
4. Set every `tr[i][k] = 0` and `fin = 0`.
5. For each transition `source input destination`:
   find `x`, `y` and the alphabet number `k`; set `tr[x][k] = tr[x][k] | (1<<y)`. Ask again if a name is unknown.
6. Read the accepting states. For each state `x`: set `fin = fin | (1<<x)`.
7. Set `dm[0] = 1` (the set `{first state}`) and `cnt = 1`.
8. Set `cur = 0`.
9. **While `cur < cnt`:**
   1. For each alphabet `k` from 0 to m-1:
   2. Set `s = 0`.
   3. For each state `i` from 0 to n-1: if state `i` is in `dm[cur]`, set `s = s | tr[i][k]`.
   4. Search for `s` in `dm[0 … cnt-1]`. Let `j` be the place where it is found.
   5. If `s` is not found, set `dm[cnt] = s`, set `j = cnt`, then `cnt = cnt + 1`.
   6. Set `dt[cur][k] = j`.
   7. After all alphabets, set `cur = cur + 1`.
10. Print `cnt` (number of DFA states).
11. For each DFA state `i`: print `q i = {` names of the NFA states in `dm[i]` `}`. Print `∅` if the set is empty.
12. For each `i` and `k`: print `δ(q i, alphabet k) = q dt[i][k]`.
13. For each DFA state `i`: if `dm[i] & fin` is not 0, `i` is an accepting state. Print it.
14. Stop.

**Why it ends:** the list grows only when a new set appears, and there are at most 2ⁿ sets.

---

## Q4. Minimization of a DFA

**Aim:** Find the DFA with the fewest states that accepts the same language.
**Method:** table-filling. Two states are *distinguishable* if some input string takes one to an accepting state and the other to a non-accepting state.

**Algorithm**
1. Start.
2. Declare `n`, `m`, the move table `next[n][m]`, `accept[n]`, the pair table `diff[n][n]`, the group list `cls[n]` and `groups`.
3. Read `n`, `m` and the alphabet letters.
4. Read `next[i][k]` for every state `i` and alphabet `k`.
5. Read the accepting states and set `accept[i] = 1` for each.
6. **Mark the base pairs.** For every pair `(i, j)`: set `diff[i][j] = 1` if exactly one of `i`, `j` is accepting; otherwise 0.
7. **Repeat until nothing changes:**
   1. Set `changed = 0`.
   2. For every pair `i < j` with `diff[i][j] = 0`:
   3. For each alphabet `k`: let `a = next[i][k]` and `b = next[j][k]`.
   4. If `diff[a][b] = 1`, set `diff[i][j] = diff[j][i] = 1`, set `changed = 1`, and go to the next pair.
   5. If `changed = 1` after the whole round, repeat the round.
8. **Make groups.** Set `cls[i] = -1` for all `i` and `groups = 0`.
9. For each state `i` from 0 to n-1: if `cls[i] = -1`:
   1. For each state `j` from `i` to n-1: if `diff[i][j] = 0`, set `cls[j] = groups`.
   2. Set `groups = groups + 1`.
10. Print `groups` (number of states in the minimal DFA).
11. For each group `g`: pick the first state `r` with `cls[r] = g` (the representative).
12. For each alphabet `k`: print `δ(q g, k) = q cls[next[r][k]]`.
13. Print each group `g` whose representative `r` has `accept[r] = 1` as an accepting state.
14. Print `cls[0]` as the start state.
15. Stop.

**Why a mark spreads:** if `a` and `b` are different, any pair that moves into `(a, b)` is also different. So steps 7.3 and 7.4 repeat until stable.

---

## Q5. Lexical analyzer

**Aim:** Break a source file into tokens and name each token type.

**Algorithm**
1. Start.
2. Declare the keyword list (for example `int float char if else while for return void`), a character `c`, and a buffer `buf`.
3. Open the file `5-input.txt` for reading.
4. If the file cannot be opened, print "File not found" and go to step 15.
5. Read one character into `c` (keep it as an `int` so EOF can be detected).
6. While `c` is not EOF, repeat steps 7 to 13.
7. If `c` is a space, tab or newline: skip it and go to step 13.
8. **Word:** if `c` is a letter or `_`:
   1. Set `buf` empty.
   2. While `c` is a letter, a digit or `_`: add `c` to `buf` and read the next character.
   3. Push the last read character back to the file.
   4. If `buf` is in the keyword list, print `buf : Keyword`.
   5. Otherwise print `buf : Identifier`.
9. **Number:** else if `c` is a digit:
   1. Set `buf` empty.
   2. While `c` is a digit: add `c` to `buf` and read the next character.
   3. Push the last read character back.
   4. Print `buf : Number`.
10. **Operator:** else if `c` is one of `+ - * / = < > %`, print `c : Operator`.
11. **Special symbol:** else if `c` is one of `; , ( ) { } [ ]`, print `c : Special Symbol`.
12. **Other:** else print `c : Unknown`.
13. Read the next character into `c` and go back to step 6.
14. Close the file.
15. Stop.

(In your program the push-back is `ungetc(c, fp)`. The keyword test is a loop with `strcmp`.)

---

## Q6. Lex program: reject strings that contain "mehr"

**Aim:** Print Accepted or Rejected for each input line, rejecting any line that has `mehr` (any capitals) as a block.

**Algorithm: Lex part**
1. Start.
2. In the definitions part, include `stdio.h` and set `%option noyywrap`.
3. Write the rules in this order:
   1. Rule 1: `.*[mM][eE][hH][rR].*` → print `Rejected`.
   2. Rule 2: `.*` → print `Accepted`.
   3. Rule 3: `\n` → do nothing.
4. In `main`: print `Enter String:`, then call `yylex()`.
5. Lex reads a line. It tries all rules on the text from the current position.
6. Lex picks the rule with the **longest match**. If two rules match the same length, it picks the **earlier rule**.
7. A line with `mehr` is matched fully by Rule 1 and also fully by Rule 2. They tie, so Rule 1 wins and prints `Rejected`.
8. A line without `mehr` matches only Rule 2, so it prints `Accepted`.
9. Rule 3 removes the newline and then Lex goes to the next line.
10. Stop at end of input.

**Note:** this program has no Yacc part.

---

## Q7. Yacc program: check a valid variable name

**Aim:** Accept a name that starts with a letter and has only letters and digits.

**Algorithm: Lex part**
1. Start.
2. Include `y.tab.h` so Lex knows the token `VARIABLE`.
3. Rule: `[a-zA-Z][a-zA-Z0-9]*` → `return VARIABLE`.
4. Rule: space or tab → ignore.
5. Rule: `\n` or any other single character → `return` that character.
6. Define `yywrap()` to return 1.

**Algorithm: Yacc part**
1. Declare the token `VARIABLE`.
2. Grammar:
   `input : (empty) | input VARIABLE '\n' { print "Valid Variable" } | input '\n'`
3. In `main`: print the prompt, then call `yyparse()`.
4. `yyparse()` asks `yylex()` for tokens one at a time.
5. If the whole line is one `VARIABLE` followed by newline, the second alternative matches and prints Valid.
6. If the line starts with a digit or has a symbol, the token stream does not fit any rule. Yacc calls `yyerror()`, which prints `Invalid Variable`.
7. Stop.

**Compile:** `lex 7.l`, `yacc -d 7.y`, `gcc lex.yy.c y.tab.c -o 7`, `./7`.
(`-d` makes `y.tab.h`.)

---

## Q8. Calculator using Lex and Yacc

**Aim:** Evaluate expressions with `+ - * /`, brackets and unary minus.

**Algorithm: Lex part**
1. Start.
2. Include `y.tab.h`.
3. Rule: `[0-9]+` → set `yylval = atoi(yytext)`, then `return NUMBER`.
4. Rule: space or tab → ignore.
5. Rule: any other character (`+ - * / ( )` and newline) → return it as itself.

**Algorithm: Yacc part**
1. Declare the token `NUMBER`.
2. Declare precedence, lowest to highest:
   `%left '+' '-'`
   `%left '*' '/'`
   `%right UMINUS`
3. Grammar:
   `calc : (empty) | calc expr '\n' { print "Result: " $2 } | calc '\n'`
4. Rules for `expr`, each computes `$$` (the value of the left side) from `$1`, `$2`, `$3` (the right side items):
   1. `NUMBER` → `$$ = $1`
   2. `expr '+' expr` → `$$ = $1 + $3`
   3. `expr '-' expr` → `$$ = $1 - $3`
   4. `expr '*' expr` → `$$ = $1 * $3`
   5. `expr '/' expr` → if `$3 == 0` print "Division by zero" and set `$$ = 0`; else `$$ = $1 / $3`
   6. `'(' expr ')'` → `$$ = $2`
   7. `'-' expr %prec UMINUS` → `$$ = -$2`
5. Because `*` and `/` are listed below `+` and `-`, they bind tighter, so `2+3*4` gives 14.
6. `%prec UMINUS` gives the unary minus its own high priority, so `-2*3` is `(-2)*3`.
7. In `main`: call `yyparse()`. On a syntax error, `yyerror()` prints the message.
8. Stop.

---

## Q9. BNF to Yacc: build an abstract syntax tree (AST)

**Aim:** Build the tree of an assignment and print it in post-order.
**Grammar:** `S → id = E`; `E → E+T | E-T | T`; `T → T*F | T/F | F`; `F → (E) | id | num`.

**Algorithm: Lex part**
1. Start.
2. Rule: letter followed by letters or digits → `yylval.s = copy of yytext`, `return ID`.
3. Rule: digits → `yylval.s = copy of yytext`, `return NUM`.
4. Rule: space or tab → ignore.
5. Rule: `\n` → `return 0` (end of input).
6. Rule: any other character → return it.

**Algorithm: Yacc part**
1. Define a node: `text`, `left`, `right`.
2. Define `%union { char *s; Node *n; }`. Declare `ID NUM` with type `s`, and `E T F` with type `n`.
3. Function `MK(text, l, r)`: allocate a node, copy the text, set `left = l` and `right = r`, return it.
4. Function `POST(n)`: if `n` is null return; `POST(n->left)`; `POST(n->right)`; print `n->text`.
5. Rules, in order:
   1. `F : ID` → `$$ = MK($1, NULL, NULL)` (a leaf).
   2. `F : NUM` → `$$ = MK($1, NULL, NULL)`.
   3. `F : '(' E ')'` → `$$ = $2` (brackets leave no node).
   4. `T : F` → `$$ = $1`.
   5. `T : T '*' F` → `$$ = MK("*", $1, $3)`. Same for `/`.
   6. `E : T` → `$$ = $1`.
   7. `E : E '+' T` → `$$ = MK("+", $1, $3)`. Same for `-`.
   8. `S : ID '=' E` → `root = MK("=", MK($1, NULL, NULL), $3)`, then `POST(root)`.
6. The parser builds from the bottom up: leaves first, then operators, then `=`.
7. The levels E, T, F make `*` and `/` bind tighter than `+` and `-`.
8. Post-order prints left, right, node. So `a = b + c * d` prints `a b c d * + =`.
9. Stop.

---

## Q10. Yacc program: syntax check of the FOR statement

**Aim:** Check that a `for` statement is written correctly, including empty parts and nested loops.

**Algorithm: Lex part**
1. Start.
2. Rule: `for` → `return FOR`. It is listed **before** the name rule, so it wins the tie.
3. Rule: letters, digits, `_` starting with a letter or `_` → `return ID`.
4. Rule: digits → `return NUM`.
5. Rule: `<=  >=  ==  !=  <  >` → `return RELOP`.
6. Rule: `++` → `return INC`. Rule: `--` → `return DEC`.
7. Rule: whitespace → ignore.
8. Rule: any other character → return it.

**Algorithm: Yacc part**
1. Declare tokens `FOR ID NUM RELOP INC DEC`.
2. Grammar:
   1. `prog : stmt` → print "Valid FOR statement syntax".
   2. `stmt : FOR '(' init ';' cond ';' inc ')' body`
   3. `init : (empty) | ID '=' expr`
   4. `cond : (empty) | expr RELOP expr | expr`
   5. `inc : (empty) | ID '=' expr | ID INC | ID DEC | INC ID | DEC ID`
   6. `expr : ID | NUM | expr '+' expr | expr '-' expr | expr '*' expr | expr '/' expr | '(' expr ')'`
   7. `body : ';' | '{' list '}' | one`
   8. `list : (empty) | list one`
   9. `one : ID '=' expr ';' | stmt`
3. `init`, `cond` and `inc` may each be empty, so `for(;;)` is valid.
4. A `stmt` can be a `one`, and a `body` can be a `one`. So a `for` can sit inside another `for`.
5. `yyparse()` reads tokens from Lex and tries to match `prog`.
6. If the whole input matches, print `Valid FOR statement syntax`.
7. Otherwise `yyerror()` prints `Invalid FOR statement syntax`.
8. Stop.

---

*Part 2 is written from the compact programs. I did not run it as code. It is a text description of the same steps.*
