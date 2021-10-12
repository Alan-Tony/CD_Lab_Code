%{

%}

/* This tells flex to read only one input file */ 
%option noyywrap


%%

[,;(){}._[]|] {fprintf(yyout,"Delimiter: %s\n",yytext);}

"<="|">="|"!="|"=="|"<"|">"|"="|"+"|"-"|"*"|"/"  {fprintf(yyout,"Operator: %s\n",yytext);}

("print")|("int")|("if")|("endif")|("then")|("else")|("while")|("do")|("enddo") {fprintf(yyout,"Keyword: %s\n",yytext);}

[a-z][a-z0-9]* {fprintf(yyout,"This is an dentifier: %s\n",yytext);}

[0-9]+ {fprintf(yyout,"This is a constant: %s\n",yytext);}

\/\*(.*\n)*.*\*\/    {fprintf(yyout,"(Non token) Comment: %s\n",yytext);}

\t  {fprintf(yyout,"(Non token) Tab space\n");}
\n  {fprintf(yyout,"(Non token) Newline\n");}
[ ] {fprintf(yyout,"(Non token) Single Space\n");} 


%%

main()
{
extern FILE *yyin, *yyout; 
  
    
    yyin = fopen("input.txt", "r"); 
  
   
    yyout = fopen("Output.txt", "w"); 
  
    yylex();
	 

    return 0; 
}
