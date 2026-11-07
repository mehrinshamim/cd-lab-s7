// Q4: minimize a DFA (table-filling / marking distinguishable pairs).  Build: gcc 4.c -o 4
#include <stdio.h>
int n, m, i, j, k, ch, cc;
int d[10][10], f[10], x[10][10], cls[10];     // x[i][j] = 1 if states i and j are distinguishable
char sym[10];
int rep(int c) { int r = 0; while (cls[r] != c) r++; return r; }   // first state of class c

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter number of input symbols: "); scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (i = 0; i < m; i++) scanf(" %c", &sym[i]);
    printf("Enter transition table (row = state, col = symbol):\n");
    for (i = 0; i < n; i++) for (j = 0; j < m; j++) scanf("%d", &d[i][j]);
    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) f[i] = 1;

    for (i = 0; i < n; i++) for (j = 0; j < n; j++) x[i][j] = f[i] != f[j];   // accepting vs non-accepting
    do {                                                                       // repeat until no new pair is marked
        ch = 0;
        for (i = 0; i < n; i++) for (j = i + 1; j < n; j++) if (!x[i][j])
            for (k = 0; k < m; k++) if (x[d[i][k]][d[j][k]]) { x[i][j] = x[j][i] = ch = 1; break; }
    } while (ch);

    for (i = 0; i < n; i++) cls[i] = -1;                                       // unmarked pairs = same class
    for (i = 0; i < n; i++) if (cls[i] < 0) { for (j = i; j < n; j++) if (!x[i][j]) cls[j] = cc; cc++; }

    printf("\n--- Minimized DFA ---\nTotal States: %d\n\nTransitions:\n", cc);
    for (i = 0; i < cc; i++) for (k = 0; k < m; k++) printf("δ(q%d, %c) = q%d\n", i, sym[k], cls[d[rep(i)][k]]);
    printf("\nAccepting states: ");
    for (i = 0; i < cc; i++) if (f[rep(i)]) printf("q%d ", i);
    printf("\nStart state: q%d\n", cls[0]);
}
