# Viva Question Bank — Programs 1–10

Standalone revision sheet: flip through program-by-program right before the
exam. Each answer is intentionally short — if one doesn't fully make sense,
go back to the matching section named in `00-foundations.md` /
`01-cycle1-automata.md` / `02-cycle2-lexyacc.md`.

## Table of Contents

- [General theory (cross-cutting)](#general-theory)
- [Program 1 — ε-closure](#program-1)
- [Program 2 — eliminate ε-transitions](#program-2)
- [Program 3 — NFA to DFA (subset construction)](#program-3)
- [Program 4 — DFA minimization](#program-4)
- [Program 5 — hand-written lexical analyzer](#program-5)
- [Program 6 — substring rejection with Lex](#program-6)
- [Program 7 — validate a variable name (Yacc)](#program-7)
- [Program 8 — calculator (Lex + Yacc)](#program-8)
- [Program 9 — BNF → Yacc, build an AST](#program-9)
- [Program 10 — check C `for` statement syntax](#program-10)

---

<a id="general-theory"></a>
## General theory (cross-cutting — not tied to one program)

**Q: What's the real difference between an NFA and a DFA?**
A: A DFA has exactly one next state for every (state, symbol) pair; an NFA
can have zero, one, or many. An ε-NFA additionally allows free moves that
consume no input symbol.

**Q: Are NFAs and DFAs equally powerful (do they recognize the same class of languages)?**
A: Yes — every NFA (even with ε) can be converted to an equivalent DFA
(that's exactly what Programs 2+3 do), so both recognize exactly the
regular languages. DFAs just can't do it with fewer "live possibilities" at
once — they trade state-count for determinism.

**Q: Why would a DFA ever have more states than the NFA it came from?**
A: Because subset construction makes one DFA state per *reachable subset*
of NFA states. In the worst case, that's `2^n` states for `n` NFA states
(though real examples are usually far smaller, as our running example
shows: 5 NFA states → only 4 DFA states).

**Q: Why do compilers separate lexing from parsing instead of doing it all in one pass?**
A: Separation of concerns + efficiency. The lexer handles the simple,
regular-language-level pattern matching (tokens) very fast; the parser only
has to deal with a much smaller alphabet (token types, not characters) and
can focus purely on grammatical structure. It also means the same lexer
logic doesn't have to be duplicated across different possible grammars.

**Q: What's the difference between a lexer's "token" and "lexeme"?**
A: Lexeme = the actual matched text (e.g. `"total"`). Token = the category
it's classified as (e.g. `IDENTIFIER`).

**Q: Why does Lex always pick the longest possible match?**
A: So that, e.g., the identifier rule doesn't stop after matching just `f`
when the input is actually `for` — longest match is what lets a single
general rule (`[a-zA-Z]+`) coexist with more specific keyword handling
without ambiguity about *where* a token ends.

---

<a id="program-1"></a>
## Program 1 — ε-closure

**Q: Why does ε-closure of a state always include the state itself?**
A: Taking "zero" ε-moves is always valid — you're trivially still at the
state you started from even without moving.

**Q: Why is ε-closure computed with a graph traversal (DFS/BFS) instead of a single table lookup?**
A: Because ε-transitions can chain (q0→q1→q2 via two separate ε-edges), so
reachability isn't just "direct neighbors" — you must keep following
ε-edges transitively until no new states are found.

**Q: What happens if the ε-transition graph has a cycle?**
A: The `visited[]` array prevents infinite recursion — once a state is
marked visited, `dfs` won't revisit it, so a cycle just gets each state
visited exactly once, same as an acyclic graph.

**Q: Why is ε-closure computed separately for every state instead of once globally?**
A: Because ε-closure is a *per-state* set — `ECLOSE(q0)` and `ECLOSE(q1)`
can be (and usually are) different sets, so each needs its own traversal
and its own fresh `visited[]` array.

**Q: What's the time complexity of computing ε-closure for all `n` states?**
A: Each DFS is `O(n)` in the worst case (visiting every state once along
`n²` possible edges checked), run `n` times → `O(n³)` overall for this
straightforward approach (fine for small, exam-sized automata).

---

<a id="program-2"></a>
## Program 2 — Eliminate ε-transitions

**Q: What exactly does the resulting NFA (without ε) accept, compared to the original?**
A: The exact same language — eliminating ε doesn't change what's accepted,
it just removes the "free move" mechanism by baking its effect directly
into the regular transitions.

**Q: Walk through the formula used for a new transition.**
A: `δ'(q,a) = ⋃ ECLOSE(t)` for every `t` reachable by one real `a`-move from
any state `p` in `ECLOSE(q)`. In words: ε-close first, take one real step,
ε-close again.

**Q: Why can the resulting machine still be nondeterministic?**
A: Because a single state `q` can have several different `t`'s reachable
via that formula for the same symbol `a` — eliminating ε doesn't force
determinism, it only removes free moves. (Example A shows this: `q0` on
`a` still goes to *two* states, `{q2,q3}`.)

**Q: When is a state accepting in the new machine?**
A: When its ε-closure contains any originally-accepting state — i.e., you
could reach an accepting state "for free," without consuming any input.

**Q: Why does this program call `computeClosure()` once up front instead of recomputing closures inline?**
A: The closure table is reused many times (once per state × per symbol ×
per candidate target) — computing it once avoids massive redundant DFS
calls.

---

<a id="program-3"></a>
## Program 3 — NFA to DFA (subset construction)

**Q: What does a single DFA state actually represent?**
A: A *set* of NFA states that could all be "active" simultaneously after
reading the same input prefix.

**Q: Why is the algorithm structured as a worklist/BFS instead of computing all `2^n` subsets up front?**
A: Most of the `2^n` possible subsets are never actually reachable from the
start state on any real input — the worklist only builds states that are
actually needed, which is why real-world DFAs are usually much smaller
than the `2^n` theoretical bound.

**Q: How do you decide if a new DFA state is "the same" as one already built?**
A: Compare bitsets member-by-member (`isSameState`) — two DFA states are
identical iff they represent the exact same *set* of NFA states, regardless
of how that set was reached.

**Q: How is a DFA state marked accepting?**
A: If the NFA-state subset it represents contains **any** NFA accepting
state.

**Q: In the running example, why does the DFA end up with only 4 states instead of up to `2^5=32`?**
A: Because from the start, only 4 distinct subsets are ever actually
reachable by any sequence of real moves — all other theoretical subsets
(like `{q0,q4}`) are simply never produced by the `move()` computation, so
they're never added.

**Q: What does a transition to the "dead"/empty state mean?**
A: That no continuation of the input can ever lead to acceptance from here
— once you're in it, every further symbol keeps you there and you reject.

---

<a id="program-4"></a>
## Program 4 — DFA minimization

**Q: What does it mean for two states to be "equivalent"?**
A: For every possible remaining input string, starting from either state
gives the same accept/reject answer.

**Q: Why does the base case of the table-filling algorithm compare accepting status?**
A: Because the empty string alone already distinguishes an accepting state
from a non-accepting one — that's the simplest possible distinguishing
string.

**Q: Explain the induction step in your own words.**
A: If reading some symbol `a` from `p` and from `q` lands you in two states
already known to be distinguishable, then `p` and `q` are distinguishable
too — just read `a` first, then whatever string told the two destinations
apart.

**Q: Why is this a `do { } while(changed)` loop instead of a fixed number of passes?**
A: Because marking one pair distinguishable can be exactly what's needed to
newly justify marking a *different* pair — dependencies chain, so you must
keep looping until a full pass makes zero new marks (a fixpoint), not stop
after one or two passes.

**Q: Is it possible for a DFA to already be minimal (no states merge)?**
A: Yes — see the running example (`a⁺b`'s DFA): all 4 states turn out
pairwise distinguishable, so minimization changes nothing. Compare "Example
B" in the same file, where `q1` and `q2` genuinely merge.

**Q: After merging, how do you build the minimized DFA's transitions?**
A: Pick any one representative state from each equivalence class; its
transitions (mapped through the same class-grouping) define the new DFA's
transitions, since every member of the class is guaranteed to transition to
equivalent destinations anyway.

---

<a id="program-5"></a>
## Program 5 — Hand-written lexical analyzer

**Q: Why must you push back (`fseek(..., -1, SEEK_CUR)`) a character after reading an identifier/number?**
A: The inner loop has to read one character *past* the end of the token
just to discover that it no longer matches — that extra character still
belongs to the *next* token and must be returned to the stream, or it gets
lost.

**Q: Why are whitespace characters skipped with `continue` rather than being emitted as their own token type?**
A: They carry no meaning for later stages (parsing) — the syllabus
explicitly asks for them to be ignored, same as a real compiler's lexer.

**Q: How does this program decide "keyword" vs "identifier" if both start with a letter?**
A: Both are first read into the same buffer with the same loop (anything
alphanumeric/underscore); only afterward is the complete word checked
against a fixed keyword list — if it matches, it's a keyword, otherwise an
identifier.

**Q: What's a limitation of this hand-written lexer compared to a generated one (Lex)?**
A: It has no automatic longest-match/priority resolution across many rules
or regular-expression support — every category has to be hand-coded with
its own loop and character-class check, which doesn't scale to a language
with many token kinds.

---

<a id="program-6"></a>
## Program 6 — Substring rejection with Lex

**Q: Why does rule order matter in this specific program?**
A: Both rules can match the entire input line with equal length when the
forbidden substring is present, so Lex's tie-break rule ("first-written rule
wins on equal-length match") decides the outcome — the reject rule must
come first.

**Q: Would swapping the two rules' order break the program? How?**
A: Yes — if the plain `.*` rule were listed first, it would *always* win
ties, so the reject rule would never fire (the forbidden-substring case
would always print "Accepted" incorrectly).

**Q: Does this program check for the substring at a fixed position, or anywhere in the line?**
A: Anywhere — the `.*` before and after `[mM][eE][hH][rR]` let any number of
characters (including zero) appear before and after the match.

**Q: Is the match case-sensitive?**
A: No — each letter is given as a character class with both cases, e.g.
`[mM]`, so `MEHR`, `mehr`, `MeHr`, etc. are all detected.

---

<a id="program-7"></a>
## Program 7 — Validate a variable name (Yacc)

**Q: What makes a string a "valid variable" per this grammar?**
A: It must lex as a single `VARIABLE` token (one letter, then any number of
letters/digits) followed by a newline, and nothing else.

**Q: What happens if the input is just digits, like `123`?**
A: The lexer's letter-first rule doesn't match, so the catch-all `. {
return yytext[0]; }` rule returns the raw digit character as its own
token — which the grammar's `line` rule can't match (it only accepts
`VARIABLE` or `'\n'`), so `yyerror` fires: "Invalid Variable."

**Q: Why does the lexer have a separate rule for `\n` instead of letting the catch-all handle it too?**
A: Because the grammar needs to *recognize* `'\n'` specifically as end-of-
line (it appears explicitly in the `line` rule) — if it fell through to the
generic `.` rule it would still return the same character code, but writing
it explicitly makes the intent clear and avoids relying on fallthrough
behavior for something structurally important.

**Q: Where does the "Invalid Variable" message actually get printed?**
A: Inside `yyerror()`, which Yacc's generated parser automatically calls
whenever it can't match the current token against any grammar rule.

---

<a id="program-8"></a>
## Program 8 — Calculator (Lex + Yacc)

**Q: Why is `%left`/`%right` needed here at all — isn't the grammar already well-defined?**
A: The grammar `expr: expr '+' expr | expr '*' expr | ...` is ambiguous by
itself (it doesn't say which operator groups first) — `%left`/`%right`
resolve the shift-reduce conflicts that ambiguity creates, by telling Yacc
which operator has higher precedence and which direction to associate.

**Q: Walk through why `-2*3` doesn't accidentally parse as `-(2*3)`.**
A: The rule `'-' expr %prec UMINUS` is given `UMINUS`'s very high
precedence via `%prec`, overriding what it would otherwise inherit from the
literal `'-'` token (which has low precedence) — so unary minus binds to
just `2`, producing `(-2)*3`.

**Q: Why is division-by-zero handled inside the action code instead of as a grammar rule?**
A: `expr '/' expr` is syntactically valid no matter what the operands'
*values* turn out to be — you can't know at parse time whether the divisor
will evaluate to zero, so it has to be checked when the division actually
executes.

**Q: How does a number's actual value get from the lexer to the parser?**
A: The lexer's rule sets the global `yylval = atoi(yytext)` before
returning `NUMBER`; Yacc automatically makes that value available to the
grammar action as `$1` (or whichever position the `NUMBER` token occupies).

**Q: What would happen if `%left '*' '/'` were listed *before* `%left '+' '-'` instead of after?**
A: Precedence is lowest-to-highest in declaration order, so swapping them
would make `+`/`-` bind *tighter* than `*`/`/` — `2+3*4` would then
(incorrectly) evaluate as `(2+3)*4 = 20` instead of `2+(3*4) = 14`.

---

<a id="program-9"></a>
## Program 9 — BNF → Yacc, build an AST

**Q: Why does this grammar need a `%union` when Program 8 didn't?**
A: Program 8's tokens/nonterminals are all plain numbers (one type fits
everything). Program 9 needs to carry two different kinds of values — raw
identifier/number text (`char*`) from the lexer, and built tree-node
pointers (`struct node*`) from the parser — so each needs its own typed
field in a union.

**Q: In general, how do you build an AST node for a grammar rule like `E: E '+' T`?**
A: Create a new node whose token/label is the operator (`"+"`), and whose
left and right children are the *already-built* subtrees for `$1` and
`$3` — `mknode("+", $1, $3)`.

**Q: Why are `ID` and `NUM` both turned into leaf nodes (`mknode($1, NULL, NULL)`) instead of being used directly?**
A: So the tree has a uniform node structure everywhere — every operand,
whether an identifier or a literal number, becomes a leaf node with no
children, letting the same `print_postorder` function walk the whole tree
without needing to special-case terminals.

**Q: What traversal order does `print_postorder` use, and why does that matter here?**
A: Left subtree, then right subtree, then the node's own token — this
produces exactly the postfix notation for the expression (operators appear
*after* their operands), which is also the order a stack-based evaluator
would want to execute operations in.

**Q: Why is the grammar split into `E`, `T`, `F` layers instead of one flat `expr` rule?**
A: This layering encodes operator precedence structurally — `T` (handling
`*`/`/`) sits "inside" `E` (handling `+`/`-`), so lower-precedence operators
always end up at a higher (outer) level of the tree automatically, without
needing `%left`/`%right` declarations at all.

---

<a id="program-10"></a>
## Program 10 — Check C `for` statement syntax

**Q: Why does nearly every clause (`initialization`, `condition`, `increment`) have an empty alternative?**
A: Because real C allows a `for` loop to omit any of the three clauses
(classic example: `for(;;)` is valid, infinite-loop C) — the grammar has to
accept the empty string for each to match that.

**Q: How does this grammar support nested `for` loops without any special handling?**
A: `statement` includes `for_stmt` as one of its alternatives, and
`for_stmt`'s `body` can contain a `statement_list` built from `statement`s
— so `for_stmt` is reachable from inside its own `body`, making the
grammar naturally recursive.

**Q: This program never computes a value anywhere — why not?**
A: Because the task is purely to check *syntax* (shape), not to evaluate or
build anything from the `for` loop's contents — contrast with Program 8
(computes a value) and Program 9 (builds a tree).

**Q: What happens mechanically when the input doesn't match the grammar?**
A: Yacc's generated `yyparse()` reaches a point where the next token can't
extend any valid parse under the grammar's rules, and calls `yyerror()`,
which here prints "Invalid FOR statement syntax."

**Q: Why are `relop`, like `<`, `LE`, `GE`, grouped into their own nonterminal instead of listing each option directly inside `condition`?**
A: Readability and reuse — it isolates "what counts as a comparison
operator" as its own concept, which would also make it easy to reuse
`relop` elsewhere in a larger grammar (e.g., inside `if` statements) without
repeating the list.
