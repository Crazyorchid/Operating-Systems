//
// Created by runtao on 17/9/20.
//

#include "TicketBooker.h"
#define MAX_PNUMBERS 50
#define UNIT_TIME 5
#define QUATEMAX 10
#define MAX_NUMBER 100000
#define MAX_TIME 5000
#define THRESHOLD 3
#define PRIORITYNUMBER 7

#include "iostream"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector"
#include "queue"

using namespace  std;

typedef struct process // make everything pyblic and define the structure type of the vars
{
    char pID[16];
    int arriveTime;
    int priorityNumber;
    int age;
    int tickets;
    int endTime;
    int readyTime;
    int runningTime;
    int waitingTime;
    int requiredRunTime;
    int comeTime;
    int leaveTime;

    bool inQueue;
    bool outing;
    bool runnning;


} Process;

Process person[MAX_PNUMBERS];
int PNumber;
int Time = 0;
bool newArrivals = true;


/*class OSQueue
{
public:
    vector<Process> vec;

    OSQueue(){};
    ~OSQueue();
};*/
deque<Process>arrayQ1[PRIORITYNUMBER];
queue<Process>finalQ;
deque<Process>queue2;

//compare the arrival time between two person, if the arriving time are the same then comapring the ID;
//Order of simultaneous arrivals of same-priority customers
struct contrast
{
    bool operator()(process a, process b){
        if (a.arriveTime !=b.arriveTime){
            return a.arriveTime > b.arriveTime;
        }else{
            return strcmp(a.pID, b.pID) > 0;
        }

    }
};

//Q A-->B-->C
priority_queue<Process, vector<Process>, contrast> firstPrioQ;
priority_queue<Process, vector<Process>, contrast> secondPrioQ;

//read the flie and implement the
void readTxt(char **argv)
{
    int i = 0;
    freopen(argv[1],"r",stdin);
    while (scanf("%s %d %d %d %d", person[i].pID, &person[i].arriveTime, &person[i].age, &person[i].priorityNumber, &person[i].tickets) != EOF);
    {

        person[i].readyTime = -MAX_NUMBER;
        person[i].runningTime = 0;
        person[i].waitingTime = 0;
        person[i].comeTime = 0;
        person[i].leaveTime = 0;
        person[i].inQueue = false;
        person[i].runnning = false;
        person[i].outing = false;
        person[i].requiredRunTime = UNIT_TIME * person[i].tickets;
        i++;
    }
    PNumber = i;

}

bool missionNotEnd() {
    int i = 0;
    while (i < PNumber) {
        if (!person[i].outing)
            return true;
        i++;
    }
    return false;
}
    void RefreshQueue() {
        while (!firstPrioQ.empty())
            firstPrioQ.pop();
        while (!secondPrioQ.empty())
            secondPrioQ.pop();
        for (int i = 0; i < PNumber; i++) {
            if (Time >= person[i].arriveTime && person[i].inQueue == false) {
                person[i].inQueue = true;
                if (person[i].priorityNumber <= THRESHOLD) {
                    firstPrioQ.push(person[i]);
                } else {
                    secondPrioQ.push(person[i]);
                }
            }
        }
        while (!firstPrioQ.empty()) {
            Process x = firstPrioQ.top();
            firstPrioQ.pop();
            arrayQ1[x.priorityNumber - 1].push_back(x);
        }
        while (!secondPrioQ.empty()) {
            Process x = secondPrioQ.top();
            secondPrioQ.pop();
            queue2.push_back(x);
        }
    }
    void AgingMech() {
        deque<Process> temp;
        while (queue2.empty() == false) {
            Process x = queue2.front();
            queue2.pop_front();
            if (x.age >= 10) {
                x.age = 0;
                bool panduan = x.priorityNumber <= 1;
                if (panduan == false)
                    x.priorityNumber -= 1;
            }
            if (x.priorityNumber <= 3) {
                x.comeTime = 0;
                int index = x.priorityNumber - 1;
                arrayQ1[index].push_back(x);
            } else
                temp.push_back(x);
        }
        queue2 = temp;
    }
    void CheckFormP() {
        while (firstPrioQ.size() != 0)
            firstPrioQ.pop();
        while (secondPrioQ.size() != 0)
            secondPrioQ.pop();
        int k;
        for (k = 0; k < PNumber; k++) {
            if (person[k].inQueue == false && person[k].arriveTime <= Time) {
                person[k].inQueue = true;
                if (person[k].priorityNumber <= 3) {
                    firstPrioQ.push(person[k]);
                } else if (person[k].priorityNumber > 3) {
                    secondPrioQ.push(person[k]);
                }
            }
        }
        while (firstPrioQ.empty() == false) {
            Process t = firstPrioQ.top();
            int index = t.priorityNumber - 1;
            arrayQ1[index].push_back(t);
            firstPrioQ.pop();
        }
        while (secondPrioQ.empty() == false) {
            queue2.push_back(secondPrioQ.top());
            secondPrioQ.pop();
        }
    }

    bool Q1ArrayExcute() {
        bool IND = false;
        for (int i = 0; i < PRIORITYNUMBER; i++) {
            if (!IND) {
                if (arrayQ1[i].size() > 0) {
                    Process curP = arrayQ1->front();
                    arrayQ1[i].pop_front();
                    curP.age = 0;

                    if (curP.runnning == false) {
                        curP.readyTime = Time;
                        curP.outing = true;
                    }
                    int timeleft = curP.requiredRunTime - curP.runningTime;
                    int weighted_time_quantum = (QUATEMAX - curP.priorityNumber) * 10;
                    int readrunnimgTime = min(timeleft, weighted_time_quantum);

                    curP.runningTime = curP.runningTime + readrunnimgTime;
                    Time = Time + readrunnimgTime;
                    if (weighted_time_quantum < timeleft) {
                        curP.comeTime = curP.comeTime + 1;
                    }
                    deque<Process> tmpQ;
                    while (!queue2.empty()) {
                        Process x = queue2.front();
                        x.age++;
                        tmpQ.push_back(x);
                        queue2.pop_front();
                    }
                    queue2 = tmpQ;

                    CheckFormP();

                    if (curP.runnning < curP.requiredRunTime) {
                        if (curP.comeTime >= 3) {
                            curP.comeTime = 0;
                            curP.priorityNumber = curP.priorityNumber + 1;
                        }
                        if (curP.priorityNumber <= 3) {
                            int index = curP.priorityNumber - 1;
                            arrayQ1[index].push_back(curP);
                        } else if (curP.priorityNumber > 3) {
                            queue2.push_back(curP);
                        }
                    } else {
                        curP.endTime = Time;
                        curP.outing = true;
                        for (int i = 0; i < PNumber; i++) {
                            if (person[i].pID == curP.pID)
                                person[i].outing = true;
                        }
                        curP.waitingTime = curP.endTime - curP.readyTime - curP.runningTime;
                        finalQ.push(curP);
                    }
                    AgingMech();
                    IND = true;
                }

            } else
                break;
        }
        return IND;
    }

    Process acIP(int time) {
        Process x;
        priority_queue<Process, vector<Process>, contrast> Q1;
        x.priorityNumber = PRIORITYNUMBER;
        x.arriveTime = time;
        int index = 0;
        while (index < PNumber) {
            if (person[index].arriveTime <= time && person[index].inQueue == false) {
                Q1.push(person[index]);
            }
            index++;
        }
        if (!Q1.empty())
            x = Q1.top();
        return x;
    }

    bool Que2Excute(bool QE) {

        if (QE == true)
            return false;
        else if (queue2.empty() == false) {
            Process curP = queue2.front();
            queue2.pop_front();
            curP.age = 0;
            if (curP.runnning == false) //first run
            {
                curP.readyTime = Time;
                curP.runnning = true;
            }

            int leftTime = curP.tickets * UNIT_TIME - curP.runningTime;
            int weighted_time_quantum = 20 * UNIT_TIME - curP.leaveTime;
            int gettorunningTime = min(leftTime, weighted_time_quantum);
            bool interuptFLAG = true;

            int TogetTime = gettorunningTime + Time;
            Process x = acIP(TogetTime);
            int torunRealTime = x.arriveTime - Time; //real excut time

            curP.runningTime += torunRealTime;
            Time += torunRealTime;

            if (newArrivals) {
                deque<Process> tempQueue;
                while (!queue2.empty()) {
                    Process x = queue2.front();
                    x.age++;
                    tempQueue.push_back(x);
                    queue2.pop_front();
                }
                queue2 = tempQueue;
                AgingMech();
            }
            if (torunRealTime < gettorunningTime) {
                if (x.priorityNumber > 3)
                    interuptFLAG = false;
                else if (x.priorityNumber < curP.priorityNumber) {
                    interuptFLAG = true;
                    curP.leaveTime = 0;
                    curP.comeTime += 1;
                }
            } else {
                curP.leaveTime = 0;
                if (weighted_time_quantum < leftTime)
                    curP.comeTime += 1;
            }

            if (!interuptFLAG) //must not finish ,don't need age++
            {
                curP.leaveTime += torunRealTime;
                queue2.push_front(curP);
                newArrivals = false;
                return true;
            } else {
                newArrivals = true;
            }

            if (curP.runningTime < curP.requiredRunTime) //not finish
            {
                queue2.push_back(curP);
            } else {
                curP.endTime = Time;
                curP.outing = true;
                for (int i = 0; i < PNumber; i++) {
                    if (person[i].pID == curP.pID)
                        person[i].outing = true;
                }
                curP.waitingTime = curP.endTime - curP.readyTime - curP.runningTime;
                finalQ.push(curP);
            }
            CheckFormP();
            return true;
        } else
            return false;
    }

    int main(int argc, char **argv) {
        readTxt(argv);
        while (missionNotEnd() && Time <= MAX_TIME) {
            RefreshQueue();

            bool QE = Q1ArrayExcute();
            bool QE2 = Que2Excute(QE);
            if (QE == false && QE2 == false)
                Time = Time + 5;
        }

        printf("name arrival end ready running waiting\n");
        while (finalQ.empty() == false)
        {
            Process temp = finalQ.front();
            finalQ.pop();
            printf("%s %d %d %d %d %d\n", temp.pID, temp.arriveTime, temp.endTime, temp.readyTime, temp.runningTime,
                   temp.waitingTime);
        }
        return 0;

    }




