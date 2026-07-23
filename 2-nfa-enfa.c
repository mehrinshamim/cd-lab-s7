#include <stdio.h>

#define MAX 10

int n, m;
int eps[MAX][MAX];
int trans[MAX][MAX][MAX];
int final[MAX];

void closure(int state, int visited[])
{
    int i;
    visited[state] = 1;

    for (i = 0; i < n; i++)
    {
        if (eps[state][i] && !visited[i])
            closure(i, visited);
    }
}

int main()
{
    int i, j, k;

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols: ");
    scanf("%d", &m);

    printf("Enter final states (0 or 1):\n");
    for (i = 0; i < n; i++)
        scanf("%d", &final[i]);

    printf("Enter epsilon transition matrix:\n");
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            scanf("%d", &eps[i][j]);

    printf("Enter transition table:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            printf("State q%d Input %d\n", i, j);
            for (k = 0; k < n; k++)
                scanf("%d", &trans[i][j][k]);
        }
    }

    printf("\nEquivalent NFA without epsilon transitions\n");

    for (i = 0; i < n; i++)
    {
        int visit[MAX] = {0};

        closure(i, visit);

        for (j = 0; j < m; j++)
        {
            int result[MAX] = {0};

            for (k = 0; k < n; k++)
            {
                if (visit[k])
                {
                    int x;
                    for (x = 0; x < n; x++)
                    {
                        if (trans[k][j][x])
                            result[x] = 1;
                    }
                }
            }

            printf("q%d --%d--> { ", i, j);

            for (k = 0; k < n; k++)
            {
                if (result[k])
                    printf("q%d ", k);
            }

            printf("}\n");
        }
    }

    printf("\nNew Final States: ");

    for (i = 0; i < n; i++)
    {
        int visit[MAX] = {0};

        closure(i, visit);

        for (j = 0; j < n; j++)
        {
            if (visit[j] && final[j])
            {
                printf("q%d ", i);
                break;
            }
        }
    }

    printf("\n");

    return 0;
}