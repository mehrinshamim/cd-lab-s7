// Q5: lexical analyzer (skips spaces/tabs/newlines).  Reads 5-input.txt.  Build: gcc 5.c -o 5
#include <stdio.h>
#include <ctype.h>
#include <string.h>
char *kw[] = {"int","float","char","double","if","else","while","for","return","void","break","continue","do","switch","case","default"};
int isKw(char *s) { for (int i = 0; i < 16; i++) if (!strcmp(s, kw[i])) return 1; return 0; }

int main() {
    FILE *fp = fopen("5-input.txt", "r");
    if (!fp) { puts("File not found"); return 1; }
    int c, i; char t[100];
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) continue;                                        // ignore blanks
        if (isalpha(c) || c == '_') {                                    // identifier / keyword
            for (i = 0; isalnum(c) || c == '_'; c = fgetc(fp)) t[i++] = c;
            t[i] = 0; ungetc(c, fp);                                     // push back the char that ended the token
            printf("%s : %s\n", t, isKw(t) ? "Keyword" : "Identifier");
        } else if (isdigit(c)) {                                         // number
            for (i = 0; isdigit(c); c = fgetc(fp)) t[i++] = c;
            t[i] = 0; ungetc(c, fp);
            printf("%s : Number\n", t);
        }
        else if (strchr("+-*/=<>%", c)) printf("%c : Operator\n", c);
        else if (strchr(";,(){}[]", c)) printf("%c : Special Symbol\n", c);
        else printf("%c : Unknown\n", c);
    }
}
