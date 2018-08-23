#include <iostream>
#include <queue>
#include <sstream>
#include "System.h"
using namespace std;

int clock = 0;
double sumSSD = 0;
double totalSSD = 0;
double coreAccess = 0;

//reads a string and places the seperate words in a vector and returns the vector
vector<string> divide(string x) {
	vector<string> temp;
	string buffer;
	stringstream ss(x);

	while (ss >> buffer)
		temp.push_back(buffer);
	return temp;
}

//default constructor that sets up number of cores and processes in the system and begins the first processes.
System::System(int x ,int y,vector<process> list) {
	numCores = y;
	numProcesses = x;

	//setting up the vector coreList based on how many cores there are
	for (int i = 0; i < numCores; i++) {
		core temp(i);
		coreList.push_back(temp);
	}

	//setting up the vector processList
	for (unsigned int i = 0; i < x; i++) {
		processList.push_back(list[i]);
	}
	findEvent();
}

//prints the status of the cores
void System::printStatusOfCores() {
	for (int i = 0; i < numCores; i++) {
		if (coreList[i].isFree())
			cout << "core " << i<< " is free" << endl;
		else
			cout << "core " << i << " is busy " << coreList[i].getTimer() << endl;
	}
}

//returns the number of the first core that is available or -1 if there are no cores available.
int System::freeCores() {
	for (unsigned int i = 0; i < coreList.size(); i++) {
		if (coreList[i].isFree())
			return i;
	}
	return -1;
}

//finds the next Event in the System
void System::findEvent() {
	int quickest = 99999999;
	int coreNUM;
	string event = "empty";

	if (processesStarted + 1 < numProcesses) {
		if (processList[processesStarted + 1].getArrivalTime() < quickest) {
			quickest = processList[processesStarted + 1].getArrivalTime();
			event = "arrival";
		}
	}
	
	for (unsigned int i = 0; i < numCores; i++) {
		if (coreList[i].isFree() == false) {
			if (coreList[i].getTimer() < quickest) {
				quickest = coreList[i].getTimer();
				event = "core";
				coreNUM = i;
			}
		}
	}

	if (!ssd.isFree()) {
		if (ssd.getTimer() < quickest) {
			quickest = ssd.getTimer();
			event = "ssd";
		}
	}

	if (!inputD.isFree()) {
		if (inputD.getTimer() < quickest) {
			quickest = inputD.getTimer();
			event = "input";
		}
	}

	if (event == "core") {
		clock = quickest;
		coreCompletionRoutine(coreNUM);
	}
	else if (event == "ssd") {
		clock = quickest;
		ssdCompletionRoutine();
	}
	else if (event == "input") {
		clock = quickest;
		inputCompletionRoutine();
	}

	else if (event == "arrival") {
		clock = quickest;
		processArrivalRoutine(processList[processesStarted + 1]);
	}
	else {
		simulationCompletionRoutine();
	}
}

//when a process finished a request this function should be invoked so it correctly puts the process in the appropriate position in the system
void System::nextRequestRoutine(process &temp) {
	vector<string> request = divide(temp.getRequest());
	if (request[0] == "CORE")
		coreRequestRoutine(temp);
	else if (request[0] == "SSD")
		ssdRequestRoutine(temp);
	else if (request[0] == "INPUT")
		inputRequestRoutine(temp);
	else {
		proccessCompletionRoutine(temp.getProcessNum());
		findEvent();
	}
}

//function for requesting a core 
void System::coreRequestRoutine(process &temp) {
	int coreNum = freeCores();
	vector<string> request = divide(temp.getRequest());
	cout << "-- Process " << temp.getProcessNum() << " requests a core at time " << clock  << " ms for " << request[1] << " ms" << endl;
	if (coreNum != -1) {
		coreList[coreNum].setCoreBusy(stoi(request[1]) + clock, temp.getProcessNum());
		cout << "-- Process " << temp.getProcessNum() << " will release a core at time  " << stoi(request[1]) + clock << endl;
		temp.setState("RUNNING");
	}
	else {
		cout << "-- Process " << temp.getProcessNum() << " must wait for a core" << endl;
		readyQueue.push(temp);
		temp.setState("READY");
		cout << "-- Ready queue now contains " << readyQueue.size() << " process(es) waiting for a core" << endl;
	}
	findEvent();
}

//function for requesting an SSD
void System::ssdRequestRoutine(process &temp) {
	vector<string> request = divide(temp.getRequest());
	cout << "-- Process " << temp.getProcessNum() << " requests the ssd for: " << request[1] << " ms at " << clock << endl;
	if (ssd.isFree()) {
		ssd.setSSDbusy(stoi(request[1]) + clock, temp.getProcessNum());
		cout << "-- Process " << temp.getProcessNum() << " has the SSD for " << request[1] << endl;
	}
	else {
		cout << "-- Process " << temp.getProcessNum() << " has been added to SSD queue for " << request[1] << endl;
		ssdQueue.push(temp);
	}
	temp.setState("BLOCKED");
	findEvent();
}

//function for requesting an input device
void System::inputRequestRoutine(process &temp) {
	vector<string> request = divide(temp.getRequest());
	cout << "-- Process " << temp.getProcessNum() << " requests the input device for: " << request[1] << " ms at " << clock << endl;
	if (inputD.isFree()) {
		inputD.setInputBusy(stoi(request[1]) + clock, temp.getProcessNum());
		cout << "-- Process " << temp.getProcessNum() << " starts input at " << clock << endl;
	}
	else {
		inputQueue.push(temp);
		cout << "-- Process " << temp.getProcessNum() << " has been added to input queue for " << request[1] << endl;
	}
	temp.setState("BLOCKED");
	findEvent();
}

//function for completing core request
void System::coreCompletionRoutine(int coreNum) {
	int processFinished = coreList[coreNum].getProcessID();

	vector<string> del = divide(processList[processFinished].getRequest());
	coreAccess += stoi(del[1]);

	processList[processFinished].deleteRequest();

	cout << "-- CORE completion event for process " << processFinished << " at time " << clock << " ms" << endl;
	if (readyQueue.empty()) {
		coreList[coreNum].setCoreFree();
		cout << "-- Core " << coreNum << " is now free" << endl;
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = readyQueue.front();
		readyQueue.pop();
		vector<string> request = divide(temp.getRequest());
		coreList[coreNum].setCoreBusy(stoi(request[1]) + clock,temp.getProcessNum());
		cout << "-- Process "<< temp.getProcessNum() << " will release a core at time " << stoi(request[1]) + clock << " ms " << endl;
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for completing ssd completion
void System::ssdCompletionRoutine() {
	int processFinished = ssd.getProcessID();

	vector<string> del = divide(processList[processFinished].getRequest());
	sumSSD += stoi(del[1]);
	totalSSD++;

	processList[processFinished].deleteRequest();
	cout << "-- SSD completion event for process " << processFinished << " at time " << clock << " ms" << endl;

	if (ssdQueue.empty()) {
		cout << "-- SSD is now free" << endl;
		ssd.setSSDfree();
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = ssdQueue.front();
		ssdQueue.pop();
		vector<string> request = divide(temp.getRequest());
		ssd.setSSDbusy(stoi(request[1]) + clock,temp.getProcessNum());
		cout << "-- Process " << temp.getProcessNum() << " will release SSD at time " << stoi(request[1]) + clock << " ms " << endl;
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for input device completion
void System::inputCompletionRoutine() {
	int processFinished = inputD.getProcessID();
	processList[processFinished].deleteRequest();
	cout << "-- CORE completion event for process " << processFinished << " at time " << clock << " ms" << endl;

	if (inputQueue.empty()) {
		inputD.setInputFree();
		cout << "-- Input device is now free " << endl;
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = inputQueue.front();
		inputQueue.pop();
		vector<string> request = divide(temp.getRequest());
		inputD.setInputBusy(stoi(request[1]) + clock, temp.getProcessNum());
		cout << "-- Process " << temp.getProcessNum() << " will release input device at time " << stoi(request[1]) + clock << " ms " << endl;
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for when a process first arrives where it sends the first core request to its appropriate position in the system
void System::processArrivalRoutine(process & temp) {
	cout << "-- Arrival event for process "<< temp.getProcessNum() <<" at time " << processList[temp.getProcessNum()].getArrivalTime() << endl;
	clock = processList[temp.getProcessNum()].getArrivalTime();
	cout << endl;
	printProcesses(processesStarted);
	processesStarted++;
	coreRequestRoutine(temp);
}

//prints out the status of all other processess when a processes is completed
void System::proccessCompletionRoutine(int x) {
	cout << endl;
	cout << "Process " << x << " terminated at time " << clock << " ms" << endl;
	processList[x].setState("Terminated");
	for (int i = 0; i < processesStarted + 1; i++) {
		cout << "Process " << i << " is " << processList[i].getState() << endl;
	}
	cout << endl;
}

//prints out the summary
void System::simulationCompletionRoutine() {
	cout << "SUMMARY " << endl;
	cout << "Number of processes that completed: " << processesStarted + 1 << endl;
	cout << "Total number of SSD accesses: " << totalSSD << endl;
	cout << "Average SSD access time: ";
	printf("%.2f", (sumSSD / totalSSD));
	cout << endl;
	cout << "Total elapsed time: " << clock << " ms" << endl;
	cout << "Core Utilization: ";
	printf("%.2f", (coreAccess / clock) * 100);
	cout << " percent" << endl;
	cout << "SSD Utilization: "; 
	printf("%.2f", (sumSSD / clock) * 100);
	cout << " percent" << endl;
}

//when a process first arrives it prints out the status of all the previous processess
void System::printProcesses(int x) {
	cout << "Process " << x + 1 << " starts at a time " << processList[x + 1].getArrivalTime() << " ms" << endl;
	if (x != -1) {
		for (int i = 0; i <= x; i++) {
			cout << "Process " << i << " is " << processList[i].getState() << endl;
		}
	}
	cout << endl;
}