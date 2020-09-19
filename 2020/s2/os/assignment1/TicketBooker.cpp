/*
 * @Author: your name
 * @Date: 2020-09-11 14:46:42
 * @LastEditTime: 2020-09-16 19:36:29
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /COMP_SCI_3004:7064/TicketBooker.cpp
 */
#include <algorithm>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <string>
#include <vector>

#define QUEUE1 1
#define QUEUE2 2
#define QUEUE3 3

#define NEW_COME 0
#define EXPIRE 1
#define UPDATE 2
#define GRAB 3

#define MAXLENGTH 100
#define MAXTIME 1000000
#define TICKET_UNIT 5
#define PRIORITY 3
#define AGE_UNIT 100

using namespace std;

// Process
struct Process {
    string name;
    int arrival;
    int end;
    int ready;
    int running;
    int waitting;
    // total required time
    int total;
    int priority;
    // the queue scheduled from
    int from_queue;
    int cur_queue;
    int age;
    int running_time;
    int last_end;
    int queue_arrival;

    bool operator<(const Process &other) const {
        // arriving queue
        while (cur_queue == QUEUE3 && other.cur_queue == QUEUE3)
            return arrival == other.arrival
                       ? stoi(name.substr(1)) > stoi(other.name.substr(1))
                       : arrival > other.arrival;

        while (cur_queue == QUEUE2 && other.cur_queue == QUEUE2) {
            // remaining time
            while (total - running != other.total - other.running)
                return total - running > other.total - other.running;
            while (priority != other.priority)
                return priority > other.priority;
            while (queue_arrival != other.queue_arrival)
                return queue_arrival > other.queue_arrival;

            if (from_queue == QUEUE1)
                return false;
            else
                return true;
        }

        while (cur_queue == QUEUE1 && other.cur_queue == QUEUE1) {
            while (priority != other.priority)
                return priority > other.priority;
            while (queue_arrival != other.queue_arrival)
                return queue_arrival > other.queue_arrival;

            while (from_queue == QUEUE3 && other.from_queue == QUEUE3)
                return stoi(name.substr(1)) > stoi(other.name.substr(1));

            if (from_queue == QUEUE3)
                return false;
            else if (from_queue == QUEUE1)
                return other.from_queue == QUEUE3;
            else if (other.from_queue == QUEUE2) {
                if (total - running != other.total - other.running)
                    return total - running > other.total - other.running;
                if (priority != other.priority)
                    return priority > other.priority;
                if (queue_arrival != other.queue_arrival)
                    return queue_arrival > other.queue_arrival;
                return true;
            }
        }
        return name > other.name;
    }

    void operator=(const Process &other) {
        name = other.name;
        arrival = other.arrival;
        end = other.end;
        ready = other.ready;
        running = other.running;
        waitting = other.waitting;
        total = other.total;
        priority = other.priority;
        from_queue = other.from_queue;
        age = other.age;
        running_time = other.running_time;
        last_end = other.last_end;
        cur_queue = other.cur_queue;
        queue_arrival = other.queue_arrival;
    }
};


priority_queue<Process> read_process(char *filename) {
    priority_queue<Process> jobs;
    FILE *f;
    f = fopen(filename, "r");

    char name[MAXLENGTH];
    int arrival;
    int priority;
    int age;
    int tickets;
//reading the file
    while (fscanf(f, "%s %d %d %d %d\n", name, &arrival, &priority, &age,
                  &tickets) != EOF) {
        Process process;
        process.name = string(name);
        process.arrival = arrival;
        process.age = arrival;
        process.priority = priority;
        process.from_queue = QUEUE3;
        process.total = tickets * TICKET_UNIT;
        process.ready = -1;
        process.end = -1;
        process.running = 0;
        process.waitting = 0;
        process.running_time = 0;
        process.last_end = arrival;
        process.cur_queue = QUEUE3;
        process.queue_arrival = arrival;

        jobs.push(process);
    }

    return jobs;
}

// Get next new_come event time. If not one, return -1.
int next_arrive(priority_queue<Process> q) {
    if(q.empty()==true){
      return MAXTIME;
    }
    if(q.empty()==false){
      return q.top().arrival;
    }
}

// Get next event time
int get_next_arr(int new_come, int end, int update) {
    int next_arr = MAXTIME;
    next_arr =std::min( { MAXTIME,new_come,end,update } );
    
    return next_arr;
}

int schedule_a_job(priority_queue<Process> &que1, priority_queue<Process> &que2, int &start,
                   Process &cur_process, int clock) {
    int end;
    while (que1.empty() && que2.empty())
        return MAXTIME;
    if (!que1.empty()) {
        cur_process = que1.top();
        que1.pop();
    } else if (!que2.empty()) {
        cur_process = que2.top();
        que2.pop();
    }
    if (cur_process.ready != -1)
        cur_process.waitting = cur_process.waitting + clock - cur_process.last_end;
    else
        cur_process.ready = clock;
    start = clock;

    if (cur_process.cur_queue == QUEUE1)
        end = min(10-(cur_process.priority)*10,
                     cur_process.total - cur_process.running) +
                 clock;
    else
        end = cur_process.total - cur_process.running + clock;
    return end;
}

int get_next_update(priority_queue<Process> &q) {
    while (q.empty())
        return MAXTIME;
    int zxsj = MAXTIME;
    Process tmp;
    priority_queue<Process> tmp_queue;

    while (!q.empty()) {
        tmp = q.top();
        zxsj = min(tmp.age + AGE_UNIT, zxsj);
        q.pop();
        tmp_queue.push(tmp);
    }
    q = tmp_queue;
    return zxsj;
}

void update_job(priority_queue<Process> &que1, priority_queue<Process> &que2, int update) {
    if (que2.empty())
        return;

    Process process;
    priority_queue<Process> tmp;
    while (!que2.empty()) {
        process = que2.top();
        que2.pop();
        while (process.age + AGE_UNIT == update) {
                process.age += AGE_UNIT;
                process.priority--;
        }
        if (process.priority <= PRIORITY) {
            process.from_queue = process.cur_queue;
            process.cur_queue = QUEUE1;
            process.running_time = 0;
            process.queue_arrival = update;
            que1.push(process);
        } else {
            tmp.push(process);
        }
    }
    que2 = tmp;
}

// Simulate the scheduling
vector<Process> simulate(char *filename) {
    // finished process vector
    vector<Process> finished;
    // queue 1, queue 2 and arriving queue 3
    priority_queue<Process> que1;
    priority_queue<Process> que2;
    priority_queue<Process> que3;

    // load jobs from input file
    que3 = read_process(filename);

    // main clock
    int clock = 0;
    int new_come = MAXTIME;
    int end = MAXTIME;
    int update = MAXTIME;
    int start = 0;
    Process cur_process, tmp;

    // get initial new come time
    new_come = next_arrive(que3);

    while (1) {
        // get update
        update = get_next_update(que2);
        // get next event
        int next_arr = get_next_arr(new_come, end, update);

        // execute next event
        if(next_arr == MAXTIME)
            break;

        // display_job(cur_process);
        // new come
        if (new_come == next_arr) {
            int next_arrival;
            next_arrival = que3.top().arrival;
            // push jobs arriving at the same time into que1 and que2
            while (!que3.empty() && que3.top().arrival == next_arrival) {
                tmp = que3.top();
                if (tmp.priority <= PRIORITY) {
                    tmp.cur_queue = QUEUE1;
                    que1.push(tmp);
                } else {
                    tmp.cur_queue = QUEUE2;
                    que2.push(tmp);
                }
                que3.pop();
            }
            new_come = next_arrive(que3);

            // no running process
            if (end == MAXTIME) {
                clock = next_arrival;
                end = schedule_a_job(que1, que2, start, cur_process, clock);
            }

            // running process from queue 2
            else if (cur_process.cur_queue == QUEUE2) {
                if (!que1.empty() ||
                    (!que2.empty() && cur_process < que2.top())) {
                    end = next_arrival;
                }
            }
        }

        // end
        while (end == next_arr) {

            // update running process info
            cur_process.running += end - start;
            cur_process.last_end = end;
            cur_process.age = end;
            cur_process.queue_arrival = end;

            // finish
            if (cur_process.running == cur_process.total) {
                cur_process.end = end;
                finished.push_back(cur_process);
            } else {
                while (cur_process.cur_queue == QUEUE1) {
                    cur_process.running_time += 1;
                    while (cur_process.running_time >= 2 &&
                        cur_process.cur_queue == QUEUE1) {
                        cur_process.running_time -= 2;
                        cur_process.priority += 1;
                    }
                }

                // downgrade to queue 2
                if (cur_process.priority > PRIORITY) {
                    cur_process.from_queue = cur_process.cur_queue;
                    cur_process.cur_queue = QUEUE2;
                    que2.push(cur_process);
                } 
                else {
                    cur_process.from_queue = QUEUE1;
                    cur_process.cur_queue = QUEUE1;
                    que1.push(cur_process);
                }
            }

            clock = end;
            // schedule a new process to cpu
            end = schedule_a_job(que1, que2, start, cur_process, clock);
        }

        // update
        while (update == next_arr) {
            clock = update;
            update_job(que1, que2, update);
            while (cur_process.cur_queue == QUEUE2) {
                while(!que1.empty()) {
                    end = update;
                }
            }
        }
    }

    return finished;
}

int main(int argc, char **argv) {
    if (argc < 2 or argc > 3) {
        cout << "Invalid parameters\n";
        return 0;
    }

    char *filename = argv[1];
    vector<Process> finished;
    finished = simulate(filename);
    
    cout << "name   arrival   end   ready   running   waiting\n";
    for (int i = 0; i < finished.size(); i++) {
        Process process;
        process = finished[i];
        cout << process.name << "\t\t" << process.arrival << "\t\t" << process.end << "\t\t"
             << process.ready << "\t\t" << process.running << "\t\t" << process.waitting
             << "\n";
    }
    return 0;
}