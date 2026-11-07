// Q1: epsilon-closure of every state.  Build: gcc 1.c -o 1
#include <stdio.h>
#include <string.h>
int n, e[10][10], v[10];                      // e = epsilon matrix, v = visited
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }

int main() {
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter epsilon transition matrix (%dx%d):\n", n, n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) scanf("%d", &e[i][j]);
    for (int i = 0; i < n; i++) {
        memset(v, 0, sizeof v); dfs(i);       // everything reachable by epsilon (incl. i itself)
        printf("Epsilon-Closure(q%d) = { ", i);
        for (int j = 0; j < n; j++) if (v[j]) printf("q%d ", j);
        puts("}");
    }
}
