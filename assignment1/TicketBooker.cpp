#define MAX_PNUMBERS 50
#define MAX_NUMBER 10000
#define MAX_TIME 5000
#define THRESHOLD 3
#define PRIORITYN 7
#define QUATEMAX 10
#define UNIT_TIME 5
#include "iostream"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector"
#include "queue"
using namespace std;
typedef struct person
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
    int needRunTime;
    int conTime;
    int goneTime;

    bool inQueue;
    bool hasout;
    bool hasrun;
} Person;
Person persons[MAX_PNUMBERS];
int personsNumber;
bool newPersonCome = true;
int TIME = 0;

class MyQueue// setting up the quene and store the values
{

private:
    vector<Person> vec;

public:
    MyQueue(){};
    ~MyQueue(){};
    bool isEmpty()
    {
        return vec.size() == 0;
    }
    void Clear()
    {
        vec.clear();
    }
    int Size()
    {
        return vec.size();
    }
    void Pushfront(person a)
    {
        if (vec.size() > 0)
        {
            Person x = vec[vec.size() - 1];
            vec.push_back(x);
            for (int i = 1; i < vec.size(); i++)
            {
                vec[i] = vec[i - 1];
            }
            vec[0] = a;
        }
        else
        {
            vec.push_back(a);
        }
    }
    void Pushback(person a)
    {
        vec.push_back(a);
    }
    void Popback()
    {
        vec.pop_back();
    }
    void Popfront()
    {
        int i = 0;
        while (i < vec.size() - 1) {
            vec[i] = vec[i + 1];
            i++;
        }
        vec.pop_back();
    }
    Person Front()
    {
        return vec[0];
    }
    Person Back()
    {
        return vec[vec.size() - 1];
    }
};

deque<Person> arrayQ1[PRIORITYN];
deque<Person> queue2;
queue<Person> resultQueue;

struct compare
{
    bool operator()(person a, person b)
    {
        if (a.arriveTime != b.arriveTime)
            return a.arriveTime > b.arriveTime;
        else
        {
            return strcmp(a.pID, b.pID) < 0;
        }
    }
};
priority_queue<Person, vector<Person>, compare> priQ1;
priority_queue<Person, vector<Person>, compare> priQ2;

void readTxt(char **argv)
{
    int i = 0;
    freopen(argv[1], "r", stdin);
    while (scanf("%s %d %d %d %d", persons[i].pID, &persons[i].arriveTime, &persons[i].priorityNumber, &persons[i].age, &persons[i].tickets) != EOF)
    {
        //continue

        persons[i].endTime = MAX_NUMBER;
        persons[i].readyTime = -MAX_NUMBER;
        persons[i].runningTime = 0;
        persons[i].waitingTime = 0;
        persons[i].conTime = 0;
        persons[i].goneTime = 0;
        persons[i].inQueue = false;
        persons[i].hasrun = false;
        persons[i].hasout = false;
        persons[i].needRunTime = UNIT_TIME * persons[i].tickets;
        i++;
    }
    personsNumber = i;
}
bool missionNotEnd()
{
    for (int i = 0; i < personsNumber; i++) {


        {
            if (!persons[i].hasout)
                return true;
        }
    }
    return false;
}
void QueueUpdate()
{
    while (!priQ1.empty())
        priQ1.pop();
    while (!priQ2.empty())
        priQ2.pop();
    for (int i = 0; i < personsNumber; i++)
    {
        if (TIME >= persons[i].arriveTime && persons[i].inQueue == false)
        {
            persons[i].inQueue = true;
            if (persons[i].priorityNumber <= THRESHOLD)
            {
                priQ1.push(persons[i]);
            }
            else
            {
                priQ2.push(persons[i]);
            }
        }
    }
    while (!priQ1.empty())
    {
        Person x = priQ1.top();
        priQ1.pop();
        arrayQ1[x.priorityNumber - 1].push_back(x);
    }
    while (!priQ2.empty())
    {
        Person x = priQ2.top();
        priQ2.pop();
        queue2.push_back(x);
    }
}
void AgeCqueue()
{
    deque<Person> tempQueue;
    while (queue2.empty() == false)
    {
        Person x = queue2.front();
        queue2.pop_front();
        if (x.age >= 8)
        {
            x.age = 0;
            bool judge = x.priorityNumber <= 1;
            if (judge == false)
                x.priorityNumber -= 1;
        }
        if (x.priorityNumber <= 3)
        {
            x.conTime = 0;
            int index = x.priorityNumber - 1;
            arrayQ1[index].push_back(x);
        }
        else
            tempQueue.push_back(x);
    }
    queue2 = tempQueue;
}
void CheckFormP()
{
    while (priQ1.size() != 0)
        priQ1.pop();
    while (priQ2.size() != 0)
        priQ2.pop();
    int k;
    for (k = 0; k < personsNumber; k++)
    {
        if (persons[k].inQueue == false && persons[k].arriveTime <= TIME)
        {
            persons[k].inQueue = true;
            if (persons[k].priorityNumber <= 3)
            {
                priQ1.push(persons[k]);
            }
            else if (persons[k].priorityNumber > 3)
            {
                priQ2.push(persons[k]);
            }
        }
    }
    while (priQ1.empty() == false)
    {
        Person t = priQ1.top();
        int index = t.priorityNumber - 1;
        arrayQ1[index].push_back(t);
        priQ1.pop();
    }
    while (priQ2.empty() == false)
    {
        queue2.push_back(priQ2.top());
        priQ2.pop();
    }
}
bool Que1ArrayExcute()
{
    //bool Result = false;
    bool FLAG = false;
    for (int i = 0; i < PRIORITYN; i++)
    {
        if (!FLAG)
        {
            if (arrayQ1[i].size() > 0)
            {
                Person thisP = arrayQ1[i].front();
                arrayQ1[i].pop_front();
                thisP.age = 0;

                if (thisP.hasrun == false)
                {
                    thisP.readyTime = TIME;
                    thisP.hasrun = true;
                }
                int tempremainingT = thisP.needRunTime - thisP.runningTime;
                int weighted_time_quantum = (QUATEMAX - thisP.priorityNumber) * 10;
                int TorunningTime = min(tempremainingT, weighted_time_quantum);

                thisP.runningTime = thisP.runningTime + TorunningTime;
                TIME = TIME + TorunningTime;
                if (weighted_time_quantum < tempremainingT)
                {
                    thisP.conTime = thisP.conTime + 1;
                }

                //queue2 age plus
                deque<Person> tempQueue;
                while (!queue2.empty())
                {
                    Person x = queue2.front();
                    x.age++;
                    tempQueue.push_back(x);
                    queue2.pop_front();
                }
                queue2 = tempQueue;

                CheckFormP();

                if (thisP.runningTime < thisP.needRunTime) //not finish
                {
                    if (thisP.conTime >= 3)
                    {
                        thisP.conTime = 0;
                        thisP.priorityNumber = thisP.priorityNumber + 1;
                    }
                    if (thisP.priorityNumber <= 3)
                    {
                        int index = thisP.priorityNumber - 1;
                        arrayQ1[index].push_back(thisP);
                    }
                    else if (thisP.priorityNumber > 3)
                    {
                        queue2.push_back(thisP);
                    }
                }
                else
                {
                    thisP.endTime = TIME;
                    thisP.hasout = true;
                    for (int i = 0; i < personsNumber; i++)
                    {
                        if (persons[i].pID == thisP.pID)
                            persons[i].hasout = true;
                    }
                    thisP.waitingTime = thisP.endTime - thisP.readyTime - thisP.runningTime;
                    resultQueue.push(thisP);
                }
                AgeCqueue();
                FLAG = true;
            }
        }
        else
            break;
    }
    return FLAG;
}
Person getIP(int time)
{

    Person x;
    priority_queue<Person, vector<Person>, compare> priQ;
    x.priorityNumber = PRIORITYN;
    x.arriveTime = time;
    int index = 0;
    while (index < personsNumber)
    {
        if (persons[index].arriveTime <= time && persons[index].inQueue == false)
        {
            priQ.push(persons[index]);
        }
        index++;
    }
    if (!priQ.empty())
        x = priQ.top();
    return x;
}

bool Que2Excute(bool QE)
{

    if (QE == true)
        return false;
    else if (queue2.empty() == false)
    {
        Person thisP = queue2.front();
        queue2.pop_front();
        thisP.age = 0;
        if (thisP.hasrun == false) //first run
        {
            thisP.readyTime = TIME;
            thisP.hasrun = true;
        }

        int tempremainingT = thisP.tickets * UNIT_TIME - thisP.runningTime;
        int weighted_time_quantum = 20 * UNIT_TIME - thisP.goneTime;
        int TorunningTime = min(tempremainingT, weighted_time_quantum);
        bool interuptFLAG = true;

        int TogetTime = TorunningTime + TIME;
        Person x = getIP(TogetTime);
        int torunRealTime = x.arriveTime - TIME; //real excut time

        thisP.runningTime += torunRealTime;
        TIME += torunRealTime;

        if (newPersonCome)
        {
            deque<Person> tempQueue;
            while (!queue2.empty())
            {
                Person x = queue2.front();
                x.age++;
                tempQueue.push_back(x);
                queue2.pop_front();
            }
            queue2 = tempQueue;
            AgeCqueue();
        }
        if (torunRealTime < TorunningTime)
        {
            if (x.priorityNumber > 3)
                interuptFLAG = false;
            else if (x.priorityNumber < thisP.priorityNumber)
            {
                interuptFLAG = true;
                thisP.goneTime = 0;
                thisP.conTime += 1;
            }
        }
        else
        {
            thisP.goneTime = 0;
            if (weighted_time_quantum < tempremainingT)
                thisP.conTime += 1;
        }

        if (!interuptFLAG) //must not finish ,don't need age++
        {
            thisP.goneTime += torunRealTime;
            queue2.push_front(thisP);
            newPersonCome = false;
            return true;
        }
        else
        {
            newPersonCome = true;
        }

        if (thisP.runningTime < thisP.needRunTime) //not finish
        {
            queue2.push_back(thisP);
        }
        else
        {
            thisP.endTime = TIME;
            thisP.hasout = true;
            for (int i = 0; i < personsNumber; i++)
            {
                if (persons[i].pID == thisP.pID)
                    persons[i].hasout = true;
            }
            thisP.waitingTime = thisP.endTime - thisP.readyTime - thisP.runningTime;
            resultQueue.push(thisP);
        }
        CheckFormP();
        return true;
    }
    else
        return false;
}
int main(int argc, char **argv)
{
    readTxt(argv);
    while (missionNotEnd() && TIME <= MAX_TIME)
    {
        QueueUpdate();

        bool QE = Que1ArrayExcute();
        bool QE2 = Que2Excute(QE);
        if (QE == false && QE2 == false)
            TIME = TIME + 5;
    }

    printf("name arrival end ready running waiting\n");
    while (resultQueue.empty() == false)
    {
        Person temp = resultQueue.front();
        resultQueue.pop();
        printf("%s "    " %d "  " %d "  " %d "  " %d "  " %d\n", temp.pID, temp.arriveTime, temp.endTime, temp.readyTime, temp.runningTime, temp.waitingTime);
    }
    return 0;
}

