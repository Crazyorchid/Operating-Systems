#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <bitset>
#include <list>
#include <stdexcept>
#include <functional>
#include <utility>
#include <ctime>
using namespace std;



class customer{
public:
    int id;
    int cur_arr;
    int int_arr;
    int priority;
    int run;
    int tickets;
    int ending;
    int ready;
    int running;
    int waiting;
    bool promoted;
    bool demoted;
    int continue_wait;
    int clock = 0;
};

vector<string> readinfile;
vector<customer>customers;
vector<vector<string>>result;
vector<customer>q1;
vector<customer>q2;


customer new_customer(int id, int arrive, int priority, int tickets){
    customer cus;
    cus.id = id;
    cus.cur_arr = arrive;
    cus.int_arr = arrive;
    cus.priority = priority;
    cus.run = 0;
    cus.tickets = tickets;
    cus.ending = -1;
    cus.ready = -1;
    cus.running = tickets*5;
    cus.waiting = -1;
    cus.promoted = false;
    cus.demoted = false;
    cus.continue_wait = 0;
    return cus;
}

int toInt(string str){
    int res;
    stringstream convert(str);
    convert>> res;
    return res;
}

string toString(int num){
    stringstream t;
    t<< num;
    return t.str();
}

void initial(){
    for(int i = 0; i<readinfile.size();i++){
        string s = readinfile[i];
        string buf;
        stringstream ss(s);
        int c = 0;
        int id,arrive,priority,age,tickets;
        while(ss>>buf){
            if(c==0)id = toInt(buf.substr(1,buf.size()-1));
            if(c==1)arrive = toInt(buf);
            if(c==2)priority = toInt(buf);
            if(c==3)age = toInt(buf);
            if(c==4)tickets = toInt(buf);
            c++;
        }
        customers.push_back(new_customer(id,arrive,priority,tickets));
    }
}


void output()
{
    int i;
    cout<<"name   arrival   end   ready   running   waiting"<<endl;
    for(i=0; i<result.size(); i++) // every result
    {

    }
    cout<<endl;
}

int findCustomerId(int id){

    return id;

}

int Q1sizeNot0(){
    int res = 0;
    for(int i=1; i<q1.size();i++){
        customer pre = q1[res];
        customer cur = q1[i];

        if(cur.priority<pre.priority) res = i;
        else if(cur.priority == pre.priority){
            if(cur.cur_arr < pre.cur_arr) res =i;
            else if(cur.cur_arr == pre.cur_arr){
                if(!cur.promoted && pre.promoted) res =i;
                else if (cur.promoted && pre.promoted){
                    if(cur.tickets<pre.tickets) res = i;
                    else if(cur.tickets==pre.tickets) res = cur.id<pre.id? i:res;
                }else if (!cur.promoted && !pre.promoted){
                    if(cur.ready==-1 && pre.ready!=-1) res =i;
                    else if(cur.ready!=-1 && pre.ready==-1) {}
                        else res = cur.id < pre.id ? i : res;
                }
            }
        }
    }
    return findCustomerId(q1[res].id);
}



void serve(int index)
{
    customer cur = customers[index];

    if(cur.ready == -1) cur.ready = clock;
    if(cur.cur_arr > clock) clock = cur.cur_arr;

    clock += t*5;
    cur.tickets -= t*5;

    if(cur.tickets == 0)
    {
        else
        {
            if(cur.priority <=3)
            {
                cur.runs ++;
                if(cur.runs == 2)
                {
                    cur.priority++;
                    cur.runs = 0;
                    if(cur.priority ==4)
                    {
                        cur.demoted = true;
                    }
                }
            }
        }
    }
}

int main(int argc,char *argv[])
{
    cout<<findCustomerId(q1[1].id)<<endl;

    ifstream in(argv[i]);
    string line;
    while(getline(in,line))
    {
        readinfile.push_back(line);
    }
    //int i,j,k;
    //freopen(argv[1],"r",stdin);

    initial();        // initial data
    while(customers.size()>0 && q1.size()>0 && q2.size()>0)
    {
        int index = pickNext();
        serve(index);
        q1.clear();
        q2.clear();
    }
    //input();          // input data
    //works();          // process data
    output();         // output result
    return 0;
}


