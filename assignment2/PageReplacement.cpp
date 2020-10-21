#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <algorithm>

#define DEBUG 0

using namespace std;

class Args{
    public:
        string fileName;
        string algorithm;
        int pageSize;
        int frameNumber;
        int diskWrites;
        int diskReads;
        int pageFault;
        int interval;
        int winLength;
        int bitLength;

        Args(){   
            winLength = 8;
            bitLength = 8;
            interval = 1;
        }
}arg;

class Cmd{
    public:
        unsigned int address;
        unsigned int pageNumber;
        char RW;
}cmd;
vector<Cmd> comandList;
int cmdIndex = 0;

class Frame{
    public:
        int index;
        int FIFO;
        int LRU;
        unsigned char shift;
        char readWrite;
        unsigned int pageNumber;

        Frame(int i){
            index = i;
            FIFO = -1;
            LRU = 0;
            shift = 0;
            readWrite = 'R';
            pageNumber = -1;
        }
        void hit(int a){
            if (a != 0)
                shift |= 1 << (a - 1);
        }
        void old(){
            shift >>= 1;
        }
            
};
bool FIFO(const Frame & a, const Frame & b){
    return a.FIFO < b.FIFO;
}
bool LRU(const Frame & a, const Frame & b){
    return a.LRU < b.LRU;
}
bool ARB(const Frame & a, const Frame & b){
    if (a.shift == b.shift){
        return a.FIFO < b.FIFO;
    }else{
        return a.shift < b.shift;
    }
}

bool WSARB1(const Frame & a, const Frame & b){
    int frameA = 0, frameB = 0;
    for (int i = 1 + cmdIndex - arg.winLength; i <= cmdIndex; i++){
        if (i >= 0){
            if (comandList[i].pageNumber == a.pageNumber)
                frameA++;
            if (comandList[i].pageNumber == b.pageNumber)
                frameB++;
        } 
    }
    if(frameA == frameB){
        return ARB(a, b);
    }else{
        return frameA < frameB;
    }
}

bool WSARB2(const Frame & a, const Frame & b){
    int frameA = 0, frameB = 0;
    for (int i = 1 + cmdIndex - arg.winLength; i <= cmdIndex; i++){
        if (i >= 0){
            if (comandList[i].pageNumber == a.pageNumber)
                frameA++;
            if (comandList[i].pageNumber == b.pageNumber)
                frameB++;
        }  
    }
    if(frameA == frameB){
        return ARB(a, b);
    }else{
        return frameA > frameB;
    }
}

int FindVictim(Args &arg, vector<Frame> &frameList){
    for (int i = 0; i < frameList.size(); i++){
        if (frameList[i].pageNumber == -1){
            return i;
        }
    }

    vector<Frame> flist = frameList;
    if (arg.algorithm == "FIFO"){
        sort(flist.begin(), flist.end(), FIFO);
    }
    else if (arg.algorithm == "LRU"){
        sort(flist.begin(), flist.end(), LRU);
    }
    else if (arg.algorithm == "ARB"){
        sort(flist.begin(), flist.end(), ARB);
    }
    else if (arg.algorithm == "WSARB-1"){
        sort(flist.begin(), flist.end(), WSARB1);
    }
    else if (arg.algorithm == "WSARB-2"){
        sort(flist.begin(), flist.end(), WSARB2);
    }else{
        cout<<"please input write algorithm";
    }

    return flist[0].index;
}
void Run(Args &arg, vector<Frame> &frameList, vector<Cmd> &comandList)
{
    int i;
    int victim;
    for (cmdIndex = 0; cmdIndex < comandList.size(); cmdIndex++){
        if (DEBUG){
            cout << "Count:" << cmdIndex + 1 << "  ";
        }
        if (cmdIndex%arg.interval == 0){
            for (int fdx = 0; fdx < frameList.size(); fdx++){
                frameList[fdx].old();
            }
        }

        Cmd cmd = comandList[cmdIndex];
        for (i = 0; i < frameList.size(); i++){
            if (frameList[i].pageNumber == cmd.pageNumber){
                break;
            }
        }
        if (i == frameList.size()){
            arg.diskReads++;
            arg.pageFault++;
            if (DEBUG){
                cout << "Miss Page " << cmd.pageNumber << "  ";
            }
            victim = FindVictim(arg, frameList);

            if (DEBUG){
                cout << "Replace page " << frameList[victim].pageNumber << " ";
            }
            if (frameList[victim].readWrite == 'W'){
                arg.diskWrites++;
            }
            frameList[victim].FIFO = cmdIndex;
            frameList[victim].LRU = cmdIndex;
            frameList[victim].shift = 0;
            frameList[victim].pageNumber = cmd.pageNumber;
            frameList[victim].readWrite = cmd.RW;
            
            frameList[victim].hit(arg.bitLength);
        }
        else{
        
            frameList[i].hit(arg.bitLength);
            frameList[i].LRU = cmdIndex;
            
            if (cmd.RW == 'W'){
                frameList[i].readWrite = cmd.RW;
            }
            if (DEBUG){
                cout << "Hit Page " << frameList[i].pageNumber << "  ";
            }
        }
        if (DEBUG){
            cout << " frames ";
            for (int fdx = 0; fdx < frameList.size(); fdx++){
                cout << frameList[fdx].pageNumber << " " << frameList[fdx].readWrite << " " << bitset<8>(frameList[fdx].shift) << "   ";
            }
            cout << endl;
        }
    }
}
int main(int argc, char *argv[]){

    if (argc >= 5){
        arg.fileName = argv[1];

        arg.pageSize = atoi(argv[2]);
        arg.frameNumber = atoi(argv[3]);
        arg.algorithm = argv[4];

        if (arg.algorithm == "ARB" || arg.algorithm == "WSARB-1" || arg.algorithm == "WSARB-2"){
            arg.bitLength = atoi(argv[5]);
            arg.interval = atoi(argv[6]);
        }
        if (arg.algorithm == "WSARB-1" || arg.algorithm == "WSARB-2"){
            arg.winLength = atoi(argv[7]);
        }
        arg.diskWrites = 0;
        arg.diskReads = 0;
        arg.pageFault = 0;

        Cmd cmd;
        ifstream infile;
        infile.open(arg.fileName.c_str());

        char line[100];
        while (infile.peek()!=EOF){   
            infile.getline(line,100);
            if (line[0] == 'R' || line[0] == 'W'){   
                sscanf(line, "%c%x", &cmd.RW, &cmd.address);
                cmd.pageNumber = cmd.address / arg.pageSize;
                comandList.push_back(cmd);
            }
        }
        infile.close();

        vector<Frame> frameList;
        for (int i = 0; i < arg.frameNumber; i++){
            frameList.push_back(Frame(i));
        }

        Run(arg, frameList, comandList);
        cout<<std::left<<setw(20)<<"events in trace:"<<std::right<<comandList.size()<<endl;
        cout<<std::left<<setw(20)<<"total disk reads:"<<std::right<<arg.diskReads<<endl;
        cout<<std::left<<setw(20)<<"total disk writes:"<<std::right<<arg.diskWrites<<endl;
        cout<<std::left<<setw(20)<<"page frameAults:"<<std::right<<arg.pageFault<<endl;
    }
    else{
      cout<<"Please provide more than 5 arguments"<<endl;
    }
    return 0;
}