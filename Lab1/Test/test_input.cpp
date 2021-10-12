#include<iostream>
#include<string>
#include<stdlib.h>
using namespace std;

int main(){

    char c = '\0', prev;
    string s;

    while(1){

        c = getc(stdin);
        if(c != EOF){

            s.push_back(c);
        }
        else{
            break;
        }
    }

    cout<<s;
    cout<<"\nLength of string= "<<s.length();
    cout<< ((s[s.length()-2] == EOF) ? "EOF" : "Not EOF");

    cout<<"\n";
    return 0;
}