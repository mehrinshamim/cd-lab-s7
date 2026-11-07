// Q3: NFA -> DFA (subset construction).  Build: gcc 3.c -o 3
// A set of NFA states is stored as a bitmask: bit j set  <=>  q_j is in the set.
#include <stdio.h>
int n, m, i, j, k, x, fin;                    // fin = mask of NFA accepting states
int tr[10][10];                               // tr[i][k] = mask of states reached from q_i on symbol k
int dm[1024], dt[1024][10], cnt = 1;          // dm[s] = NFA-set of DFA state s, dt = DFA transitions
char sym[10];

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter number of input symbols: "); scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (k = 0; k < m; k++) scanf(" %c", &sym[k]);
    printf("Enter transition matrices (0/1) for each symbol:\n");
    for (k = 0; k < m; k++) {
        printf("Symbol %c:\n", sym[k]);
        for (i = 0; i < n; i++) for (j = 0; j < n; j++) { scanf("%d", &x); if (x) tr[i][k] |= 1 << j; }
    }
    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) fin |= 1 << i;

    dm[0] = 1;                                // DFA state 0 = { q0 }
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
        for (j = 0; j < n; j++) if (dm[i] >> j & 1) printf("q%d ", j);
        printf(dm[i] ? "}\n" : "∅ }\n");
    }
    printf("\nTransitions:\n");
    for (i = 0; i < cnt; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], dt[i][k]);
    printf("\nAccepting states: ");
    for (i = 0; i < cnt; i++) if (dm[i] & fin) printf("q%d ", i);
    puts("");
}
