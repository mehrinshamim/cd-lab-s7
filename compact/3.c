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
