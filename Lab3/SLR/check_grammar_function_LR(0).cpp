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
    cout<<"enter the input string: ";
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
