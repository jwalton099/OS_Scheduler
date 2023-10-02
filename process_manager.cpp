#include <iostream>
#include <iomanip>   // For output requirements
#include <unistd.h>  // For fork, pipe, exec, etc.
#include <cstdlib>   // For exit
#include <sys/wait.h> // For wait
#include "queue_array.h"
#include <queue>
#include <math.h>

using namespace std;

// Define PCB structure
struct PCB {
    int pid;
    int priority;
    int value;
    int start_time;
    int run_time;
    int cpu_time;
    int quantum;
};

// Define major variables
int Time = 0;
int RunningState = 0;
QueueArray <int> ReadyState(4);
QueueArray <int> BlockedState1(4);
QueueArray <int> BlockedState2(4);
QueueArray <int> BlockedState3(4);
PCB pcbArray[100];

//Print format for any given pid
void printPID(int pid) {
    cout << std::setw(2)  << pcbArray[pid].pid << "  "
     << std::setw(4)  << pcbArray[pid].priority << "  "
     << std::setw(7)  << pcbArray[pid].value << "  "
     << std::setw(11) << pcbArray[pid].start_time << "  "
     << std::setw(8)  << pcbArray[pid].cpu_time << endl;
}

//Prints formatted output for all pids in a queue
void printPIDs(queue<int> pids) {
    while(!pids.empty()){
	printPID(pids.front());
	pids.pop();
    }
}

//Gets a queue of all PIDs in a given QueueArray
queue<int> getPIDs(QueueArray<int> qa) {
    queue<int> pids;
    for(int i=0;i<qa.Asize(); i++){
	if(qa.Qsize(i)>0){
	    int* ptr = qa.Qstate(i);
            for(int j=0; j<qa.Qsize(i); j++){
		pids.push(ptr[j]);
	    }
	}
    }
    return pids;
}

//Gets a queue of all PIDS for a given QueueArray for a given index
queue<int> getPID(QueueArray<int> q, int index) {
    queue<int> pids;
    int* ptr = q.Qstate(index);
    for(int i=0; i<q.Qsize(index); i++){
	pids.push(ptr[i]);
    }
    return pids;
}

//Prints the header before a section of output
void printHeader() {
    cout <<":"<<endl<<"PID  Priority Value  Start Time  Total CPU time" << endl;
}

//Function called by Q and C to increment time unit by 1
void incrementTime() {
    Time++;
    pcbArray[RunningState].quantum-=1;
    pcbArray[RunningState].cpu_time+=1;
    //When time increments, the active process' quantum decreases and cpu time increases
    if(pcbArray[RunningState].cpu_time >= pcbArray[RunningState].run_time)
    {
	//If it has been running for the total time it needs to complete, start next process
	RunningState=ReadyState.Dequeue();
	return;
    }
    if(pcbArray[RunningState].quantum<=0)
    {
	//If a process has used all its quantum, decrease priority, requeue it, and start next process
	if(pcbArray[RunningState].priority<3)
	    pcbArray[RunningState].priority+=1;
        pcbArray[RunningState].quantum=pow(2,pcbArray[RunningState].priority);
	ReadyState.Enqueue(RunningState,pcbArray[RunningState].priority);
        RunningState=ReadyState.Dequeue();
    }
}

void reporterProcess() {
    cout<<"*****************************************************"<<endl;
    cout<<"The current system state is as follows:"<<endl;
    cout<<"*****************************************************"<<endl<<endl;

    cout<<"CURRENT TIME: "<<Time<<endl<<endl;

    cout<<"RUNNING PROCESS";
    printHeader();
    printPID(RunningState);

    cout<<endl<<"BLOCKED PROCESS: "<<endl;
    cout<<"Queue of processes Blocked for resource 0 ";
    if(BlockedState1.QAsize()==0){
	cout<<"is empty"<<endl;
    }
    else {
	printHeader();
	printPIDs(getPIDs(BlockedState1));
    }
    cout<<"Queue of processes Blocked for resource 1 ";
    if(BlockedState2.QAsize()==0){
        cout<<"is empty"<<endl;
    }
    else {
        printHeader();
        printPIDs(getPIDs(BlockedState2));
    }
    cout<<"Queue of processes Blocked for resource 2 ";
    if(BlockedState3.QAsize()==0){
        cout<<"is empty"<<endl;
    }
    else {
        printHeader();
        printPIDs(getPIDs(BlockedState3));
    }

    cout<<endl<<"PROCESSES READY TO EXECUTE: "<<endl;
    for(int i = 0; i<4; i++){
        cout<<"Queue of processes with priority "<<i<<" ";
        if(ReadyState.Qsize(i)==0){
            cout<<"is empty"<<endl;
        }
        else {
	    printHeader();
	    printPIDs(getPID(ReadyState,i));
        }
    }

    cout<<"*****************************************************"<<endl<<endl;
}

int main() {

    string command, input;
    char buffer[1024];  // Buffer for reading from the pipe

    // Read commands from the pipe (stdin)
    ssize_t bytesRead;
    while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        input = buffer;
	command = input.substr(0,1);

	int space1,space2,space3;
	string command = input.substr(0,1);
        if(command=="S") {
	    //Code section for Starting a new process
	    space1 = input.find(' ');
	    space2 = input.find(' ', space1+1);
	    space3 = input.find(' ', space2+1);
	    int pid = stoi(input.substr(space1+1,space2-space1-1));
	    int value = stoi(input.substr(space2+1,space3-space2-1));
	    int run_time = stoi(input.substr(space3+1,input.length()-space3-1));

	    int slot = pid;
            pcbArray[slot].pid = pid;
            pcbArray[slot].priority = 0;
            pcbArray[slot].value = value;
            pcbArray[slot].start_time = Time;
            pcbArray[slot].run_time = run_time;
            pcbArray[slot].cpu_time = 0;
	    pcbArray[slot].quantum = 1;

	    if(RunningState==0){
		RunningState=pid;
	    }
            else {
		ReadyState.Enqueue(pcbArray[slot].pid,0);
	    }
       	}
        else if(command=="B"){
	    //Code section for Blocking the running process		
	    if(pcbArray[RunningState].priority>0){
		pcbArray[RunningState].priority-=1;
	    }
	    pcbArray[RunningState].quantum=pow(2,pcbArray[RunningState].priority);
            space1 = input.find(' ');
	    int rid = stoi(input.substr(space1+1,input.length()-space1-1));
	    if(rid==0){
		BlockedState1.Enqueue(RunningState,pcbArray[RunningState].priority);
	    }
	    else if(rid==1){
		BlockedState2.Enqueue(RunningState,pcbArray[RunningState].priority);
	    }
	    else {
		BlockedState3.Enqueue(RunningState,pcbArray[RunningState].priority);
	    }
	    RunningState = ReadyState.Dequeue();
        }
        else if(command=="U") {
	    //Code section for unblocking the first process in a given QueueArray
            space1 = input.find(' ');
            int rid = stoi(input.substr(space1+1,input.length()-space1-1));
            int pid=0;
	    if(rid==0){
                pid = BlockedState1.Dequeue();
            }
            else if(rid==1){
                pid = BlockedState2.Dequeue();
            }
            else {
                pid = BlockedState3.Dequeue();
            }
	    ReadyState.Enqueue(pid,pcbArray[pid].priority);
        }
        else if(command=="Q") {
	    //Code section for incrementing time
	    incrementTime();
        }
        else if(command=="C") {
	    //Code section for changing the value of the running process, increments time
            space1 = input.find(' ');
	    space2 = input.find(' ', space1+1);
	    string cmd = input.substr(space1+1,space2-space1-1);
	    int num = stoi(input.substr(space2+1,input.length()-space2-1));

	    if(cmd=="A") {
		pcbArray[RunningState].value += num;
	    }
	    else if (cmd=="S") {
                pcbArray[RunningState].value -= num;
	    }
            else if (cmd=="M") {
                pcbArray[RunningState].value *= num;
            }
            else if (cmd=="D") {
                pcbArray[RunningState].value /= num;
            }
            else {
                cerr<<"Not valid cmd"<<endl;
            }
	    incrementTime();
        }
        else if(command=="P") {
	    pid_t childPid = fork();

            if (childPid == 0) {
                // Child process (reporter)
                reporterProcess();
                exit(0); // Terminate the reporter process
            } else if (childPid < 0) {
                cerr<<"Failed to fork reporter process"<<endl;
            }
        }
        else if(command=="T") {
	    int count = 0;
	    double accum = 0.0;
	    //code to terminate on "T"
	    for(int i=0; i<100; i++){
		if(pcbArray[i].pid!=0){
		    count++;
		    accum+=(Time-pcbArray[i].start_time);
		}
	    }
	    cout<<"The average Turnaround Time: "<<accum/count<<endl;
            exit(0);
	}
	else {
	    return 1;
	}
    }
    return 0;
}
