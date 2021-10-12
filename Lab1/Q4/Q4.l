%{
#include <stdio.h>

int line_count=0,charecter_count=0; 
%}

%option noyywrap


%%


\n {line_count++;}

[^\n\t ] {charecter_count++;}

%%


main()
{
extern FILE *yyin, *yyout; 
    yyin = fopen("input_fileQ4.txt", "r");
    yylex();
    printf("\nnumber of lines in the input file = %d \n",line_count);
    printf("number of charecters in the input file = %d",charecter_count);
   

    return 0; 
}
