#include <stdio.h>

int n;
int eps[10][10];
int visited[10];

void dfs(int state) {
    visited[state] = 1;

    for (int j = 0; j < n; j++) {
        if (eps[state][j] == 1 && visited[j] == 0) {
            dfs(j);
        }
    }
}

int main() {
    int i, j;

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter epsilon transition matrix (%dx%d):\n", n, n);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            scanf("%d", &eps[i][j]);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            visited[j] = 0;

        dfs(i);

        printf("Epsilon-Closure(q%d) = { ", i);

        for (j = 0; j < n; j++)
            if (visited[j])
                printf("q%d ", j);

        printf("}\n");
    }

    return 0;
}