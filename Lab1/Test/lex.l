%{
int max_var_len=1;
%}

/* This tells flex to read only one input file */ 
%option noyywrap


%%

[a-z]{1,2}

%%

// This program ignores comments and 
// removes redundant whitespaces (viz. single space, tab space, new line)


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
