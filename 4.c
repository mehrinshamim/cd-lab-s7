#include <stdio.h>

#define MAX 10

int n, m;
char symbols[MAX];
int delta[MAX][MAX];
int accepting[MAX];
int dist[MAX][MAX];
int class[MAX];

int main() {
    int i, j, k;

    printf("Enter number of states: ");
    scanf("%d", &n);
    printf("Enter number of input symbols: ");
    scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (i = 0; i < m; i++) scanf(" %c", &symbols[i]);

    printf("Enter transition table (row = state, col = symbol):\n");
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            scanf("%d", &delta[i][j]);

    printf("Enter accepting states (enter -1 to stop): ");
    while (scanf("%d", &i) && i != -1) accepting[i] = 1;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            dist[i][j] = (accepting[i] != accepting[j]);

    int changed;
    do {
        changed = 0;
        for (i = 0; i < n; i++) {
            for (j = i + 1; j < n; j++) {
                if (!dist[i][j]) {
                    for (k = 0; k < m; k++) {
                        if (dist[delta[i][k]][delta[j][k]]) {
                            dist[i][j] = dist[j][i] = 1;
                            changed = 1;
                            break;
                        }
                    }
                }
            }
        }
    } while (changed);

    int classCount = 0;
    for (i = 0; i < n; i++) class[i] = -1;
    for (i = 0; i < n; i++) {
        if (class[i] == -1) {
            for (j = i; j < n; j++)
                if (!dist[i][j]) class[j] = classCount;
            classCount++;
        }
    }

    printf("\n--- Minimized DFA ---\n");
    printf("Total States: %d\n", classCount);

    printf("\nTransitions:\n");
    for (i = 0; i < classCount; i++) {
        int rep = 0;
        while (class[rep] != i) rep++;

        for (k = 0; k < m; k++)
            printf("δ(q%d, %c) = q%d\n", i, symbols[k], class[delta[rep][k]]);
    }

    printf("\nAccepting states: ");
    for (i = 0; i < classCount; i++) {
        int rep = 0;
        while (class[rep] != i) rep++;
        if (accepting[rep]) printf("q%d ", i);
    }

    printf("\nStart state: q%d\n", class[0]);

    return 0;
}