%{

#include<stdio.h>

%}

%option noyywrap

%%


[ \t\n]+ {printf(" ");}

[^ \n\t]+ {printf("%s", yytext);}


%%

main()
{  
    yylex();	 

    return 0; 
}
