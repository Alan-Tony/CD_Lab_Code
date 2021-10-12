#include<ctype.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#define BUF_LIM 100 //Define buffer limit

char *BUF;
FILE *fout;
char* keywords[] = {"print", "int", "if", "endif", "then", "else", "while", "do", "enddo"};
int max_var_len;

int readAlnum(char *code, int len, int pos);
char* readFile(char* filename);
int main(){

    bool valid_comment;
    char prev;
    BUF = (char*)calloc(BUF_LIM, sizeof(*BUF));
    int buf_count = 0;

    char* keywords[] = {"print", "int", "if", "endif", "then", "else", "while", "do", "enddo"};
    int num_keywords = sizeof(keywords) / sizeof(*keywords);

    //Code string
    //char* code = "int 375 a2 ; b = 3;";
    char *filename = "input.txt";
    char* code = readFile(filename);
    if(!code){

        printf("\nNo code input.\nAborting");
        return 0;
    }

    //Creating output file
    fout = fopen("output.txt", "w");

    printf("\nEnter maximum variable length: ");
    scanf("%d", &max_var_len);

    int len = strlen(code);
    fprintf(fout, "\nLexical analysis:\n");
    for(int i=0; i<len; i++){

        if(code[i] == ' '){

            if(!i || i && code[i-1] != ' '){
                fprintf(fout, "\n(Not a token) Single space");
            }
        }
        else if(code[i] == '\t'){
            
            if(!i || i && code[i-1] != '\t'){
                fprintf(fout, "\n(Not a token) Tab space");
            }
        }
        else if(code[i] == '\n'){
            
            if(!i || i && code[i-1] != '\n'){
                fprintf(fout, "\n(Not a token) New line");
            }
        }
        else if(code[i] == '/' && i+1 < len &&  code[i+1] == '*'){

            //Multi-line comment detected
            valid_comment = false;
            i += 2;
            prev = '\0';
            while(i < len){

                if(prev == '*' && code[i] == '/'){

                    //End of comment detected
                    valid_comment = true;
                    i++;
                    break;
                }

                prev = code[i];
                i++;
            }
            
            if(valid_comment){
                
                //Ignoring comment
                //printf("\n(Not a token) Valid multi-line comment detected");
            }
            else{

                fprintf(fout, "\nInvalid comment detected.\nAborting...");
                exit(0);
            }

            i--;
        }
        else if(code[i] ==  ',' || code[i] ==  ';' || code[i] == '(' || code[i] == ')' || code[i] == '[' || code[i] == ']' || code[i] == '{' || code[i] == '}'){

            fprintf(fout, "\nDelimiter: %c", code[i]);
        }
        else if(i < len-1 &&  (code[i] == '!' || code[i] == '<' || code[i] == '>' || code[i] == '=') && code[i+1] == '='){

            fprintf(fout, "\nOperator: %c%c", code[i], code[i+1]);
            i++;
        }
        else if(code[i] == '<' || code[i] == '>' || code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/' || code[i] == '='){

            fprintf(fout, "\nOperator: %c", code[i]);
        }
        else if(isalnum(code[i])){

            i = readAlnum(code, len, i);
        }
        else{

            fprintf(fout, "\nInvalid character: %c", code[i]);
        }
    }

    printf("\n");
    return 0;
}

char* readFile(char* filename){

    char *buffer = 0;
    long length;
    FILE * f = fopen (filename, "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = (char*) malloc (length+1);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
        buffer[length] = '\0';
    }
    else{
        printf("\nCould not open file.");
    }

    if (buffer)
    {
        printf("\nCode input:\n\n%s\n", buffer);
    }

    return buffer;
}

char* subString(char* str, int left, int right)
{
    int i;
    char* subStr = (char*)malloc(
                  sizeof(char) * (right - left + 2));
 
    for (i = left; i <= right; i++)
        subStr[i - left] = str[i];
    subStr[right - left + 1] = '\0';
    return (subStr);
}

bool isKeyWord(char *s){

    bool result = false;
    int num_keywords = sizeof(keywords)/sizeof(char*);
    for(int i=0; i<num_keywords; i++){

        if(strcmpi(s, keywords[i]) == 0){
            result = true;
            break;
        }
    }

    return result;
}

int readAlnum(char *code, int len, int pos){

    if(pos >= len){
        printf("\nPosition (pos) out of bounds. \nAborting...");
        exit(0);
    }

    bool constant, identif, inv_identif = false, only_alpha;

    int i = pos;

    if(isdigit(code[i])){

        constant = true;
        identif = false;
        only_alpha = false;
    }
    else{

        constant = false;
        identif = true;
        only_alpha = true;
    }

    i++;
    while(i < len && isalnum(code[i])){

        if(constant && isalpha(code[i])){

            inv_identif = true;
            constant = false;
            //break;
            //Got to end of alnum sequence
        }
        else if(only_alpha && isdigit(code[i])){

            only_alpha = false;
        }

        i++;
    }

    char* lexime = subString(code, pos, i-1);

    if(inv_identif){

        fprintf(fout, "\nInvalid identifier: %s", lexime);
    }
    else if(constant){

        fprintf(fout, "\nConstant: %s", lexime);
    }
    else if(only_alpha && isKeyWord(lexime)){

        fprintf(fout, "\nKeyword: %s", lexime);
    }
    else if(identif){

        if(strlen(lexime) > max_var_len){

            fprintf(fout, "\nIdentifier \"%s\" exceeds maximum length of: %d\nAborting...", lexime, max_var_len);
            exit(0);
        }
        else{

            fprintf(fout, "\nIdentifier: %s", lexime);
        }
    }
    else{
        
        fprintf(fout, "\nUnknown string: %s", lexime);
    }

    if(lexime){

        free(lexime);
    }

    return i-1;
}