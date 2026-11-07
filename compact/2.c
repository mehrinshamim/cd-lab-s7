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
