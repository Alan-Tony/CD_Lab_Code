#include <iostream>
#include <cstdio>
#include <vector>
using namespace std;

string p[10];
vector<char> first_set;
vector<char> firstarr[100];
vector<char> followarr[100];
vector<char> non_term;
vector<char>term;
vector<char> second_set;
int g=0,h=0;
char followc[10][100];
char firstc[10][100];

void followf(char c,int b, int a);
void follow_func(char ch);

int count_of_prod,m=0,n=0;

void follow_func(char cha)
{
    int x,y;
    if(cha == p[0][0]) {
        second_set[m++] = '$';
    }
    for(x = 0; x < 10; x++)
    {
        for(y=2;y < 10; y++)
        {
            if(p[x][y] == cha)
            {
                if(p[x][y+1] != '\0')
                {
            
                    followf(p[x][y+1], x, y+2);
                }
                 
                if(p[x][y+1]=='\0' && cha!=p[x][0])
                {
                    follow_func(p[x][0]);
                }
            }
        }
    }

}
void followf(char cha, int x1, int x2)
{
     
    if(!(isupper(cha)))
        second_set[m++] = cha;
    else
    {
        int i = 0, j = 1;
        int k;
        for(i = 0; i < count_of_prod; i++)
        {
            if(firstc[i][0] == cha)
                break;
        }
         
        while(firstc[i][j] != '!')
        {
            if(firstc[i][j] != '#')
            {
                second_set[m++] = firstc[i][j];
            }
            else
            {
                if(p[x1][x2] == '\0')
                {
                    follow_func(p[x1][0]);
                }
                else
                {
                    followf(p[x1][x2], x1, x2+1);
                }
            }
            j++;
        }
    }
}


void FirstFind(int x,char var,int y)
{
    int k;
    if(!(isupper(var)))
    {
        first_set[n++]=var;

    }

    for(k=0;k<count_of_prod;k++)
    {
        if(p[k][0]==var)
        {
            if(p[k][2]=='#')
            {
                if(p[x][y]=='\0')
                {
                    first_set[n++]='#';
                }
                else if((x!=0 || y!=0 ) && p[x][y]!='\0')
                {
                    FirstFind(x,p[x][y],y+1);
                }

                else
                first_set[n++]='#';

            }

            else if(!(isupper(p[k][2])))
            first_set[n++]=p[k][2];

            else
            {
                FirstFind(k,p[k][2],3);
            }
        }
    }
}


int main()
{
    int y=0,z=0;
    
    int i,j,k;
    int down=0;
    cout<<"Enter the Number of Productions: ";
    cin>>count_of_prod;
    

    for(i=0;i<count_of_prod;i++){
    first_set.push_back(0);
    second_set.push_back(0);
    }

    for(k=0;k<10;k++)
    {
    for(int v=0;v<100;v++)
    {
        followc[k][v]='!';
    }
    }

    for(i=0;i<10;i++)
    {
        for(j=0;j<100;j++)
        firstc[i][j]='!';
    }

    for(i=0;i<count_of_prod;i++)
    {
        string temp;
        cout<<"enter "<<i+1<<"th : ";
       cin>>temp;
       for(j=0;j<temp.length();j++)
       {
           if(temp[j]>='A' && temp[j]<='Z'){
               int flag2=0;
            for(int u=0;u<non_term.size();u++)
            {
                if(temp[j]==non_term[u]){
                flag2=1;
                break;
                }

            }
            if(flag2!=1)
           non_term.push_back(temp[j]);
           }


           else if(temp[j]!='=' && temp[j]!='#' && !(temp[j]>='A' && temp[j]<='Z'))
           {
               int flag3=0;

               for(int u=0;u<term.size();u++)
            {
                if(temp[j]==term[u]){
                flag3=1;
                break;
                }

            }
            if(flag3!=1)
           term.push_back(temp[j]);
           }
       }
       p[i]=temp;
       

    }
    
    
    
    int point=-1;
    vector<char> check(count_of_prod,0);
    int co=count_of_prod;
    char check1[100];
    char var;
    int x1=0,x2;
    bool flag=false;
    for(j=0;j<count_of_prod;j++)
    {
        x2=0; flag=false;
        var=p[j][0];

        for(k=0;k<=point;k++)
        {
            if(var==check[k])
            flag=true;
            break;
        }

        if(flag==true)
        continue;

        point=point+1;
        FirstFind(0,var,0);

        check[point]=var;
        cout<<"\n"<<"First("<<var<<")"<<"= { ";
        firstc[x1][x2]=var;
        x2+=1;
        
        for(i=y;i<n;i++)
        {
            int ch=1;
            for(int l=0;l<x2;l++)
            {
                if(firstc[x1][l]==first_set[i])
                {
                    ch=0;
                    break;
                }

            }
            
             if(ch==1)
            {
                cout<<first_set[i]<<", ";
                firstarr[g].push_back(first_set[i]);
                firstc[x1][x2]=first_set[i];
                x2+=1;
            }

           
        }
        g++;
        

    x1+=1;
    y=n;
    cout<<"}"<<"\n";
    
    }

cout<<"\n \n";
point = -1;
x1=0;
for(int f=0;f<count_of_prod;f++)
{
    var = p[f][0];
    x2=0;flag=false;
    for(k = 0; k <= point; k++){
            if(var == check1[k])
                flag = true;
    }


    if(flag==true)
    continue;


    down=down+1;
    point=point+1;
    follow_func(var);
    check1[point]=var;
    cout<<" follow ("<<var<<") = {";
    followc[x1][x2]=var;
    x2++;
    for(i = z; i < m; i++) 
    {
        int last = 0, che = 0;
        for(last = 0; last < x2; last++)
        {
            if (second_set[i] == followc[x1][last])
            {
                che = 1;
                break;
            }
        }
            if(che == 0)
            {
                cout<<second_set[i]<<", ";
                followarr[h].push_back(second_set[i]);
                followc[x1][x2] = second_set[i];
                x2+=1;
            }
    }
    h++;
        
        cout<<" }"<<"\n \n";
        z = m;
        x1++;
    }

    /*for(i=0;i<8;i++)
    {
        for(j=0;j<firstarr[i].size();j++)
        {
            cout<<firstarr[i][j]<<" ";
        
            
        }
        cout<<"\n";
    }*/

    for(i=0;i<non_term.size();i++)
    {
        cout<<non_term[i]<<" ";
    }

    cout<<"\n \n";

    for(i=0;i<term.size();i++)
    {
        cout<<term[i]<<" ";
    }

return 0;


}