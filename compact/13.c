#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
char s[100]; int p = 0;                       // input string and current position

void err() { printf("Syntax Error at position %d\n", p); exit(1); }
void sp()  { while (isspace(s[p])) p++; }     // skip spaces
int E();                                      // prototype: F() calls E()

int F() {                                     // F → ( E ) | number
    int v = 0; sp();
    if (s[p] == '(') { p++; v = E(); sp(); if (s[p++] != ')') err(); }
    else if (isdigit(s[p])) while (isdigit(s[p])) v = v * 10 + s[p++] - '0';
    else err();
    return v;
}
int T() {                                     // T → F { (*|/) F }
    int v = F(), r; char o;
    for (sp(); s[p] == '*' || s[p] == '/'; sp()) {
        o = s[p++]; r = F();
        if (o == '*') v *= r; else if (r) v /= r; else err();
    }
    return v;
}
int E() {                                     // E → T { (+|-) T }
    int v = T(), r; char o;
    for (sp(); s[p] == '+' || s[p] == '-'; sp()) {
        o = s[p++]; r = T(); v += (o == '+') ? r : -r;
    }
    return v;
}
int main() {
    printf("Enter an expression: "); fgets(s, 100, stdin);
    int v = E(); sp();
    if (s[p]) err();                          // leftover characters → invalid
    printf("Valid Expression\nResult = %d\n", v);
}
```