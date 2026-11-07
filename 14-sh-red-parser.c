#include <stdio.h>
#include <string.h>
char stack[100], input[100];
int top = -1;
int ip = 0;
void display()
{
int i;
printf("\nStack: ");
for(i = 0; i <= top; i++)
printf("%c", stack[i]);
printf("\tInput: %s", input + ip);
}

int reduce()
{
if(top >= 0 && stack[top] == 'i')
{
stack[top] = 'E';
printf("\tReduce: E -> i");
return 1;
}
if(top >= 2 &&
stack[top-2] == 'E' &&
stack[top-1] == '+' &&
stack[top] == 'E')
{
top -= 2;
stack[top] = 'E';
printf("\tReduce: E -> E+E");
return 1;
}
if(top >= 2 &&
stack[top-2] == 'E' &&
stack[top-1] == '-' &&
stack[top] == 'E')
{
top -= 2;
stack[top] = 'E';
printf("\tReduce: E -> E-E");
return 1;
}
if(top >= 2 &&
stack[top-2] == 'E' &&
stack[top-1] == '*' &&
stack[top] == 'E')
{
top -= 2;
stack[top] = 'E';
printf("\tReduce: E -> E*E");
return 1;
}
if(top >= 2 &&
stack[top-2] == 'E' &&
stack[top-1] == '/' &&
stack[top] == 'E')
{
top -= 2;

stack[top] = 'E';
printf("\tReduce: E -> E/E");
return 1;
}
if(top >= 2 &&
stack[top-2] == '(' &&
stack[top-1] == 'E' &&
stack[top] == ')')
{
top -= 2;
stack[top] = 'E';
printf("\tReduce: E -> (E)");
return 1;
}
return 0;
}
int main()
{
printf("Enter expression (use i for identifier): ");
scanf("%s", input);
strcat(input, "$");
while(1)
{
display();
if(input[ip] != '$')
{
stack[++top] = input[ip++];
printf("\tShift");
}
while(reduce())
{
display();
}
if(input[ip] == '$' &&
top == 0 &&
stack[top] == 'E')
{
printf("\n\nAccepted\n");
break;
}
if(input[ip] == '$')
{
printf("\n\nRejected\n");

break;
}
}
return 0;
}