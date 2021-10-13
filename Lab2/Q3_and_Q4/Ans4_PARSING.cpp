#include <bits/stdc++.h>
using namespace std;

vector<char> *first;
vector<char> *follow;

vector<char> non_term;
vector<char> term;
int num_non_term, num_term;

//Index
int non_term_index[256];
int term_index[256];

vector<string> productions;
int num_prod;
int **parse_table;

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

void calcIndices(){

    //Initialize indices of each non-terminal and terminals
    for(int i=0; i<num_term; i++){

        term_index[(int)term[i]] = i;
    }
    for(int i=0; i<num_non_term; i++){

        non_term_index[(int)non_term[i]] = i;
    }
}

//Compute First

bool Union(set<char> &first, vector<char> &rhs_first, bool include_eps);
void helperFirst(int target_index, vector<bool> &found, vector<vector<int>> &where){

    if(found[target_index]){
        return;
    }

    string prod;
    int rhs_index;
    bool epsilon;

    //Set that stores first set
    set<char> firstSet;

    //Iterate through each production where the non-term is in LHS
    for(auto prod_index : where[target_index]){

        prod = productions[prod_index];
        epsilon = true;

        for(unsigned int i=2; i<prod.length() && epsilon; i++){
            
            epsilon = false;

            if(!isupper(prod[i])){

                firstSet.insert(prod[i]);
                break;
            }
            else{

                rhs_index = non_term_index[(int)prod[i]];
                if(!found[rhs_index]){

                    helperFirst(rhs_index, found, where);
                }

                epsilon = Union(firstSet, first[rhs_index], true);
            }
        }
    }

    //Copy set elements to first[target_index]
    for(auto ch : firstSet){

        first[target_index].push_back(ch);
    }

    found[target_index] = true;
}

void computeFirst(){

    string prod;
    int index;

    first = new vector<char>[num_non_term];
    vector<bool> found(num_non_term, false);

    //Positions where target non-terminal is in LHS
    vector<vector<int>> where = vector<vector<int>>(num_non_term, vector<int>({}));
    for(int i=0; i<num_prod; i++){

        prod = productions[i];
        index = non_term_index[(int)prod[0]];
        where[index].push_back(i);
    }

    //Find first of all non-terminals
    cout<<"\nFirst set: \n";
    for(int i=0; i<num_non_term; i++){

        cout<<"\n"<<non_term[i]<<": ";
        if(!found[i]){

            helperFirst(i, found, where);
        }

        //Display
        for(auto ch : first[i]){
            cout<<ch<<" ";
        }
    }

}

//Compute Follow

void helperFollow(int target_index, vector<bool> &found, vector<vector<pair<int, int>>> &where){

    int prod_num, pos, lhs_index, rhs_index, i;
    string prod;
    char next_char;
    set<char> followSet;
    bool epsilon;
    for(auto prod_pos : where[target_index]){

        prod_num = prod_pos.first;
        prod = productions[prod_num];

        pos = prod_pos.second;
        //Target non-terminal is at end production
        epsilon = true;
        for(i=pos+1; i<prod.length() && epsilon; i++){

            next_char = prod[i];
            if(!isupper(next_char)){

                followSet.insert(next_char);
                break;
            }
            else{
                
                rhs_index = non_term_index[(int)next_char];
                epsilon = Union(followSet, first[rhs_index], false);
            }

        }
        
        if(i == prod.length() && prod[0] != prod[pos]){

            lhs_index = non_term_index[(int)prod[0]];
            if(!found[lhs_index]){
                helperFollow(lhs_index, found, where);
            }
            epsilon = Union(followSet, follow[lhs_index], false);
        }

    }

    //Copy set elements to follow[target_index]
    for(auto ch : followSet){

        follow[target_index].push_back(ch);
    }

    found[target_index] = true;
    
}

void computeFollow(){

    follow = new vector<char>[num_non_term];
    vector<bool> found(num_non_term, false);

    //Production numbers and positions where target non-terminal is in the RHS
    vector<vector<pair<int, int>>> where(num_non_term, vector<pair<int, int>>({}));
    string prod;
    int target_index;
    for(int i=0; i<num_prod; i++){

        prod = productions[i];
        for(unsigned int j=2; j<prod.length(); j++){

            if(isupper(prod[j])){

                target_index = non_term_index[(int)prod[j]];
                where[target_index].push_back(make_pair(i, j));
            }
        }
    }

    //Start symbol will have $ in follow
    follow[0].push_back('$');

    //Find follow of every non-terminal
    cout<<"\nFollow characters: ";
    for(int i=0; i<num_non_term; i++){
        
        cout<<"\n"<<non_term[i]<<": ";
        if(!found[i]){

            helperFollow(i, found, where);
        }

        for(auto ch : follow[i]){
            cout<<ch<<" ";
        }
    }

}

void generateParseTable(){

    int i, j;

    //Initialize parse table with -1 as production index for 
    //each non-terminal symbol on reading a terminal
    parse_table = new int*[num_non_term];
    for(i=0; i<num_non_term; i++){

        parse_table[i] = new int[num_term];
        for(j=0; j<num_term; j++){

            parse_table[i][j] = -1;
        }
    }

    //Populate parse table based on follow and first terminals
    string p;
    int lhs_index, ti;
    for(int i=0; i<productions.size(); i++){

        p = productions[i];
        lhs_index = non_term_index[(int)p[0]];
        if(p[2] == '#'){    //If null production, populate for follow of current non-terminal

            for(auto f : follow[lhs_index]){

               /* if(f == '#'){
                    f = '$';
                }
                */

                ti = term_index[(int)f];
                parse_table[lhs_index][ti] = i;
            }
        }
        else if(p[2] < 'A' || p[2] > 'Z'){  //If first is terminal

            ti = term_index[(int)(p[2])];
            parse_table[lhs_index][ti] = i;
        }
        else{   //If not null production, populate for first of current non-terminal

            for(auto f : first[lhs_index]){

                if(f == '#'){
                    for(auto f : follow[lhs_index]){

                        /* if(f == '#'){
                                f = '$';
                            }
                            */

                            ti = term_index[(int)f];
                            parse_table[lhs_index][ti] = i;
                        }
                }

                ti = term_index[(int)f];
                parse_table[lhs_index][ti] = i;
            }
        }
    }
}

void displayParseTable(){

    int index;
    cout<<"\n";
    for(auto t : term){

        cout<<"\t"<<t;
    }

    for(int i=0; i<num_non_term; i++){

        cout<<"\n"<<non_term[i]<<"\t";
        for(int j=0; j<num_term; j++){

            index = parse_table[i][j];
            if(index == -1){

                cout<<"-1\t";
            }
            else{
                
                cout<<productions[index] << "\t";
            }
        }
    }
}



stack<char> s,st_temp;

void push_help_func(int n)
{
    for(int i=productions[n].size()-1;i>=2;i--)
    {
        if(productions[n][i]!='#')
        s.push(productions[n][i]);
        

    }
}

int main()
{
    string buffer="";
    int m,n;
    int i,j;
    i=0;
     acceptProductions();
    calcIndices();
    computeFirst();
    computeFollow();
    generateParseTable();
    displayParseTable();

    cout<<"\nEnter the input string to be checked by parser: ";
    cin>>buffer;
    buffer+='$';
    s.push('$');
    

    s.push(productions[0][0]);
     //this is basically the start variable S(or any other start Non-terminal)
   
    while(i!=buffer.size())
    {
        if(s.top()=='$')
        {
            break;
        }
        if(s.top()!=buffer[i] && isupper(s.top()))
        {
            m=non_term_index[(int)s.top()];
            n=term_index[(int)buffer[i]];
            s.pop();
            if(parse_table[m][n]==-1)
            {
                cout<<" the string cannot be produced using the given grammar!"<<"\n";
                cout<<"exiting....";
                break;
            }
            push_help_func(parse_table[m][n]);
            st_temp = s;
           
            cout<<"\n";
            cout<<"using production: ";
            cout<<productions[parse_table[m][n]];

            cout<<"\nthe contents of stack right now are: ";
            while(!st_temp.empty())
            {
                cout<<st_temp.top()<<",";
                st_temp.pop();
            }



        }

        else
        {
            if(s.top()==buffer[i]){
                i++;
                s.pop();
            }
            else{

                cout<<" the string cannot be produced using the given grammar!"<<"\n";
                cout<<"exiting....";
                break;
            }
        }

        

    }
    if(s.top()=='$')
    cout<<" the string will be accepted by the grammar!";

    else
    cout<<"string is not accepted!";




}
bool Union(set<char> &Set, vector<char> &Vector, bool include_eps){

    bool epsilon = false;
    for(auto ch : Vector){

        if(ch == '#'){
            epsilon = true;
        }
        
        if(ch != '#' || ch == '#' &&  include_eps)
        Set.insert(ch);
    }

    return epsilon;












}


