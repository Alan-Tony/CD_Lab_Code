%{


%}

%option noyywrap


%%



[.|\n]+ {fprintf(yyout, "%s", yytext);}


%%


main()
{
extern FILE *yyin, *yyout; 
  
    
    yyin = fopen("input_fileQ3.txt", "r"); 
  
   
    yyout = fopen("Output_fileQ3.txt", "w"); 
  
    yylex();	 

    return 0; 
}
