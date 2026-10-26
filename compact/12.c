#include <stdio.h>
#include <string.h>
#include <ctype.h>
char p[20][20], F[26][30], W[26][30], t[30];   // productions, FIRST sets, FOLLOW sets, temp
int n, i, j, ch = 1;                           // ch = "did anything change?"

int add(char *s, char c) {                     // add c to set s if absent; 1 if it changed
    if (strchr(s, c)) return 0;
    int l = strlen(s); s[l] = c; s[l + 1] = 0; return 1;
}

void fst(char *x, char *r) {                   // FIRST of a string x → r
    r[0] = 0;
    for (; *x; x++) {
        if (!isupper(*x)) { add(r, *x); return; }          // terminal or '#': add and stop
        int eps = 0;
        for (char *c = F[*x - 'A']; *c; c++)               // copy FIRST(X) without #
            if (*c == '#') eps = 1; else add(r, *c);
        if (!eps) return;                                  // X can't vanish → stop
    }
    add(r, '#');                                           // every symbol could vanish
}

int main() {
    char *c;
    printf("Enter number of productions: "); scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%s", p[i]);             // format E->TR   (# = epsilon)

    while (ch) {                                           // ---- FIRST ----
        ch = 0;
        for (i = 0; i < n; i++) {
            fst(p[i] + 3, t);                              // p[i]+3 skips "E->"
            for (c = t; *c; c++) ch |= add(F[p[i][0] - 'A'], *c);
        }
    }

    add(W[p[0][0] - 'A'], '$'); ch = 1;                    // ---- FOLLOW ---- rule 1
    while (ch) {
        ch = 0;
        for (i = 0; i < n; i++)
            for (j = 3; p[i][j]; j++) {
                if (!isupper(p[i][j])) continue;           // only non-terminals
                int B = p[i][j] - 'A';
                fst(p[i] + j + 1, t);                      // FIRST(beta). Empty beta gives {#}
                for (c = t; *c; c++)                       // rule 2
                    if (*c != '#') ch |= add(W[B], *c);
                if (strchr(t, '#'))                        // rule 3: beta nullable or B is last
                    for (c = W[p[i][0] - 'A']; *c; c++) ch |= add(W[B], *c);
            }
    }

    for (i = 0; i < 26; i++) if (F[i][0]) printf("FIRST(%c)  = { %s }\n", 'A' + i, F[i]);
    for (i = 0; i < 26; i++) if (W[i][0]) printf("FOLLOW(%c) = { %s }\n", 'A' + i, W[i]);
}