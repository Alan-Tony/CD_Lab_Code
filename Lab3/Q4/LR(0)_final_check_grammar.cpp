#include <bits/stdc++.h>
using namespace std;
vector<vector<string>> parse_table;
int non_term_index[256];
int term_index[256];
vector<char> non_term;
vector<char> term;

vector<string> productions;
int num_term;
int num_non_term;
int num_prod;
map<char, vector<int>> where;
vector<set<string>> states;
vector<int> char_dotPos;
map<string, int> stateFromItem;

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


void check_string()
{
    /*
    productions.push_back("Z=S");
    productions.push_back("S=AA");
    productions.push_back("A=aA");
    productions.push_back("A=b");
    
    terminals_list.push_back('a');
    terminals_list.push_back('b');
    terminals_list.push_back('$');
    non_terminals_list.push_back('S');
    non_terminals_list.push_back('A');
    num_terminals=terminals_list.size();
    num_non_terminals=non_terminals_list.size();
    vector<string> temp0;
    temp0.push_back("s3");
    temp0.push_back("s4");
    temp0.push_back(";");
    temp0.push_back("2");
    temp0.push_back("1");
    vector<string> temp1;
    temp1.push_back(";");
    temp1.push_back(";");
    temp1.push_back("a");
    temp1.push_back(";");
    temp1.push_back(";");
    vector<string> temp2;
    temp2.push_back("s3");
    temp2.push_back("s4");
    temp2.push_back(";");
    temp2.push_back("5");
    temp2.push_back(";");
    vector<string> temp3;
    temp3.push_back("s3");
    temp3.push_back("s4");
    temp3.push_back(";");
    temp3.push_back("6");
    temp3.push_back(";");
    vector<string> temp4;
    temp4.push_back("r3");
    temp4.push_back("r3");
    temp4.push_back("r3");
    temp4.push_back(";");
    temp4.push_back(";");
    vector<string> temp5;
    temp5.push_back("r1");
    temp5.push_back("r1");
    temp5.push_back("r1");
    temp5.push_back(";");
    temp5.push_back(";");
    vector<string> temp6;
    temp6.push_back("r2");
    temp6.push_back("r2");
    temp6.push_back("r2");
    temp6.push_back(";");
    temp6.push_back(";");

    parse_table.push_back(temp0);
    parse_table.push_back(temp1);
    parse_table.push_back(temp2);
    parse_table.push_back(temp3);
    parse_table.push_back(temp4);
    parse_table.push_back(temp5);
    parse_table.push_back(temp6);

*/

    int i,j,k=0,l;
   /* for(i=0;i<256;i++)
    {
        terminals.push_back(';');
        non_terminals.push_back(';');
    }
    terminals[(int)'a']=0;
    terminals[(int)'b']=1;
    terminals[(int)'$']=2;
    non_terminals[(int)'S']=1;
    non_terminals[(int)'A']=0;
    */
    int top_current;
    int index;

    int m,n;
    stack<char> st;
    int main_flag=1;
    st.push('0');
    string buffer;
    /*string buffer="aabb$";
    string temp="";*/
    string temp="";
    cout<<"\nEnter the input string: ";
    cin>>buffer;
    buffer.push_back('$');
    i=0;
    while(i!=buffer.size())
    {
        k++;
        m=term_index[(int)buffer[i]];
        n=st.top()-'0';
        int flag=0;
        int count=0;
        string s=parse_table[n][m];
        if(s=="-1")
        {
            main_flag=0;
            break;
        }
        if(s=="a")
        {
            cout<<"string is accepted!";
            cout<<"\n";
            main_flag=1;
            break;
        }

        if(s[0]=='s')
        {
            st.push(buffer[i]);
            st.push(s[1]);
            i++;
        }

        else
        {
            if(s[0]=='r')
            {
                temp=productions[(int)s[1]-'0'];
                for(l=2;l!=temp.length();l++)
                count++;

                count*=2;

                while(count!=0)
                {
                    if(st.empty())
                    {
                        flag=1;
                        break;
                    }
                    st.pop();
                    count--;

                }
                char top_rn=st.top();

                if(flag==1)
                {
                    cout<<" string is rejected!!";
                    cout<<"\n";
                    break;
                }

                st.push(productions[s[1]-'0'][0]);
                top_current=(int)(top_rn-'0');
                index=non_term_index[(int)st.top()];
                string c=parse_table[top_current][index+num_term];
                if(c=="-1")
                {
                    main_flag=0;
                    break;
                }
                st.push(c[0]);

            }
        }
    }

    if(main_flag==0)
    {
        cout<<"string is rejected!";
        cout<<"\n";
    }

}

int main()
{
    acceptProductions();
    calcIndices();
    findStates();
    printStatesPT();
    check_string();

    cout<<"\n";
}


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