#include <stdio.h>
#include <string.h>

#define MAX 10

int n;
int trans[MAX][2][MAX];
int visited[100];
int queue[100];
int front = 0, rear = 0;

int main()
{
    int i, j, k;

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter transition table\n");

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < 2; j++)
        {
            printf("State %d Input %d\n", i, j);

            for(k = 0; k < n; k++)
            {
                scanf("%d", &trans[i][j][k]);
            }
        }
    }

    queue[rear++] = 1;

    while(front < rear)
    {
        int current = queue[front++];

        if(visited[current])
            continue;

        visited[current] = 1;

        printf("\nSubset %d\n", current);

        for(j = 0; j < 2; j++)
        {
            int next = 0;

            for(i = 0; i < n; i++)
            {
                if(current & (1 << i))
                {
                    for(k = 0; k < n; k++)
                    {
                        if(trans[i][j][k])
                            next |= (1 << k);
                    }
                }
            }

            printf("Input %d -> %d\n", j, next);

            if(!visited[next])
                queue[rear++] = next;
        }
    }

    return 0;
}