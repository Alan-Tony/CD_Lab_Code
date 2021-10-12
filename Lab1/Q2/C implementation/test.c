#include<stdio.h>
#include<string.h>

char* get(int i){

    /*
    char ar[] = {'a', '\0', 'b'};
    printf("%s", ar);
    */

    char* operators[] = {"+", "-", "*", "/", "=", "<", ">", "<="};

    return operators[i];
}

char* keywords[] = {"print", "int", "if", "endif", "then", "else", "while", "do", "enddo"};

int main(){

    /*
    char* operator;
    for(int i=0; i<8; i++){

        operator = get(i);
        printf("\n%d", operator);
        
    }
    */

    //printf("\n%d\n", strcmp("Hello", "Hello"));
    //printf("\n%d\n", sizeof(keywords)/sizeof(char*));

    char *buffer = 0, *filename = "input.txt";
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
    // start to process your data / extract strings here...
        printf("\nCode input:\n\n%s", buffer);
    }

    printf("\n");
    return 0;
}