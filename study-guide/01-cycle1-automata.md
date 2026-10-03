# Cycle I — Automata Programs (1–4)

Read `00-foundations.md` first — every "Concept" line below points back to a
section there. All four dry runs use **Example A** from §A.6 of that file:

```
States: q0(start), q1, q2, q3, q4(accepting)     Alphabet: {a, b}
ε:   q0->q1,  q1->q2
a:   q2->q2,  q2->q3
b:   q3->q4
Language recognized: a⁺b
```

## Table of Contents

- [Program 1 — ε-closure of every state](#program-1)
- [Program 2 — eliminate ε-transitions](#program-2)
- [Program 3 — subset construction, NFA → DFA](#program-3)
- [Program 4 — minimize a DFA](#program-4)

---

<a id="program-1"></a>
## Program 1 — `1-e-closure.c`: ε-closure of every state

**Problem statement (syllabus):** *Write a program to find ε-closure of all
states of any given NFA with ε-transition.*

**Concept used:** §A.3 (ε-closure) — pure graph DFS over ε-edges only.

### Algorithm
1. Read `n` states and an `n×n` ε-transition matrix (`eps[i][j]=1` means
   `qi -ε-> qj`).
2. For every state `i` from `0` to `n-1`: reset a `visited[]` array, run DFS
   from `i` following only `eps` edges, mark every state reached.
3. Print all marked states as `ECLOSE(qi)`.

### Annotated code
```c
#include <stdio.h>

int n;
int eps[10][10];
int visited[10];

void dfs(int state) {
    visited[state] = 1;                 // mark current state reached
    for (int j = 0; j < n; j++) {
        if (eps[state][j] == 1 && visited[j] == 0) {
            dfs(j);                      // follow ε-edge, recurse
        }
    }
}

int main() {
    int i, j;
    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter epsilon transition matrix (%dx%d):\n", n, n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            scanf("%d", &eps[i][j]);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) visited[j] = 0;   // reset per starting state
        dfs(i);
        printf("Epsilon-Closure(q%d) = { ", i);
        for (j = 0; j < n; j++)
            if (visited[j]) printf("q%d ", j);
        printf("}\n");
    }
    return 0;
}
```
**Callouts:**
- `visited[state] = 1` is set *before* recursing — this is what stops infinite
  loops if the ε-graph has a cycle.
- The outer loop in `main` re-runs the whole DFS once per state, because
  ε-closure is defined *per starting state*, not globally.

### Dry run (Example A)
Input matrix (5 states, ε only on `q0->q1` and `q1->q2`):
```
0 1 0 0 0
0 0 1 0 0
0 0 0 0 0
0 0 0 0 0
0 0 0 0 0
```
Trace for `i=0`: `dfs(0)` marks `q0`; sees `eps[0][1]=1`, unvisited → `dfs(1)`
marks `q1`; sees `eps[1][2]=1`, unvisited → `dfs(2)` marks `q2`; `q2` has no
outgoing ε-edges → returns all the way back up.

**Output:**
```
Epsilon-Closure(q0) = { q0 q1 q2 }
Epsilon-Closure(q1) = { q1 q2 }
Epsilon-Closure(q2) = { q2 }
Epsilon-Closure(q3) = { q3 }
Epsilon-Closure(q4) = { q4 }
```

---

<a id="program-2"></a>
## Program 2 — `2-nfa-enfa.c`: eliminate ε-transitions

**Problem statement:** *Write a program to convert NFA with ε-transition to
NFA without ε-transition.*

**Concept used:** §A.3 (ε-closure, reused as a subroutine) + §A.4 (the
"move across a symbol, then close again" trick).

### Algorithm
For every state `q` and every real symbol `a`:
```
δ'(q, a) = ⋃_{p ∈ ECLOSE(q)}  ⋃_{t : δ(p,a) includes t}  ECLOSE(t)
```
In words: from `q`, first take any free ε-moves (`ECLOSE(q)`), then take one
real step on `a` from *any* of those states, landing on some `t`, then take
any more free ε-moves from `t` (`ECLOSE(t)`). The union of everywhere that
lands is the new, ε-free transition.

A state `q` becomes accepting in the new machine if `ECLOSE(q)` contains any
original accepting state (reachable "for free" from an accepting state).

### Annotated code
```c
#include <stdio.h>
#define MAX 10

int n, m;
char symbols[MAX];
int eps[MAX][MAX];
int trans[MAX][MAX][MAX];      // trans[state][nextState][symbolIndex]
int closure[MAX][MAX];         // closure[i][j] = 1 if qj in ECLOSE(qi)
int newTrans[MAX][MAX][MAX];
int accepting[MAX];
int newAccepting[MAX];

void dfs(int state, int visited[]) {
    visited[state] = 1;
    for (int j = 0; j < n; j++)
        if (eps[state][j] && !visited[j]) dfs(j, visited);
}

void computeClosure() {                 // same idea as Program 1, but stores
    for (int i = 0; i < n; i++) {        // results for every state up front
        int visited[MAX] = {0};
        dfs(i, visited);
        for (int j = 0; j < n; j++) closure[i][j] = visited[j];
    }
}

int main() {
    /* ... reads n, m, symbols, eps matrix, trans matrices, accepting ... */
    computeClosure();

    for (int q = 0; q < n; q++)
        for (int a = 0; a < m; a++)
            for (int p = 0; p < n; p++)
                if (closure[q][p])                    // p reachable via ε from q
                    for (int t = 0; t < n; t++)
                        if (trans[p][t][a])            // real move p -a-> t
                            for (int u = 0; u < n; u++)
                                if (closure[t][u])      // u reachable via ε from t
                                    newTrans[q][u][a] = 1;

    for (int q = 0; q < n; q++) {
        newAccepting[q] = accepting[q];
        for (int p = 0; p < n; p++)
            if (closure[q][p] && accepting[p]) { newAccepting[q] = 1; break; }
    }
    /* ... print newTrans / newAccepting ... */
}
```
**Callouts:**
- The quadruple-nested loop (`p`, `t`, `u`) is a direct, literal translation
  of the `δ'(q,a) = ⋃ ECLOSE(move(ECLOSE(q), a))` formula above — each
  variable name corresponds to one step: `p` = state after ε-closing `q`,
  `t` = state after one real move, `u` = state after ε-closing `t`.
- `newAccepting[q]` reuses the *same* `closure` table computed once, instead
  of re-running DFS — this is why `computeClosure()` is a separate function
  called once up front.

### Dry run (Example A → output of Program 1 feeds this)
Using `ECLOSE(q0)={q0,q1,q2}`, `ECLOSE(q1)={q1,q2}`, `ECLOSE(q2)={q2}`,
`ECLOSE(q3)={q3}`, `ECLOSE(q4)={q4}` from Program 1's dry run:

- `q0` on `a`: the only closure member with a real `a`-move is `q2`
  (`q2-a->q2`, `q2-a->q3`) → landing states `{q2,q3}`, each already its own
  closure → `δ'(q0,a) = {q2,q3}`. On `b`: nobody in `{q0,q1,q2}` has a `b`
  move → `δ'(q0,b) = ∅`.
- `q1` on `a`: same reasoning (closure is `{q1,q2}`, only `q2` moves) →
  `δ'(q1,a) = {q2,q3}`; `δ'(q1,b) = ∅`.
- `q2` on `a`: closure is just `{q2}` → `δ'(q2,a) = {q2,q3}`; `δ'(q2,b)=∅`.
- `q3` on `b`: closure is `{q3}`, `q3-b->q4` → `δ'(q3,b) = {q4}`;
  `δ'(q3,a)=∅`.
- `q4`: no outgoing moves at all.

**Output (NFA without ε):**
```
δ(q0,a) = { q2, q3 }     δ(q0,b) = ∅
δ(q1,a) = { q2, q3 }     δ(q1,b) = ∅
δ(q2,a) = { q2, q3 }     δ(q2,b) = ∅
δ(q3,a) = ∅              δ(q3,b) = { q4 }
δ(q4,a) = ∅              δ(q4,b) = ∅
Accepting states: { q4 }
```
Notice `q0`, `q1`, `q2` all behave *identically* now — a hint of the
merging that Program 4 would find if we minimized this (it's still an NFA
here, not a DFA, so minimization doesn't apply yet — but keep this
observation in your back pocket, it's a good viva point).

---

<a id="program-3"></a>
## Program 3 — `3-nfa-dfa.c`: subset construction, NFA → DFA

**Problem statement:** *Write a program to convert NFA to DFA.*

**Concept used:** §A.4 (subset/powerset construction), applied at the
*whole-automaton* level this time (building new states, not just new edges).

### Algorithm
1. DFA start state = the set `{0}` (just the NFA's start state index).
2. Maintain a growing list of DFA states, each one a **bitset over the `n`
   NFA states** (`dfaStates[i][j] = 1` means NFA state `j` is a member of
   DFA state `i`).
3. For each DFA state not yet processed, and each symbol: union together
   `δ(p, symbol)` for every NFA state `p` in that set → this bitset is the
   next DFA state. If it's new (no existing DFA state matches it via
   `isSameState`), append it to the list.
4. A DFA state is accepting if its bitset includes any original NFA
   accepting state.

### Annotated code
```c
int isSameState(int a[MAX], int b[MAX]) {     // two subsets are "the same
    for (int i = 0; i < n; i++)                // DFA state" iff identical
        if (a[i] != b[i]) return 0;            // as bitsets
    return 1;
}

int findDFAState(int subset[MAX]) {            // linear search through
    for (int i = 0; i < dfaCount; i++)          // DFA states built so far
        if (isSameState(dfaStates[i], subset)) return i;
    return -1;                                  // not found -> it's new
}

int main() {
    /* ... read n, m, symbols, trans[][][], accepting[] ... */

    int startState[MAX] = {0};
    startState[0] = 1;                          // DFA start = {NFA state 0}
    for (i = 0; i < n; i++) dfaStates[0][i] = startState[i];
    dfaCount = 1;

    for (int current = 0; current < dfaCount; current++) {   // worklist —
        // dfaCount can GROW inside this loop, so new sets get processed too

        dfaAccept[current] = 0;
        for (i = 0; i < n; i++)
            if (dfaStates[current][i] && accepting[i]) { dfaAccept[current]=1; break; }

        for (k = 0; k < m; k++) {
            int newSet[MAX] = {0};
            for (i = 0; i < n; i++)
                if (dfaStates[current][i])              // for every NFA state in this set
                    for (j = 0; j < n; j++)
                        if (trans[i][j][k]) newSet[j] = 1;   // union of its moves

            int idx = findDFAState(newSet);
            if (idx == -1) {                              // genuinely new subset
                for (i = 0; i < n; i++) dfaStates[dfaCount][i] = newSet[i];
                idx = dfaCount++;
            }
            dfaTrans[current][k] = idx;
        }
    }
}
```
**Callouts:**
- `for (int current = 0; current < dfaCount; current++)` is the classic
  "worklist" pattern: the loop bound `dfaCount` is re-read every iteration,
  so appending a new state inside the loop (`dfaCount++`) automatically
  schedules it to be processed later. This *is* the BFS that discovers all
  reachable subsets.
- `findDFAState` doing a linear scan is fine here because `dfaCount` stays
  small in practice, even though in theory it's bounded by `2^n`.

### Dry run (Example A → feeds in the NFA-without-ε from Program 2)
Start: `D0 = {q0}`.

| DFA state | NFA subset | on `a` | on `b` | accepting? |
|---|---|---|---|---|
| D0 | {q0} | `δ(q0,a)={q2,q3}` → **D1** | `δ(q0,b)=∅` → **D2** | no |
| D1 | {q2,q3} | `δ(q2,a)∪δ(q3,a)={q2,q3}∪∅` → **D1** (self) | `δ(q2,b)∪δ(q3,b)=∅∪{q4}` → **D3** | no |
| D2 | {} (dead) | ∅ → **D2** (self) | ∅ → **D2** (self) | no |
| D3 | {q4} | `δ(q4,a)=∅` → **D2** | `δ(q4,b)=∅` → **D2** | **yes** (contains q4) |

**Output:**
```
Total DFA states: 4
q0 = { q0 }        q1 = { q2 q3 }     q2 = { ∅ }        q3 = { q4 }
δ(q0,a)=q1   δ(q0,b)=q2
δ(q1,a)=q1   δ(q1,b)=q3
δ(q2,a)=q2   δ(q2,b)=q2
δ(q3,a)=q2   δ(q3,b)=q2
Accepting states: q3
```
(Program 3 numbers DFA states `q0..q3` in discovery order — these are *new*
state names, unrelated to the original NFA's `q0..q4`; the table above uses
`D0..D3` just to avoid confusing the two in this explanation.)

This correctly represents **a⁺b**: you must leave the start state via `a`
(a `b` straight from the start dead-ends), you can loop on `a` forever in
D1, and the *one* `b` that follows sends you to the only accepting state.

---

<a id="program-4"></a>
## Program 4 — `4-min-dfa.c`: minimize a DFA

**Problem statement:** *Write a program to minimize any given DFA.*

**Concept used:** §A.5 (table-filling / state equivalence).

### Algorithm
1. Read the DFA (transition table `delta[state][symbol]`, accepting set).
2. Initialize `dist[i][j] = 1` if `accepting[i] != accepting[j]` (base case).
3. Repeat: for every still-"equivalent" pair `(i,j)`, if *any* symbol `k`
   leads to a pair `(delta[i][k], delta[j][k])` already marked
   distinguishable, mark `(i,j)` distinguishable too. Stop when a full pass
   changes nothing.
4. Group states that stayed un-marked together — each group becomes one
   state in the minimized DFA (transitions/accepting copied from any one
   representative of the group, since they're all equivalent by
   definition).

### Annotated code
```c
int delta[MAX][MAX], accepting[MAX], dist[MAX][MAX], class[MAX];

/* ... read n, m, symbols, delta[][], accepting[] ... */

for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
        dist[i][j] = (accepting[i] != accepting[j]);   // base case

int changed;
do {
    changed = 0;
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if (!dist[i][j])                            // still "maybe equal"
                for (k = 0; k < m; k++)
                    if (dist[delta[i][k]][delta[j][k]]) {  // their next states already differ
                        dist[i][j] = dist[j][i] = 1;
                        changed = 1;
                        break;
                    }
} while (changed);                                       // fixpoint loop

int classCount = 0;
for (i = 0; i < n; i++) class[i] = -1;
for (i = 0; i < n; i++)
    if (class[i] == -1) {
        for (j = i; j < n; j++)
            if (!dist[i][j]) class[j] = classCount;      // group i with every
        classCount++;                                     // state not distinguishable from it
    }
```
**Callouts:**
- The `do { } while(changed)` loop is the fixpoint — table-filling always
  needs possibly multiple passes, because a pair can only be marked once
  the pairs *it depends on* are already marked, and dependencies can chain.
- `for (j = i + 1; ...)` and symmetric assignment `dist[i][j]=dist[j][i]=1`
  — the table is symmetric, so only the upper triangle is actually computed.
- The final grouping loop greedily assigns the *lowest unassigned index* as
  each new class's first member — that's why `class[rep]` searches (in the
  print section) for `rep` by scanning upward until it matches a class id.

### Dry run, part 1 — Example A's DFA (from Program 3) is already minimal
Feed in the 4-state DFA from Program 3 (`D0..D3`, accepting = `{D3}`):
```
delta: D0: a->D1,b->D2   D1: a->D1,b->D3   D2: a->D2,b->D2   D3: a->D2,b->D2
```
- Base case: `D3` (accepting) differs from `D0,D1,D2` → those 3 pairs marked
  distinguishable immediately.
- `(D1,D2)`: on `b`, `D1->D3` and `D2->D2`; pair `(D3,D2)` is already
  distinguishable → so `(D1,D2)` becomes distinguishable too.
- `(D0,D1)`: on `b`, `D0->D2` and `D1->D3`; pair `(D2,D3)` already
  distinguishable → `(D0,D1)` distinguishable.
- `(D0,D2)`: on `a`, `D0->D1` and `D2->D2`; pair `(D1,D2)` just got marked
  above → `(D0,D2)` distinguishable too.

**Result: all 6 pairs distinguishable → 4 classes → no states merge.** This
DFA for a⁺b is *already* minimal — a legitimate, useful outcome (don't
expect every DFA you're handed to shrink!).

### Dry run, part 2 — "Example B": a DFA that *does* shrink
Since Example A doesn't demonstrate an actual merge, here's a second,
tiny DFA purely to see classes actually combine (practice this one by hand
— it's the shape most exam questions use):
```
States: q0(start), q1, q2, q3(accepting)     Alphabet: {0, 1}
δ(q0,0)=q1   δ(q0,1)=q2
δ(q1,0)=q3   δ(q1,1)=q3
δ(q2,0)=q3   δ(q2,1)=q3
δ(q3,0)=q3   δ(q3,1)=q3
```
- Base case: `q3` (accepting) distinguishable from `q0,q1,q2`.
- `(q1,q2)`: on `0`, both go to `q3`; on `1`, both go to `q3` → **no
  distinguishing symbol found** → `(q1,q2)` stays equivalent.
- `(q0,q1)`: on `0`, `q0->q1`, `q1->q3`; pair `(q1,q3)` already
  distinguishable (base case) → `(q0,q1)` distinguishable.
- `(q0,q2)`: symmetric reasoning → distinguishable.

**Result: classes = `{q0}`, `{q1,q2}`, `{q3}` → 3 states instead of 4.**
```
Minimized DFA:  C0 --0,1--> C1 --0,1--> C2 --0,1--> C2 (self-loop)
Start: C0.  Accepting: C2.
```
`q1` and `q2` merge because, from either one, *every* future input leads to
exactly the same outcome — the table-filling algorithm never finds a string
that tells them apart.

---

**Next:** `02-cycle2-lexyacc.md` for programs 5–10 (uses Part B of
`00-foundations.md`).
