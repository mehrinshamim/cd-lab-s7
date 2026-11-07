#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX 100
char operatorStack[MAX];
int top = -1;
char operandStack[MAX][10];
int operandTop = -1;
int tempCount = 1;
int precedence(char op)
{
if (op == '*' || op == '/')
return 2;
else if (op == '+' || op == '-')

return 1;
else
return 0;
}
void generateCode()
{
char op;
char operand1[10], operand2[10], temp[10];
op = operatorStack[top--];
strcpy(operand2, operandStack[operandTop--]);
strcpy(operand1, operandStack[operandTop--]);
sprintf(temp, "t%d", tempCount++);
printf("%s = %s %c %s\n",
temp, operand1, op, operand2);
strcpy(operandStack[++operandTop], temp);
}
int main()
{
char expression[MAX];
printf("Enter the expression: ");
scanf("%s", expression);
for (int i = 0; expression[i] != '\0'; i++)
{
char ch = expression[i];
// If operand
if (isalnum(ch))
{
operandStack[++operandTop][0] = ch;
operandStack[operandTop][1] = '\0';
}

// If opening parenthesis
else if (ch == '(')
{
operatorStack[++top] = ch;
}
// If closing parenthesis
else if (ch == ')')
{
while (top != -1 && operatorStack[top] != '(')
{
generateCode();
}
top--;
}
// If operator
else
{
while (top != -1 &&
operatorStack[top] != '(' &&
precedence(operatorStack[top]) >= precedence(ch))
{
generateCode();
}
operatorStack[++top] = ch;
}
}
// Process remaining operators
while (top != -1)
{
generateCode();
}
return 0;
}