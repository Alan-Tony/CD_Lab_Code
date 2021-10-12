#include<bits/stdc++.h>
using namespace std;

int num_non_term, num_term;
vector<char> non_term;
vector<char> term;

vector<string> productions;
int num_prod;

void acceptProductions(){

    vector<bool> visited(256, false);

    ifstream infile("input_productions.txt");

    string prod, curr_prod;
    bool rhs;
    while (getline(infile, prod)){

        curr_prod = "";
        rhs = false;        
        for(auto ch : prod){

            if(ch == '='){

                //Reached RHS
                rhs = true;
            }
            else if(ch == '|'){
                
                productions.push_back(prod[0] + (prod[1] + curr_prod));

                //New production with the same LHS
                curr_prod = "";
            }
            else if(rhs){

                curr_prod += ch;
            }

            if(ch != '=' && ch != '|' && !visited[(int)ch - 1]){

                if(isupper(ch)){

                    non_term.push_back(ch);
                }
                else if(ch != '#'){

                    term.push_back(ch);
                }
            }

            //Already visited character
            visited[(int)ch - 1] = true;
        }
        if(curr_prod.length()){
            
            productions.push_back(prod[0] + (prod[1] + curr_prod));
        }

    }
    term.push_back('$');

    num_non_term = non_term.size();
    num_term = term.size();
    num_prod = productions.size();

    cout<<"\nProductions: ";
    for(auto prod : productions){

        cout<<"\n"<<prod;
    }

    cout<<"\nNon-terminals: \n";
    for(auto ch : non_term){

        cout<<ch<<"\t";
    }

    cout<<"\nTerminals: \n";
    for(auto ch : term){

        cout<<ch<<"\t";
    }

}

int main(){

    acceptProductions();

    cout<<"\nLR0 items: ";
    for(auto prod : productions){

        if(prod[2] == '#'){
            cout<<"\n"<<prod[0]<<"=.";
            continue;
        }

        for(unsigned int i=2; i<=prod.length(); i++){

            cout<<"\n";
            for(unsigned int j=0; j<prod.length(); j++){

                if(i==j){
                    cout<<".";
                }
                cout<<prod[j];
            }

            if(i == prod.length()){
                cout<<".";
            }
        }
    }

    cout<<"\n";
    return 0;
}