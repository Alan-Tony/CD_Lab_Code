/*

1. The states should be represented in the form of vector of map of LR1 items
2. There should be map from characteristic LR1 items to index of state vector (named LR1Map)
3. There should be a map from characteristic LR0 item to a vector of indices of the state vector
(This is because 2 LR1 items can have the same LR0 item but different lookahead sets)
4. LR1 items are pairs of string (LR0) and set of lookahead terminals (char)
5. "where" is a map from string (production) to production index
6. char_dotPos is the position of the '.' in the characteristic LR1 item's corresponding LR0 item (same order as states vector)

*/

#include<bits/stdc++.h>
using namespace std;

vector<char> *first;

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
vector<map<string, set<char>>> states;
vector<int> char_dotPos;
map<pair<string, set<char>>, int> LR1Map;
map<string, vector<int>> LR0Map;

//Useful to determine the order of states while printing from LR0, since LR0 map stores LR0 items
//as key values in alphabetical order of first character rather than order of addition to the map
vector<string> LR0_order;

//Parse Table
vector<vector<string>> parse_table;
vector<vector<string>> final_parse_table;

void calcIndices();
void computeFirst();


//function to create LALR parse table from CLR parse table by merging necassary rows
void mergetable(vector<vector<int>> dup,int num_of_term,int num_of_non_term){

    /*
        dup[i]

        Steps:
        1. min_index = dup[i][0]
        2. For dup[i][j], j>0:
            a. Rename S(dup[i][j]) to S(dup[i][0]) and RHS of table dup[i][j] to dup[i][0] and union (includes R)
        3. For x = dup[i][j], j>0
            a. For y > x : do -- in parse table as well as dup
            b. Delete row x

    */
    
    int i, k,l;
    for(i=0;i<dup.size();i++)
    {
        for(k=0;k<num_term+num_of_non_term;k++)
        {
            for(int j=0;j<dup[i].size();j++)
            {
                if(parse_table[dup[i][j]][k]!="-1")
                {
                    parse_table[dup[i][0]][k]=parse_table[dup[i][j]][k];
                    break;
                }

            }
        }

        for(int m=0;m<parse_table.size();m++)
        {
            for(int n=0;n<parse_table[m].size();n++)
            {
                for(int j=1;j<dup[i].size();j++)
                {
                    if(parse_table[m][n]=="s" + to_string(dup[i][j]))
                    {
                        parse_table[m][n]="s" + to_string(dup[i][0]);
                    }        
                    else if(parse_table[m][n]==to_string(dup[i][j]))
                    {
                        parse_table[m][n]=to_string(dup[i][0]);
                    }
                }
                
            }
        }

        
        
        for(unsigned int j=1; j<dup[i].size(); j++){

            auto x = dup[i][j];
            //Reduce dup elements > x by 1
            for(unsigned p=0; p<dup.size(); p++){

                for(unsigned q=0; q<dup[p].size(); q++){

                    if(dup[p][q] > x){
                        
                        dup[p][q]--;
                    }
                }
            }

            //Reduce parse table entries Sy and y by one if y>x
            for(unsigned p=0; p<parse_table.size(); p++){

                auto row = parse_table[p];
                for(unsigned q=0; q<row.size(); q++){

                    auto entry = row[q];
                    if(entry[0] == 's' && stoi(entry.substr(1)) > x){

                        parse_table[p][q] = "s" + to_string(stoi(entry.substr(1)) - 1);
                    }
                    else if(isdigit(entry[0]) && stoi(entry.substr(1)) > x){

                        parse_table[p][q] = to_string(stoi(entry.substr(1)) - 1);
                    }
                }
            }

            //Delete row j from parse table
            parse_table.erase(parse_table.begin() + x);
        }
    }

}

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

    //Find first and follow
    calcIndices();
    computeFirst();

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
    states.push_back(map<string, set<char>>());
    string LR0Item = putDot(productions[0], 0);
    auto LR1Item = make_pair(LR0Item, set<char>({'$'}));
    char_dotPos.push_back(0);
    states[0].insert(LR1Item);
    //Populate LR0 map
    LR0Map.insert(make_pair(LR0Item, vector<int>({0})));
    LR0_order.push_back(LR0Item);
    //Populate LR1 map
    LR1Map.insert(make_pair(LR1Item, 0));
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
bool Union(set<char> &Set, vector<char> &Vector, bool include_eps);
void Closure(int curr_state, queue<int> &state_queue){

    //Get characteristic item of current state
    auto char_LR1Item = *(states[curr_state].begin());
    int curr_char_dotPos = char_dotPos[curr_state];

    queue<pair<pair<string, set<char>>, int>> item_queue;
    item_queue.push(make_pair(char_LR1Item, curr_char_dotPos));
    pair<string, set<char>> curr_item;
    int  dot_pos;
    while(!item_queue.empty()){

        auto res = item_queue.front();
        item_queue.pop();

        curr_item = res.first;
        dot_pos = res.second;

        if(dot_pos == curr_item.first.length()-1-2){

            //Add reduce or accept to parse table
            if(curr_item.first[0] == 'Z'){    //Accept

                parse_table[curr_state][num_term - 1] = "a";
            }
            else{   //Reduce

                //Find the production number of the current item
                string reduce_prod = curr_item.first;
                reduce_prod.pop_back();     //curr_item.first.substr(0, curr_item.first.length()-1)
                int reduce_prod_num = -1;
                for(auto prod_num : where.find(reduce_prod[0])->second){

                    if(productions[prod_num] == reduce_prod){

                        reduce_prod_num = prod_num;
                        break;
                    }
                }

                for(auto ch : curr_item.second){
                    
                    parse_table[curr_state][term_index[(int)ch]] = "r" + to_string(reduce_prod_num);
                }
            }
        }
        else{
            
            char next_char = curr_item.first[dot_pos+2+1];

            //Check if state exists. 
            //     a. If it exists update parse tree
            //          Next item
            string shifted_LR0Item = swapPos(curr_item.first, dot_pos+2, dot_pos+2+1);
            //          Check
            auto found = LR1Map.find(make_pair(shifted_LR0Item, curr_item.second));
            int next_state;
            if(found != LR1Map.end()){

                //Transition to found state
                next_state = found->second;                
                
            }
            //     b. Create new state
            else{
                
                int state_num = states.size();
                states.push_back(map<string, set<char>>());
                auto new_LR1Item = make_pair(shifted_LR0Item, curr_item.second);
                states[state_num].insert(new_LR1Item);
                char_dotPos.push_back(dot_pos+1);
                //Update LR1 Map
                LR1Map.insert(make_pair(new_LR1Item, state_num));
                //Update LR0 Map
                auto pair_found = LR0Map.find(shifted_LR0Item);
                if(pair_found != LR0Map.end()){

                    pair_found->second.push_back(state_num);
                }
                else{   //Did not find pair

                    //TODO: update order of LR0 items
                    LR0_order.push_back(shifted_LR0Item);
                    LR0Map.insert(make_pair(shifted_LR0Item, vector<int>({state_num})));
                }

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
                    auto new_LR0Item = putDot(prod, 0);

                    //Find set of char
                    set<char> new_LookAhead;
                    if(dot_pos == curr_item.first.length()-2-2){

                        new_LookAhead = curr_item.second;
                    }
                    else{
                        
                        char next_next_char = curr_item.first[dot_pos+2+2];
                        if(isupper(next_next_char)){

                            Union(new_LookAhead, first[non_term_index[(int)next_next_char]], false);
                        }
                    }

                    auto new_LR1Item = make_pair(new_LR0Item, new_LookAhead);
                    auto found = states[curr_state].find(new_LR0Item);
                    if(found != states[curr_state].end()){
                        
                        for(auto ch : new_LookAhead){

                            found->second.insert(ch);
                        }
                    }
                    else{

                        item_queue.push(make_pair(new_LR1Item, 0));
                        states[curr_state].insert(new_LR1Item);
                    }
                }
            }
        }

    }
}

void CLRtoLALR(){
    // vector variable in which each used to store the indices of the rows which need to be unioned
    vector<vector<int>> dup;
    for(auto p: LR0Map)
    {
        if(p.second.size() >=2)
        {
            dup.push_back(p.second);
        }
    }    
    mergetable(dup,num_term,num_non_term);
}

//Print states and parse table
void printStatesPT(){

    cout<<"\n\nSates: ";
    int state_number=0;
    for(auto LR0Item : LR0_order){

        auto p = *LR0Map.find(LR0Item);

        cout<<"\nState "<<state_number<<": ";
        vector<string> LR0items;
        for(auto item : states[p.second[0]]){

            LR0items.push_back(item.first);
        }

        for(auto item : LR0items){

            cout<<item<<" (";
            auto lookAhead = set<char>();
            //Union lookaheads
            for(auto state_num : p.second){

                auto found = states[state_num].find(item);
                for(auto ch : found->second){

                    lookAhead.insert(ch);
                }
            }
            //Print lookaheads
            for(auto ch : lookAhead){

                cout<<ch<<" ";
            }
            cout<<")\t";
        }
        state_number++;
    }

    cout<<"\n\nCLR Parse Table: \n\t";
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
    cout<<"\n\n";

    CLRtoLALR();
    cout<<"\nLALR Parse Table: \n\t";
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

    int i,j,k=0,l;
    int top_current;
    int index;

    int m,n;
    stack<char> st;
    int main_flag=1;
    st.push('0');
    string buffer;

    string temp="";
    cout<<"\n\nEnter the input string: ";
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


//Driver code
int main(){

    acceptProductions();
    findStates();
    printStatesPT();
    check_string();

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

//Compute First
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
    cout<<"\nFirst set:";
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