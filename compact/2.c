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
