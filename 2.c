
#include <stdio.h>

#define MAX 10

int n, m;
char symbols[MAX];
int eps[MAX][MAX];
int trans[MAX][MAX][MAX];
int closure[MAX][MAX];
int newTrans[MAX][MAX][MAX];
int accepting[MAX];
int newAccepting[MAX];

void dfs(int state, int visited[]) {
    visited[state] = 1;
    for (int j = 0; j < n; j++) {
        if (eps[state][j] && !visited[j]) {
            dfs(j, visited);
        }
    }
}

void computeClosure() {
    for (int i = 0; i < n; i++) {
        int visited[MAX] = {0};
        dfs(i, visited);
        for (int j = 0; j < n; j++) {
            closure[i][j] = visited[j];
        }
    }
}

int main() {
    int i, j, k;

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols (excluding ε): ");
    scanf("%d", &m);
    printf("Enter the symbols (space separated): ");
    for (i = 0; i < m; i++) {
        scanf(" %c", &symbols[i]);
    }

    printf("Enter epsilon transition matrix (%d x %d):\n", n, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &eps[i][j]);
        }
    }

    printf("Enter transition matrices for each symbol (0/1):\n");
    for (k = 0; k < m; k++) {
        printf("Symbol %c:\n", symbols[k]);
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                scanf("%d", &trans[i][j][k]);
            }
        }
    }

    printf("Enter accepting states (enter -1 to stop): ");
    for (i = 0; i < n; i++) accepting[i] = 0;
    int state;
    while (1) {
        scanf("%d", &state);
        if (state == -1) break;
        if (state >= 0 && state < n) accepting[state] = 1;
    }

    computeClosure();

    for (i = 0; i < n; i++) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < n; j++) newTrans[i][j][k] = 0;
        }
    }

    for (int q = 0; q < n; q++) {
        for (int a = 0; a < m; a++) {
            for (int p = 0; p < n; p++) {
                if (closure[q][p]) {
                    for (int t = 0; t < n; t++) {
                        if (trans[p][t][a]) {
                            for (int u = 0; u < n; u++) {
                                if (closure[t][u]) {
                                    newTrans[q][u][a] = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (int q = 0; q < n; q++) {
        newAccepting[q] = accepting[q];
        for (int p = 0; p < n; p++) {
            if (closure[q][p] && accepting[p]) {
                newAccepting[q] = 1;
                break;
            }
        }
    }

    printf("\nNFA without ε transitions:\n");
    printf("States: ");
    for (i = 0; i < n; i++) printf("q%d ", i);
    printf("\n");

    printf("Transitions:\n");
    for (int q = 0; q < n; q++) {
        for (int a = 0; a < m; a++) {
            printf("δ(q%d, %c) = { ", q, symbols[a]);
            int first = 1;
            for (int t = 0; t < n; t++) {
                if (newTrans[q][t][a]) {
                    if (!first) printf(", ");
                    printf("q%d", t);
                    first = 0;
                }
            }
            if (first) printf("∅");
            printf(" }\n");
        }
    }

    printf("Accepting states: ");
    int first = 1;
    for (i = 0; i < n; i++) {
        if (newAccepting[i]) {
            if (!first) printf(", ");
            printf("q%d", i);
            first = 0;
        }
    }
    if (first) printf("∅");
    printf("\n");

    return 0;
}