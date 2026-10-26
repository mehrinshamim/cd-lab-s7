#include <stdio.h>
#include <string.h>
char s[100], in[100], *h[] = {"i", "E+E", "E-E", "E*E", "E/E", "(E)"};   // all handles
int t = -1, ip = 0;

int reduce() {                                     // try each handle against the stack top
    for (int k = 0; k < 6; k++) {
        int L = strlen(h[k]);
        if (t + 1 >= L && !strncmp(s + t - L + 1, h[k], L)) {   // handle on top of stack?
            t -= L - 1; s[t] = 'E'; s[t + 1] = 0;               // replace it with E
            printf("\tReduce: E -> %s", h[k]); return 1;
        }
    }
    return 0;
}
void show() { printf("\nStack: %s\tInput: %s", s, in + ip); }

int main() {
    printf("Enter expression (use i for identifier): ");
    scanf("%s", in); strcat(in, "$");
    for (;;) {
        show();
        if (in[ip] != '$') { s[++t] = in[ip++]; s[t + 1] = 0; printf("\tShift"); }
        while (reduce()) show();
        if (in[ip] == '$') {                       // input finished: accept or reject
            puts(t == 0 && s[0] == 'E' ? "\n\nAccepted" : "\n\nRejected");
            break;
        }
    }
}