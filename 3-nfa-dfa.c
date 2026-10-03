#include <stdio.h>

#define MAX 10

int n, m;
char symbols[MAX];
int trans[MAX][MAX][MAX];
int accepting[MAX];

int dfaStates[1 << MAX][MAX];
int dfaTrans[1 << MAX][MAX];
int dfaAccept[1 << MAX];
int dfaCount = 0;

int isSameState(int a[MAX], int b[MAX]) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int findDFAState(int subset[MAX]) {
    for (int i = 0; i < dfaCount; i++) {
        if (isSameState(dfaStates[i], subset)) return i;
    }
    return -1;
}

int main() {
    int i, j, k;

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols: ");
    scanf("%d", &m);
    printf("Enter symbols (space separated): ");
    for (i = 0; i < m; i++) scanf(" %c", &symbols[i]);

    printf("Enter transition matrices (0/1) for each symbol:\n");
    for (k = 0; k < m; k++) {
        printf("Symbol %c:\n", symbols[k]);
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                scanf("%d", &trans[i][j][k]);
    }

    printf("Enter accepting states (enter -1 to stop): ");
    for (i = 0; i < n; i++) accepting[i] = 0;
    while (1) {
        int s;
        scanf("%d", &s);
        if (s == -1) break;
        if (s >= 0 && s < n) accepting[s] = 1;
    }

    int startState[MAX] = {0};
    startState[0] = 1;

    for (i = 0; i < n; i++) dfaStates[0][i] = startState[i];
    dfaCount = 1;

    for (int current = 0; current < dfaCount; current++) {

        dfaAccept[current] = 0;
        for (i = 0; i < n; i++) {
            if (dfaStates[current][i] && accepting[i]) {
                dfaAccept[current] = 1;
                break;
            }
        }

        for (k = 0; k < m; k++) {
            int newSet[MAX] = {0};

            for (i = 0; i < n; i++) {
                if (dfaStates[current][i]) {
                    for (j = 0; j < n; j++) {
                        if (trans[i][j][k]) newSet[j] = 1;
                    }
                }
            }

            int idx = findDFAState(newSet);
            if (idx == -1) {
                for (i = 0; i < n; i++) dfaStates[dfaCount][i] = newSet[i];
                idx = dfaCount++;
            }
            dfaTrans[current][k] = idx;
        }
    }

    printf("\n--- DFA RESULTS ---\n");
    printf("Total DFA States: %d\n\n", dfaCount);

    printf("DFA States mapping:\n");
    for (i = 0; i < dfaCount; i++) {
        printf("q%d = { ", i);
        int empty = 1;
        for (j = 0; j < n; j++) {
            if (dfaStates[i][j]) {
                printf("q%d ", j);
                empty = 0;
            }
        }
        if (empty) printf("∅ ");
        printf("}\n");
    }

    printf("\nTransitions:\n");
    for (i = 0; i < dfaCount; i++) {
        for (k = 0; k < m; k++) {
            printf("δ(q%d, %c) = q%d\n", i, symbols[k], dfaTrans[i][k]);
        }
    }

    printf("\nAccepting states: ");
    for (i = 0; i < dfaCount; i++) {
        if (dfaAccept[i]) printf("q%d ", i);
    }
    printf("\n");

    return 0;
}