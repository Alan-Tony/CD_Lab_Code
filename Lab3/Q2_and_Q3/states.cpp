#include<bits/stdc++.h>
using namespace std;

int num_non_term, num_term;
vector<char> non_term;
vector<char> term;

//Index
int non_term_index[256];
int term_index[256];

vector<string> productions;
int num_prod;

//Where to find productions of a non-terminal
map<char, vector<int>> where;

//States
vector<set<string>> states;
vector<int> char_dotPos;
//Maps the characteristic item to its state number
map<string, int> stateFromItem;

//Parse Table
vector<vector<string>> parse_table;

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

    //Adding production to start state:
    productions.insert(productions.begin(), string("Z=")+productions[0][0]);

    cout<<"\nAugmented Grammar: ";
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

//Function to find the states
string putDot(string st, int pos);
void Closure(int curr_state, queue<int> &state_queue);
void findStates(){

    //Which production has the non-terminal at lhs
    for(unsigned int i=0; i<productions.size(); i++){

        string prod = productions[i];
        auto found = where.find(prod[0]);
        if(found != where.end()){

            found->second.push_back(i);
        }
        else{

            where.insert(make_pair(prod[0], vector<int>({})));
            where.find(prod[0])->second.push_back(i);
        }
    }

    /*
    for(auto it : where){

        cout<<"\n"<<it.first<<" size= "<<it.second.size()<<": ";
        for(auto prod_num : it.second){

            cout<<productions[prod_num]<<"\t";
        }
    }
    */

    //Create state 0
    states.push_back(set<string>());
    string item = putDot(productions[0], 0);
    char_dotPos.push_back(0);
    states[0].insert(item);
    //Map which stores which maps characteristic item to corresponding state
    stateFromItem.insert(make_pair(item, 0));
    //New parse table row
    parse_table.push_back(vector<string>(num_term + num_non_term, "-1"));

    queue<int> state_queue;
    state_queue.push(0);
    int curr_state;
    while(!state_queue.empty()){      //New states were added

        //Pop from queue
        curr_state = state_queue.front();
        state_queue.pop();

        //Compute closure
        Closure(curr_state, state_queue);
    }   

}

//Closure
string swapPos(string st, int pos1, int pos2);
void Closure(int curr_state, queue<int> &state_queue){

    //Get characteristic item of current state
    string char_item = *(states[curr_state].begin());
    int curr_char_dotPos = char_dotPos[curr_state];

    queue<pair<string, int>> item_queue;
    item_queue.push(make_pair(char_item, curr_char_dotPos));
    string curr_item;
    int  dot_pos;
    while(!item_queue.empty()){

        auto res = item_queue.front();
        item_queue.pop();

        curr_item = res.first;
        dot_pos = res.second;

        if(dot_pos == curr_item.length()-1-2){

            //Add reduce or accept to parse table
            if(curr_item[0] == 'Z'){    //Accept

                parse_table[curr_state][num_term - 1] = "a";
            }
            else{   //Reduce

                //Find the production number of the current item
                string reduce_prod = curr_item.substr(0, curr_item.length()-1);
                int reduce_prod_num = -1;
                for(auto prod_num : where.find(reduce_prod[0])->second){

                    if(productions[prod_num] == reduce_prod){

                        reduce_prod_num = prod_num;
                        break;
                    }
                }

                for(unsigned int i=0; i<num_term; i++){
                    
                    parse_table[curr_state][i] = "r" + to_string(reduce_prod_num);    //Change this hard-coded value
                }
            }
        }
        else{
            
            char next_char = curr_item[dot_pos+2+1];

            //Check if state exists. 
            //     a. If it exists update parse tree
            //          Next item
            string shifted_item = swapPos(curr_item, dot_pos+2, dot_pos+2+1);
            //          Check
            auto found = stateFromItem.find(shifted_item);
            int next_state;
            if(found != stateFromItem.end()){

                //Transition to found state
                next_state = found->second;                
                
            }
            //     b. Create new state
            else{
                
                int state_num = states.size();
                states.push_back(set<string>());
                states[state_num].insert(shifted_item);
                char_dotPos.push_back(dot_pos+1);
                //Map which stores which maps characteristic item to corresponding state
                stateFromItem.insert(make_pair(shifted_item, state_num));

                //Transition to new state
                next_state = state_num;

                //Add row to parse table for new state
                parse_table.push_back(vector<string>(num_term + num_non_term, "-1"));

                //Append new state to state_queue
                state_queue.push(state_num);
            }

            //Update parse table row of current state
            int index;
            if(isupper(next_char)){
                
                index = num_term + non_term_index[(int)next_char];
                parse_table[curr_state][index] = to_string(next_state);
            }
            else{

                index = term_index[(int)next_char];
                parse_table[curr_state][index] = "s" + to_string(next_state);
            }

            //Add new items to the state on closure
            if(isupper(next_char)){

                for(auto prod_num : where.find(next_char)->second){     //Iterating throught the set of productions with next_char in lhs

                    auto prod = productions[prod_num];
                    auto new_item = putDot(prod, 0);
                    auto found = states[curr_state].find(new_item);
                    if(found == states[curr_state].end()){
                        
                        item_queue.push(make_pair(new_item, 0));
                        states[curr_state].insert(new_item);
                    }
                }
            }
        }

    }
}

//Print states and parse table
void printStatesPT(){

    cout<<"\nSates: ";
    for(unsigned int i=0; i<states.size(); i++){

        auto state = states[i];
        cout<<"\nState "<<i<<": ";
        for(auto item : state){
            
            cout<<item<<"\t";
        }
    }

    cout<<"\nParse Table: \n\t";
    for(auto ch : term){
        cout<<ch<<"\t";
    }
    for(auto ch : non_term){
        cout<<ch<<"\t";
    }
    for(unsigned int i=0; i<parse_table.size(); i++){

        cout<<"\n"<<i<<"\t";
        for(int j=0; j< num_term+num_non_term; j++){

            cout<<parse_table[i][j]<<"\t";
        }
    }

}

//Driver code
int main(){

    acceptProductions();
    calcIndices();
    findStates();
    printStatesPT();

    cout<<"\n";
    return 0;
}

//Utility code
string putDot(string st, int pos){

    if(pos+2 >= st.length()){
        return st;
    }

    st.insert(st.begin()+pos+2, '.');
    return st;
}

string swapPos(string st, int pos1, int pos2){

    char temp = st[pos2];
    st[pos2] = st[pos1];
    st[pos1] = temp;

    return st;
}