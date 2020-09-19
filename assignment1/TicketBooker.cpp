#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <iomanip>
#include<algorithm>
using namespace std;
class Person {
public:
	Person() {
		running = 0;
		ready = -1;
	}
	string PID;
	int ARRTIME;
	int priority;
	int age;
	int tickets;
	int end;
	int running;
	int ready;
};
class myQueue {
public:
	myQueue() {
		times = 0;
		now_choose = -1;
		debug_flag = 0;
	}
	bool comp(Person& a, Person& b)
	{
		if (a.ARRTIME < b.ARRTIME)
			return true;
		else if(a.ARRTIME > b.ARRTIME)
			return false;
		if (a.priority < b.priority)
			return true;
		else if (a.priority > b.priority)
			return false;
		if(a.PID < b.PID)
			return true;
		return false;
	}
	
  bool comp2(Person a, Person b) {
		if (a.tickets > b.tickets)
			return true;
		else if (a.tickets < b.tickets)
			return false;
		if (a.priority > b.priority)
			return true;
		else if (a.priority < b.priority)
			return false;
		if (a.ARRTIME > b.ARRTIME)
			return true;
		else if (a.ARRTIME < b.ARRTIME)
			return false;
		if(a.PID > b.PID)
			return true;
		return false;
	}
	void readfile() {
		while (true) {
			Person p;
			cin >> p.PID >> p.ARRTIME >> p.priority >> p.age >> p.tickets;
			if (cin.eof())
				break;
			read_queue.push_back(p);
		}		
    int i;
    int j;
		while (i < read_queue.size() - 1) {
			while(j < read_queue.size() - 1 - i) {
				if (!comp(read_queue[j], read_queue[j + 1])) {
					Person temp = read_queue[j];
					read_queue[j] = read_queue[j + 1];
					read_queue[j + 1] = temp;
				}
        j++;
			}
      i++;
		}
	}
	void runQ1()
	{
		Person cur_cus;
		if (now_choose == -1) {
			now_choose = 0;
			cur_cus = Q1[now_choose];
			cur_cus.age++;
			QUATAMAX = 2 * (10 - cur_cus.priority);
			if (QUATAMAX >= cur_cus.tickets)
				QUATAMAX = cur_cus.tickets;
		}
		else
			cur_cus = Q1[now_choose];
		cur_cus.tickets--;
		QUATAMAX--;
		cur_cus.running += 5;
		while (cur_cus.ready == -1)
			cur_cus.ready = times;
		if (QUATAMAX == 0) {
			if (cur_cus.tickets == 0) {
				cur_cus.end = times + 5;
				debug_flag = 1;
				final.push_back(cur_cus);
			}
			else {
				if (cur_cus.age == 2) {
					cur_cus.age = 0;
					cur_cus.priority++;
					if (cur_cus.priority == 4) {
						move_1to2.push_back(cur_cus);
					}
					else
						move_1toend.push_back(cur_cus);
				}
				else {
					move_1toend.push_back(cur_cus);
				}
			}
			Q1.erase(Q1.begin() + now_choose);
			now_choose = -1;
		}
		else
			Q1[now_choose] = cur_cus;
      int i;
		while (i < Q2.size()) {
			Q2[i].age += 5;
			if (Q2[i].age == 100) {
				Q2[i].age = 0;
				Q2[i].priority--;
				if (Q2[i].priority == 3) {
					move_2to1.push_back(Q2[i]);
					Q2.erase(Q2.begin() + i);
					i--;
				}
			}i++;
		}
	}


	void runQ2()
	{
		int min = 0;
    int i;
		while (i < Q2.size()) {
			if (min == i) {
				if (Q2[i].ready == -1) {
					Q2[i].ready = times;
				}
				Q2[i].running += 5;
				Q2[i].tickets -= 1;
				if (Q2[i].tickets == 0) {
					Q2[i].end = times+5;
					debug_flag = 1;
					final.push_back(Q2[i]);
					Q2.erase(Q2.begin() + i);
					min--;
					i--;
				}
			}
			else {
				Q2[i].age += 5;
				if (Q2[i].age == 100) {
					Q2[i].age = 0;
					Q2[i].priority -= 1;
					if (Q2[i].priority == 3) {
						move_2to1.push_back(Q2[i]);
						Q2.erase(Q2.begin() + i);
						if (min > i)
							min--;
						i--;
					}
				}
			}
      i++;
		}
	}
	void mydebug()
	{
    int i;
		ofstream out("input.txt", ios::app);
		out << "Time:" <<times<< endl
			<< "name ARRTIME tickets_required running priority_number age / runs" << endl;
		out << "Q1" << endl;
		while (i < Q1.size()) {
			out << Q1[i].PID
				<< "\t" << Q1[i].ARRTIME
				<< "\t" << Q1[i].tickets
				<< "\t" << Q1[i].running
				<< "\t" << Q1[i].priority
				<< "\t" << Q1[i].age << endl;
        i++;
		}
		out << "Q2" << endl;
    int j;
		while (j < Q2.size()) {
			out << Q2[j].PID
				<< "\t" << Q2[j].ARRTIME
				<< "\t" << Q2[j].tickets
				<< "\t" << Q2[j].running
				<< "\t" << Q2[j].priority
				<< "\t" << Q2[j].age << endl;
        j++;
		}
		out << endl << endl;;
		out.close();
	}
	void swift()
	{
		for (int i = 0; i < read_queue.size(); i++) {
			if (read_queue[i].ARRTIME== times) {
				debug_flag = 1;
				if (read_queue[i].priority <= 3)
					Q1.push_back(read_queue[i]);
				else {
					int k;
					for (k = 0; i < Q2.size(); k++) {
						if (comp2(Q2[k], read_queue[i])) {
							Q2.insert(Q2.begin() + k, read_queue[i]);
							k = -1;
							break;
						}
					}
					if(k!=-1)
						Q2.push_back(read_queue[i]);
				}				
				read_queue.erase(read_queue.begin() + i);
				i--;
			}
		}
		for (int i = 0; i < move_1toend.size(); i++) {
			debug_flag = 1;
			Q1.push_back(move_1toend[i]);
		}
		move_1toend.clear();
		for (int i = 0; i < move_2to1.size(); i++) {
			debug_flag = 1;
			Q1.push_back(move_2to1[i]);
		}
		move_2to1.clear();
		for (int i = 0; i < move_1to2.size(); i++) {
			debug_flag = 1;
			int k;
			while (i < Q2.size()) {
				if (comp2(Q2[k], read_queue[i])) {
					Q2.insert(Q2.begin() + k, move_1to2[i]);
					k = -1;
          k ++;
					break;
				}
			}
			if (k != -1)
				Q2.push_back(read_queue[i]);
		}
		move_1to2.clear();
	}
	void run() 
	{
		while (true) {
			swift();
			if (Q1.empty() && Q2.empty() && read_queue.empty())
				break;
			debug_flag = 0;
			if (!Q1.empty()) {
				runQ1();
			}
			else if(!Q2.empty())
				runQ2();
			times += 5;
		}
	}
	void outfile()
	{
		cout << "name   arrival   end   ready   running   waiting" << endl;
		for (int i = 0; i < final.size(); i++) {
			cout << final[i].PID
				<<"\t" << final[i].ARRTIME
				<< "\t" << final[i].end
				<< "\t" << final[i].ready
				<< "\t" << final[i].running
				<< "\t" << final[i].end-final[i].ready-final[i].running << endl;
		}
	}
private:
	vector<Person> read_queue;
	vector<Person> Q1;
	vector<Person> Q2;
	vector<Person> final;
	vector<Person> move_1toend;
	vector<Person> move_2to1;
	vector<Person> move_1to2;
	int debug_flag;
	int now_choose;
	int QUATAMAX;
	int times;
};
int main(int argc, char* argv[])
{
	freopen(argv[1], "r", stdin);
	myQueue my;
	my.readfile();
	my.run();
	my.outfile();
}