%{
#include <stdio.h>
%}

%%

.*[mM][eE][hH][rR].*      { printf("Rejected\n"); }

.*                        { printf("Accepted\n"); }

\n                        ;

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter String:\n");
    yylex();
    return 0;
}