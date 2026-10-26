
#include <stdio.h>
#include <string.h>
char s[100] = "$", in[100];          // s = stack (always a valid string), in = input
int t = 0, ip = 0;                   // t = stack top index, ip = input index
int lvl(char c) { return (c == '*' || c == '/') ? 2 : 1; }   // + - = 1, * / = 2

int main() {
    char a, b;
    printf("Enter the expression (use i for operand): ");
    scanf("%s", in); strcat(in, "$");
    printf("\n%-15s %-15s %s\n", "STACK", "INPUT", "ACTION");
    while (1) {
        int k = t; while (s[k] == 'E') k--;      // skip E's: find topmost terminal
        a = s[k]; b = in[ip];
        printf("%-15s %-15s ", s, in + ip);

        if (a == '$' && b == '$') { puts("ACCEPT"); break; }
        if (a == 'i' && b == 'i') { puts("ERROR");  break; }   // two operands in a row

        int shift = a == '$' ||                                // empty stack → shift
                    (b != '$' && a != 'i' &&                   // b=='$' or a=='i' → reduce
                     (b == 'i' || lvl(a) < lvl(b)));           // operand, or higher level → shift

        if (shift) { s[++t] = b; s[t + 1] = 0; ip++; puts("SHIFT"); }
        else if (s[t] == 'i') { s[t] = 'E'; puts("REDUCE"); }              // i → E
        else if (t >= 2 && s[t] == 'E' && s[t-2] == 'E' && strchr("+-*/", s[t-1]))
            { t -= 2; s[t] = 'E'; s[t + 1] = 0; puts("REDUCE"); }          // E op E → E
        else { puts("ERROR"); break; }
    }
}