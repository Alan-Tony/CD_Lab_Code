%{
#include<stdlib.h>
max_var_len = 1;
%}

/* This tells flex to read only one input file */ 
%option noyywrap


%%

[,;(){}._[]|] {fprintf(yyout,"Delimiter: %s\n",yytext);}

"<="|">="|"!="|"=="|"<"|">"|"="|"+"|"-"|"*"|"/"  {fprintf(yyout,"Operator: %s\n",yytext);}

("print")|("int")|("if")|("endif")|("then")|("else")|("while")|("do")|("enddo") {fprintf(yyout,"Keyword: %s\n",yytext);}

[a-z][a-z0-9]* {if(yyleng > max_var_len) 
                { 
                    fprintf(yyout, "Identifier \"%s\" exceeds maximum length %d.\nAborting...", yytext, max_var_len); 
                    exit(0);
                } 
                else
                { 
                    fprintf(yyout, "Ientifier: %s\n",yytext );
                } }

[0-9]+ {fprintf(yyout,"Constant: %s\n",yytext);}

\/\*(.*\n)*.*\*\/    {;}

\t+  {fprintf(yyout,"(Non token) Tab space\n");}
\n+  {fprintf(yyout,"(Non token) Newline\n");}
[ ]+ {fprintf(yyout,"(Non token) Single Space\n");} 

%%

// This program ignores comments,
// removes redundant whitespaces (viz. single space, tab space, new line),
// and restricts identifier name length

main()
{
extern FILE *yyin, *yyout; 
    
    printf("\nEnter maximum variable length: ");
    scanf("%d", &max_var_len);
    
    yyin = fopen("input.txt", "r");     
    yyout = fopen("Output.txt", "w"); 
  
    yylex();
	 

    return 0; 
}
