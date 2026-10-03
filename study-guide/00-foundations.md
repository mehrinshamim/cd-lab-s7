# 00 — Foundations (read this first)

This file is the "ground floor." Every one of the 10 lab programs is a small
application of one or two ideas explained here. Read this once, then when you
read `01-cycle1-automata.md` and `02-cycle2-lexyacc.md`, each program will
explicitly say *"this uses the idea from §A.3"* etc. so you can always trace
code back to theory.

- **Part A** underlies programs 1–4 (automata).
- **Part B** underlies programs 5–10 (lex/yacc).

A single running example automaton ("Example A") is defined at the end of
Part A and then reused, unchanged, all the way through programs 1, 2, 3 and 4
in the next file — so you watch *one* automaton get transformed step by step
instead of memorizing four unrelated examples.

---

## Table of Contents

- [Part A — Automata theory (for programs 1–4)](#part-a)
  - [A.1 What a finite automaton is](#a1)
  - [A.2 NFA vs DFA vs ε-NFA — and why it matters](#a2)
  - [A.3 ε-closure — the idea used in Programs 1, 2 and 3](#a3)
  - [A.4 Subset (powerset) construction — the idea used in Programs 2 and 3](#a4)
  - [A.5 State equivalence and minimization — the idea used in Program 4](#a5)
  - [A.6 The running example: "Example A"](#a6)
- [Part B — Lex & Yacc foundations (for programs 5–10)](#part-b)
  - [B.1 Where lexing and parsing fit in a compiler](#b1)
  - [B.2 Tokens vs lexemes, and the longest-match rule](#b2)
  - [B.3 Anatomy of a Lex (`.l`) file](#b3)
  - [B.4 Anatomy of a Yacc/Bison (`.y`) file](#b4)
  - [B.5 How a `.l` and `.y` file are wired together](#b5)
  - [B.6 Semantic values: `$$`, `$1`, `%union`, `%type`](#b6)
  - [B.7 Precedence and associativity: `%left`, `%right`, `%prec`](#b7)
  - [B.8 Compile & run — cheat sheet](#b8)

---

<a id="part-a"></a>
## Part A — Automata theory (for programs 1–4)

<a id="a1"></a>
### A.1 What a finite automaton is

A finite automaton is a simple machine for recognizing strings. It is defined
by 5 things (the "5-tuple"):

| Symbol | Meaning |
|---|---|
| `Q` | finite set of states, e.g. {q0, q1, q2} |
| `Σ` (sigma) | input alphabet, e.g. {a, b} |
| `δ` (delta) | transition function — given a state and a symbol, where do you go? |
| `q0` | the start state |
| `F` | the set of accepting (final) states, `F ⊆ Q` |

You feed it a string one symbol at a time. Starting at `q0`, each symbol
moves you along `δ`. If you end in a state that's in `F` after consuming the
whole string, the string is **accepted**; otherwise it's **rejected**.

All four Cycle I programs just read in a description of one of these
machines (as matrices) and compute something about it.

<a id="a2"></a>
### A.2 NFA vs DFA vs ε-NFA — and why it matters

- **DFA (Deterministic Finite Automaton):** for every state and every symbol,
  there is **exactly one** next state. `δ(state, symbol)` is a single value.
  This is easy for a computer to execute directly — just follow the arrows.
- **NFA (Nondeterministic Finite Automaton):** a state can have **zero,
  one, or many** next states for the same symbol. `δ(state, symbol)` is a
  *set* of states. You can think of it as the machine "guessing" or trying
  all options at once.
- **ε-NFA (NFA with epsilon-transitions):** on top of being nondeterministic,
  the machine can also move to another state **without consuming any input
  symbol at all** — a "free" move, written `δ(state, ε)`.

Why three versions of basically the same idea? Because:
- ε-NFAs are the easiest to **build** from a regular expression (this is
  literally how `grep`/lex-style tools work internally — Thompson
  construction).
- DFAs are the easiest and fastest to **run** (no guessing, no backtracking).

So the whole point of Cycle I is the *pipeline* that converts the
easy-to-build machine into the easy-to-run machine:

```
ε-NFA  --(Program 1: understand ε-closure)-->
ε-NFA  --(Program 2: eliminate ε)--> plain NFA
NFA    --(Program 3: subset construction)--> DFA
DFA    --(Program 4: minimize)--> smallest equivalent DFA
```

Program 1 doesn't actually transform anything — it just computes the one
building block (ε-closure) that programs 2 and 3 both depend on. That's why
it comes first.

<a id="a3"></a>
### A.3 ε-closure — the idea used in Programs 1, 2 and 3

**Definition:** `ECLOSE(q)` = the set of *all* states reachable from `q`
using **only** ε-transitions (zero or more of them), **including `q` itself**.

Why include `q` itself? Because "zero ε-moves" is always a valid option —
you're always already at the state you started at, even if you take no free
moves at all.

**Why it's not just "one hop":** ε-transitions chain. If `q0 -ε-> q1` and
`q1 -ε-> q2`, then `ECLOSE(q0) = {q0, q1, q2}`, not just `{q0, q1}`. This is
exactly why the algorithm is a **graph traversal (DFS or BFS)** over the
ε-transitions, not a single lookup — you have to keep following ε-edges
until you stop finding new states.

**The algorithm (what Program 1 implements):**
```
ECLOSE(q):
    visited = {}
    DFS(q):
        mark q as visited
        for every state j such that there's an ε-edge q -> j:
            if j not visited:
                DFS(j)
    return visited
```
That's it — a textbook graph reachability DFS, just restricted to ε-edges.

<a id="a4"></a>
### A.4 Subset (powerset) construction — the idea used in Programs 2 and 3

This is the single most important idea in Cycle I, and it is reused **twice**:

- In **Program 2**, it's used to figure out, for a *single NFA state* `q`,
  what set of states you could be in after reading one real symbol —
  accounting for ε-moves before and after.
- In **Program 3**, it's used to build an entire **new DFA** whose states
  *are* sets of NFA states.

**The core trick:** an NFA can be in several states "at once" (because of
nondeterminism). A DFA can't — it needs one state. So: make *each DFA state
represent a set of NFA states that could all be active simultaneously*.

**General subset-construction algorithm (Program 3):**
1. The DFA's start state = `{NFA start state}` (after taking ε-closure, if
   ε-transitions are still present — in our programs, Program 2 already
   removed ε, so Program 3 starts clean with just `{q0}`).
2. For the current DFA state (a *set* `S` of NFA states) and each symbol `a`:
   compute `move(S, a)` = the union, over every NFA state `p` in `S`, of
   `δ(p, a)`. This union *is* the new DFA state.
3. If this resulting set hasn't been seen before, it's a brand-new DFA
   state — add it to a worklist.
4. Repeat until no new sets appear.
5. A DFA state (set `S`) is accepting if `S` contains *any* NFA accepting
   state.

Because a set of `n` NFA states can be any of `2^n` possible subsets, a DFA
built this way can — in the worst case — have exponentially more states than
the NFA it came from. (In practice it's usually far fewer, as you'll see in
the running example.)

**Program 2 vs Program 3 — same trick, different output shape:**

| | Program 2 | Program 3 |
|---|---|---|
| Input | ε-NFA | NFA (no ε) |
| What `move`/closure is applied to | every *single* existing state, one at a time | the *current frontier set*, growing the state list as you go |
| Output | another NFA (still possibly nondeterministic — a state may have several next states for one symbol) | a DFA (every state has exactly one next state per symbol, by construction) |

That's the key distinction to be able to say out loud in a viva: **Program 2
removes ε but can still leave nondeterminism behind; Program 3 removes
*all* remaining nondeterminism by making whole subsets into single states.**

<a id="a5"></a>
### A.5 State equivalence and minimization — the idea used in Program 4

Two DFA states `p` and `q` are **equivalent** (indistinguishable) if, for
*every possible input string*, starting from `p` and starting from `q` leads
to the same accept/reject answer. If two states are equivalent, you can
merge them into one state without changing what language the DFA accepts.

**How do you check this without trying every string (there are infinitely
many)?** With the **table-filling algorithm**, which finds *distinguishing*
pairs instead:

1. **Base case:** any pair `(p, q)` where one is accepting and the other
   isn't is immediately **distinguishable** (the empty string already tells
   them apart).
2. **Induction step:** a pair `(p, q)` is distinguishable if there exists
   some symbol `a` such that `(δ(p,a), δ(q,a))` is *already known* to be
   distinguishable. (Intuition: if reading `a` lands you in two states that
   can already be told apart, then `p` and `q` could already be told apart
   too, just by reading `a` first.)
3. Repeat step 2 until a full pass makes no new marks (a **fixpoint**).
4. Whatever pairs are *never* marked distinguishable are equivalent — merge
   each such group into one representative state.

This is exactly what `4-min-dfa.c`'s `dist[i][j]` matrix and `do { ... }
while (changed)` loop compute.

<a id="a6"></a>
### A.6 The running example: "Example A"

This one ε-NFA is used, unchanged, in every dry run in the next file. It
recognizes the language **a⁺b** — one or more `a`'s followed by exactly one
`b` (e.g. `ab`, `aab`, `aaab`... but not `b` alone or `ab` followed by
anything else).

**States:** `q0` (start), `q1`, `q2`, `q3`, `q4` (accepting)
**Alphabet:** `{a, b}`

**ε-transitions:**
```
q0 --ε--> q1
q1 --ε--> q2
```
(a chain of two ε-moves — deliberately, so ε-closure isn't a trivial
one-hop lookup)

**Real transitions:**
```
q2 --a--> q2     (self-loop: "keep reading more a's")
q2 --a--> q3     (nondeterministic branch: "or, decide this was the last a")
q3 --b--> q4     (the single b that ends the string)
```

In matrix form (what you'd actually type into the programs):

*Epsilon matrix `eps[i][j]` (5×5):*
```
     q0 q1 q2 q3 q4
q0 [  0  1  0  0  0 ]
q1 [  0  0  1  0  0 ]
q2 [  0  0  0  0  0 ]
q3 [  0  0  0  0  0 ]
q4 [  0  0  0  0  0 ]
```

*Transition matrix for `a` (`trans[i][j][0]`):*
```
     q0 q1 q2 q3 q4
q0 [  0  0  0  0  0 ]
q1 [  0  0  0  0  0 ]
q2 [  0  0  1  1  0 ]
q3 [  0  0  0  0  0 ]
q4 [  0  0  0  0  0 ]
```

*Transition matrix for `b` (`trans[i][j][1]`):*
```
     q0 q1 q2 q3 q4
q0 [  0  0  0  0  0 ]
q1 [  0  0  0  0  0 ]
q2 [  0  0  0  0  0 ]
q3 [  0  0  0  0  1 ]
q4 [  0  0  0  0  0 ]
```

**Accepting states:** `{q4}`

Keep this page bookmarked — every dry run in `01-cycle1-automata.md` refers
back to these exact matrices.

---

<a id="part-b"></a>
## Part B — Lex & Yacc foundations (for programs 5–10)

<a id="b1"></a>
### B.1 Where lexing and parsing fit in a compiler

A compiler (or any tool that reads structured text — including a calculator
or a C-syntax checker) is traditionally split into stages:

```
raw characters  -->  [ LEXER ]  -->  tokens  -->  [ PARSER ]  -->  parse tree / action
   "x = 1+2"          (scans          ID '=' NUM '+' NUM     (checks the tokens
                       char by char)                          form valid grammar,
                                                                runs actions)
```

- The **lexer** (lexical analyzer) groups raw characters into meaningful
  chunks called **tokens** (identifiers, numbers, keywords, operators...),
  throwing away things that don't matter like whitespace and comments.
- The **parser** takes the stream of tokens and checks whether they form a
  structure that matches a **grammar** — and runs code (semantic actions)
  when pieces of that structure are recognized.

Program 5 is a lexer written *by hand* in plain C (no tool), so you can see
exactly what a lexer has to do under the hood. Programs 6–10 use **Lex**
(the lexer-generator tool) and, from program 7 onward, **Yacc** (the
parser-generator tool) layered on top.

<a id="b2"></a>
### B.2 Tokens vs lexemes, and the longest-match rule

- A **lexeme** is the actual chunk of text matched, e.g. `"foo123"`.
- A **token** is the *category* it belongs to, e.g. `IDENTIFIER`.

When several of a lexer's rules could match at the current position (e.g.
the input `"int"` matches both the identifier rule `[a-zA-Z]+` **and** could
be treated as the keyword `"int"`), lexers resolve this with two standard
rules, both of which Lex follows automatically:
1. **Longest match wins** — if `for` could match `f` or `for`, Lex always
   tries to consume as many characters as possible before stopping.
2. **Earlier rule wins on a tie** — if two rules match the *same* length,
   the one written first in the `.l` file wins.

This is why in `5-lex.c` (the hand-written version) the code explicitly
keeps reading characters (`isalnum(ch)`) until the identifier *stops*,
instead of grabbing one character at a time — that loop *is* the
hand-written version of "longest match."

<a id="b3"></a>
### B.3 Anatomy of a Lex (`.l`) file

```
%{
   /* C declarations, #includes — copied verbatim into the generated file */
%}

%%
regex1   { action1 in C; }
regex2   { action2 in C; }
...
%%

/* extra C functions, e.g. main(), yywrap() */
```

- Running `flex file.l` generates `lex.yy.c`, a complete C source file
  containing a function `int yylex(void)`.
- Every call to `yylex()` scans forward from where it left off, finds the
  next token, runs that rule's action, and (if the action contains
  `return SOMETHING;`) returns that token to whoever called it.
- `yytext` is a global string holding the exact lexeme just matched.
- `yywrap()` is called when the lexer hits end-of-input; returning `1`
  tells it "no more input, stop" (we always return 1 in these labs since
  we're not chaining multiple input files).

<a id="b4"></a>
### B.4 Anatomy of a Yacc/Bison (`.y`) file

```
%{
   /* C declarations */
%}

%token NAME1 NAME2 ...      /* names of tokens coming from the lexer */

%%
grammar_rule:
      alternative_1          { action }
    | alternative_2          { action }
    ;
...
%%

/* yyerror(), main() */
```

- Running `bison -d file.y` (the `-d` is important) generates **two**
  files: `y.tab.c` (the actual parser, a function `int yyparse(void)`) and
  `y.tab.h` (the token names, as `#define`s/an enum — this is how the `.l`
  file's `return NUMBER;` and the `.y` file's `%token NUMBER` refer to the
  *same* integer).
- A grammar rule like `expr: expr '+' expr` describes a valid shape; the
  `{ action }` after it is ordinary C code that runs *when that shape is
  recognized* (i.e., on a successful **reduction** — see B.6).
- `yyparse()` repeatedly calls `yylex()` to pull the next token and tries to
  match the grammar. If the tokens never fit any rule, it calls `yyerror()`.

<a id="b5"></a>
### B.5 How a `.l` and `.y` file are wired together

This is the part that confuses people most, so slowly:

1. The `.y` file declares token names: `%token NUMBER`.
2. `bison -d` turns that into `y.tab.h`, roughly:
   `#define NUMBER 258` (an arbitrary integer, doesn't matter which).
3. The `.l` file does `#include "y.tab.h"` in its `%{ %}` block — so now it
   knows what `NUMBER` means as an integer.
4. In the `.l` file, a rule says `[0-9]+ { return NUMBER; }` — when digits
   are seen, the lexer's `yylex()` returns that integer code back to
   whoever called it (which is `yyparse()`).
5. `yyparse()` (generated from the `.y` file) calls `yylex()` in a loop,
   gets back a stream of these integer codes, and matches them against the
   grammar.
6. If a token needs to carry a **value** (not just "this was a number", but
   *which* number) that value is put in the global `yylval` by the lexer
   action, *before* returning the token — e.g.
   `{ yylval = atoi(yytext); return NUMBER; }`. The parser then accesses it
   as `$1`, `$2`, etc. (see B.6).

So the compile sequence is always, in order:
```
bison -d calc.y      # makes y.tab.c AND y.tab.h  (make this first!)
flex calc.l           # makes lex.yy.c             (needs y.tab.h to exist)
gcc lex.yy.c y.tab.c -lfl -o calc
```
**Order matters**: `y.tab.h` must exist *before* you compile/generate the
lexer, because the lexer's C code `#include`s it.

<a id="b6"></a>
### B.6 Semantic values: `$$`, `$1`, `%union`, `%type`

Inside a grammar rule's action, each symbol on the right-hand side is
referred to by position:

```
expr: expr '+' expr   { $$ = $1 + $3; }
   /*  $1    $2  $3  */
```

- `$1`, `$2`, `$3`... refer to the values of the 1st, 2nd, 3rd symbol on the
  right of the `:`. (`$2` here is the literal `'+'` token, unused.)
- `$$` is the value *this* rule produces — it becomes `$1`/`$2`/etc. the
  next time this nonterminal appears inside a bigger rule. Values flow
  **up** from the smallest pieces of the grammar to the biggest.
- By default these values are all just `int`. If you need to pass around
  something else — a `char*` string, or a pointer to a tree node — you
  declare a `%union` with one field per type you need:
  ```
  %union { char *sval; struct node *nval; }
  %token <sval> ID NUM        /* ID and NUM tokens carry a string */
  %type  <nval> S E T F       /* these grammar nonterminals carry a tree node pointer */
  ```
  Now `yylval` (set in the `.l` file) and `$1`/`$$` (used in the `.y` file)
  are typed, so `$1` for an `ID` token is a `char*`, while `$1` for an `E`
  nonterminal is a `struct node*`. This is exactly what Program 9 (AST)
  uses, because it needs to pass tree-node pointers around, not just
  numbers.

<a id="b7"></a>
### B.7 Precedence and associativity: `%left`, `%right`, `%prec`

A grammar like
```
expr: expr '+' expr | expr '*' expr | NUMBER ;
```
is **ambiguous** on its own — given `2 + 3 * 4`, should the parser group it
as `(2+3)*4` or `2+(3*4)`? Internally, Yacc builds a bottom-up (LALR) parser
that at each step decides to either **shift** (read one more token) or
**reduce** (collapse matched symbols using a rule) — this ambiguity shows
up as a **shift-reduce conflict**: having just read `2 + 3` and seeing `*`
next, should it reduce `2+3` to `expr` now, or shift the `*` and wait?

You resolve this by declaring precedence, from lowest to highest:
```
%left '+' '-'
%left '*' '/'
%right UMINUS
```
- Operators declared **later** bind **tighter** (higher precedence) — that's
  why `* /` is listed after `+ -`: multiplication binds before addition.
- `%left` means left-associative (`2-3-4` = `(2-3)-4`); `%right` means
  right-associative.
- `UMINUS` is a fake token (never actually returned by the lexer) used
  purely to give unary minus (`-x`) its *own*, very high precedence via
  `%prec UMINUS` on the rule `'-' expr %prec UMINUS`, so `-2*3` is parsed
  as `(-2)*3`, not `-(2*3)` by accident of rule order.

<a id="b8"></a>
### B.8 Compile & run — cheat sheet

```bash
# Pure lex, no yacc (e.g. programs 6):
flex 6-substr.l
gcc lex.yy.c -lfl -o 6-substr
./6-substr

# Lex + Yacc together (programs 7, 8, 9, 10):
bison -d 7-variable.y      # -> y.tab.c, y.tab.h  (do this FIRST)
flex 7-variable.l          # -> lex.yy.c           (needs y.tab.h)
gcc lex.yy.c y.tab.c -lfl -o variable
./variable

# Plain C, no lex/yacc (programs 1-5):
gcc 1-e-closure.c -o e-closure
./e-closure
```
`-lfl` links the Flex runtime library (provides a default `main`/helpers if
you don't write your own — harmless to include even when you do).

---

**Next:** open `01-cycle1-automata.md` for programs 1–4 (uses Part A above),
then `02-cycle2-lexyacc.md` for programs 5–10 (uses Part B above). Viva
questions for all 10 are consolidated separately in `03-viva-questions.md`.
