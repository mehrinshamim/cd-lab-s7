#include <stdio.h>
#include <string.h>

#define MAX_STATES 10
#define MAX_ALPHABETS 10
#define MAX_TRANSITIONS 50

int n, m;

char states[MAX_STATES][20];
char alphabets[MAX_ALPHABETS][20];

int epsilon[MAX_STATES][MAX_STATES];
int visited[MAX_STATES];

int findState(char name[]) {
    for (int i = 0; i < n; i++) {
        if (strcmp(states[i], name) == 0)
            return i;
    }

    return -1;
}

int findAlphabet(char name[]) {
    for (int i = 0; i < m; i++) {
        if (strcmp(alphabets[i], name) == 0)
            return i;
    }

    return -1;
}

void dfs(int state) {
    visited[state] = 1;

    for (int j = 0; j < n; j++) {
        if (epsilon[state][j] == 1 && visited[j] == 0) {
            dfs(j);
        }
    }
}

int main() {
    int i, j;
    int numTransitions;

    char source[20];
    char input[20];
    char destination[20];

    /* Read states */
    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter the states:\n");
    for (i = 0; i < n; i++) {
        scanf("%s", states[i]);
    }

    /* Read alphabets */
    printf("Enter number of alphabets: ");
    scanf("%d", &m);

    printf("Enter the alphabets:\n");
    for (i = 0; i < m; i++) {
        scanf("%s", alphabets[i]);
    }

    /* Initialize epsilon transition matrix */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            epsilon[i][j] = 0;
        }
    }

    /* Read all transitions */
    printf("Enter number of transitions: ");
    scanf("%d", &numTransitions);

    printf("\nEnter transitions in the form:\n");
    printf("source input destination\n");
    printf("Use 'e' for epsilon.\n\n");

    for (i = 0; i < numTransitions; i++) {

        scanf("%s %s %s", source, input, destination);

        int from = findState(source);
        int to = findState(destination);

        if (from == -1 || to == -1) {
            printf("Invalid state. Enter this transition again.\n");
            i--;
            continue;
        }

        /*
         * Only epsilon transitions are stored in the
         * epsilon matrix because this program calculates
         * epsilon-closure.
         */
        if (strcmp(input, "e") == 0) {
            epsilon[from][to] = 1;
        }
    }

    /* Find epsilon-closure of every state */
    printf("\n--- Epsilon Closures ---\n");

    for (i = 0; i < n; i++) {

        /* Reset visited array */
        for (j = 0; j < n; j++) {
            visited[j] = 0;
        }

        /* Perform DFS from state i */
        dfs(i);

        printf("Epsilon-Closure(%s) = { ", states[i]);

        for (j = 0; j < n; j++) {
            if (visited[j]) {
                printf("%s ", states[j]);
            }
        }

        printf("}\n");
    }

    return 0;
}
