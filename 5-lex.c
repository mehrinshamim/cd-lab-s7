#include <stdio.h>
#include <ctype.h>
#include <string.h>

int isKeyword(char str[])
{
    char *keywords[] = {
        "int", "float", "char", "double", "if", "else",
        "while", "for", "return", "void", "break",
        "continue", "do", "switch", "case", "default"
    };

    int n = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < n; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

int main()
{
    FILE *fp;
    char ch;
    char token[100];
    int i;

    fp = fopen("5-input.txt", "r");

    if (fp == NULL)
    {
        printf("File not found\n");
        return 0;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        // Ignore spaces, tabs and newlines
        if (ch == ' ' || ch == '\t' || ch == '\n')
            continue;

        // Identifier or Keyword
        if (isalpha(ch) || ch == '_')
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '_'))
            {
                token[i++] = ch;
            }

            token[i] = '\0';

            if (ch != EOF)
                fseek(fp, -1, SEEK_CUR);

            if (isKeyword(token))
                printf("%s : Keyword\n", token);
            else
                printf("%s : Identifier\n", token);
        }

        // Number
        else if (isdigit(ch))
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF && isdigit(ch))
            {
                token[i++] = ch;
            }

            token[i] = '\0';

            if (ch != EOF)
                fseek(fp, -1, SEEK_CUR);

            printf("%s : Number\n", token);
        }

        // Operators
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
                 ch == '=' || ch == '<' || ch == '>' || ch == '%')
        {
            printf("%c : Operator\n", ch);
        }

        // Special Symbols
        else if (ch == ';' || ch == ',' || ch == '(' || ch == ')' ||
                 ch == '{' || ch == '}' || ch == '[' || ch == ']')
        {
            printf("%c : Special Symbol\n", ch);
        }

        // Any other character
        else
        {
            printf("%c : Unknown\n", ch);
        }
    }

    fclose(fp);
    return 0;
}