#include <stdio.h>
#include <string.h>
#include <ctype.h>
char op[100], v[100][10];                // operator stack, operand stack (strings)
int to = -1, tv = -1, k = 1;             // tops, temp counter
int pr(char c) { return (c == '*' || c == '/') ? 2 : (c == '+' || c == '-') ? 1 : 0; }

void gen() {                             // pop op + 2 operands, print one 3AC line
    char o = op[to--], a[10], b[10], t[10];
    strcpy(b, v[tv--]); strcpy(a, v[tv--]);          // RIGHT operand comes off first
    sprintf(t, "t%d", k++);
    printf("%s = %s %c %s\n", t, a, o, b);
    strcpy(v[++tv], t);                              // temp becomes an operand
}

int main() {
    char e[100], *c;
    printf("Enter the expression: "); scanf("%s", e);
    for (c = e; *c; c++) {
        if (isalnum(*c)) sprintf(v[++tv], "%c", *c);                      // operand
        else if (*c == '(') op[++to] = '(';                               // open bracket
        else if (*c == ')') { while (op[to] != '(') gen(); to--; }        // close bracket
        else { while (to >= 0 && pr(op[to]) >= pr(*c)) gen(); op[++to] = *c; }   // operator
    }
    while (to >= 0) gen();                                                // flush
}