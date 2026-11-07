#include <stdio.h>
#include <string.h>
int main()
{
int n, i;
char result[10], operand1[10], operand2[10], op;
printf("Enter the number of Three Address Code instructions: ");
scanf("%d", &n);
printf("Enter the Three Address Code instructions:\n");
printf("\n8086 Assembly Code:\n");
for (i = 0; i < n; i++)
{
scanf("%s = %s %c %s", result, operand1, &op, operand2);
switch (op)
{
case '+':

printf("MOV AX, %s\n", operand1);
printf("ADD AX, %s\n", operand2);
printf("MOV %s, AX\n", result);
break;
case '-':
printf("MOV AX, %s\n", operand1);
printf("SUB AX, %s\n", operand2);
printf("MOV %s, AX\n", result);
break;
case '*':
printf("MOV AX, %s\n", operand1);
printf("MOV BX, %s\n", operand2);
printf("MUL BX\n");
printf("MOV %s, AX\n", result);
break;
case '/':
printf("MOV AX, %s\n", operand1);
printf("MOV BX, %s\n", operand2);
printf("MOV DX, 0000H\n");
printf("DIV BX\n");
printf("MOV %s, AX\n", result);
break;
default:
printf("Invalid operator\n");
}
printf("\n");
}
return 0;
}