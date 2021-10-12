%{


%}

%option noyywrap


%%


[ \t\n]+ {fprintf(yyout, " ");}

[^ \t\n]+ {fprintf(yyout, "%s", yytext);}


%%


main()
{
extern FILE *yyin, *yyout; 
  
    
    yyin = fopen("input_fileQ6.txt", "r"); 
  
   
    yyout = fopen("Output_fileQ6.txt", "w"); 
  
    yylex();	 

    return 0; 
}
