// Q1 (named input, same style as 1-e-closure-1.c): epsilon-closure of every state.
// Build: gcc 1.c -o 1      Input: states, alphabets, transitions "source input destination" (e = epsilon)
#include <stdio.h>
#include <string.h>
int n, m, t, e[10][10], v[10];                // e = epsilon matrix, v = visited
char st[10][20], al[10][20], a[20], s[20], b[20];
int id(char nm[][20], int k, char *w) { for (int i = 0; i < k; i++) if (!strcmp(nm[i], w)) return i; return -1; }
void dfs(int s) { v[s] = 1; for (int j = 0; j < n; j++) if (e[s][j] && !v[j]) dfs(j); }

int main() {
    int i, j, x, y;
    printf("Enter number of states: "); scanf("%d", &n);
    printf("Enter the states:\n");
    for (i = 0; i < n; i++) scanf("%s", st[i]);
    printf("Enter number of alphabets: "); scanf("%d", &m);
    printf("Enter the alphabets:\n");
    for (i = 0; i < m; i++) scanf("%s", al[i]);
    printf("Enter number of transitions: "); scanf("%d", &t);
    printf("\nEnter transitions in the form:\nsource input destination\nUse 'e' for epsilon.\n\n");
    for (i = 0; i < t; i++) {
        scanf("%s %s %s", a, s, b);
        x = id(st, n, a); y = id(st, n, b);
        if (x < 0 || y < 0) { puts("Invalid state. Enter this transition again."); i--; continue; }
        if (!strcmp(s, "e")) e[x][y] = 1;     // only epsilon edges matter for the closure
    }
    printf("\n--- Epsilon Closures ---\n");
    for (i = 0; i < n; i++) {
        memset(v, 0, sizeof v); dfs(i);
        printf("Epsilon-Closure(%s) = { ", st[i]);
        for (j = 0; j < n; j++) if (v[j]) printf("%s ", st[j]);
        puts("}");
    }
}
