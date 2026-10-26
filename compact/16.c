#include <stdio.h>
int main() {
    int n; char r[10], a[10], b[10], o;
    printf("Enter number of instructions: "); scanf("%d", &n);
    while (n--) {
        scanf("%s = %s %c %s", r, a, &o, b);          // reads:  t1 = b * c
        printf("MOV AX, %s\n", a);                     // every op starts by loading operand 1
        switch (o) {
            case '+': printf("ADD AX, %s\n", b); break;
            case '-': printf("SUB AX, %s\n", b); break;
            case '*': printf("MOV BX, %s\nMUL BX\n", b); break;
            case '/': printf("MOV BX, %s\nMOV DX, 0000H\nDIV BX\n", b); break;
        }
        printf("MOV %s, AX\n\n", r);                   // every op ends by storing AX
    }
}