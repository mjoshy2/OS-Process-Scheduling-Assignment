#include <iostream>
#include <queue>
#include "system.h"
using namespace std;

int clockTime = 0;
double sumSSD = 0;
double totalSSD = 0;
double coreAccess = 0;

//default constructor that sets up number of cores and processes in the System and begins the first processes.
System::System(int x, int y, vector<process> list) {
	numProcesses = x;
	numCores = y;

	//setting up the vector coreList based on how many cores there are
	for (int i = 0; i < numCores; i++) {
		core temp(i);
		coreList.push_back(temp);
	}

	processList = list;
	//setting up the vector processList
	//for (unsigned int i = 0; i < x; i++) {
	//	processList.push_back(list[i]);
	//}
	findEvent();
}

//returns the number of the first core that is available or -1 if there are no cores available.
int System::freeCores() {
	for (unsigned int i = 0; i < coreList.size(); i++) {
		if (coreList[i].isFree())
			return i;
	}
	return -1;
}

//finds the next Event in the System using the processTable
//is essentially the process table
//determines what event occurs next
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

	for (int i = 0; i < numCores; i++) {
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
		clockTime = quickest;
		coreCompletionRoutine(coreNUM);
	}
	else if (event == "ssd") {
		clockTime = quickest;
		ssdCompletionRoutine();
	}
	else if (event == "input") {
		clockTime = quickest;
		inputCompletionRoutine();
	}

	else if (event == "arrival") {
		clockTime = quickest;
		processArrivalRoutine(processList[processesStarted + 1]);
	}
	else {
		simulationCompletionRoutine();
	}
}

//when a process finished a request this function should be invoked so it correctly puts the process in the appropriate position in the System
void System::nextRequestRoutine(process &temp) {
	if (temp.empty()) {
		processCompletionRoutine(temp.getProcessNum());
		findEvent();
		return;
	}
	request next = temp.getRequest();
	string requestType = next.getRequestType();
	if (requestType == "CORE")
		coreRequestRoutine(temp);
	else if (requestType == "SSD")
		ssdRequestRoutine(temp);
	else if (requestType == "INPUT")
		inputRequestRoutine(temp);
}

//function for requesting a core 
void System::coreRequestRoutine(process &temp) {
	int coreNum = freeCores();
	//vector<string> request = divide(temp.getRequest());
	request next = temp.getRequest();
	int requestTime = next.getRequestTime();

	if (coreNum != -1) {
		coreList[coreNum].setCoreBusy(requestTime + clockTime, temp.getProcessNum());
		temp.setState("RUNNING");
	}
	else {
		readyQueue.push(temp);
		temp.setState("READY");
	}
	findEvent();
}

//function for requesting an SSD
void System::ssdRequestRoutine(process &temp) {
	request next = temp.getRequest();
	int requestTime = next.getRequestTime();

	if (ssd.isFree()) {
		ssd.setSSDbusy(requestTime + clockTime, temp.getProcessNum());
	}
	else {
		ssdQueue.push(temp);
	}
	temp.setState("BLOCKED");
	findEvent();
}

//function for requesting an input device
void System::inputRequestRoutine(process &temp) {
	request next = temp.getRequest();
	int requestTime = next.getRequestTime();

	if (inputD.isFree()) {
		inputD.setInputBusy(requestTime + clockTime, temp.getProcessNum());
	}
	else {
		inputQueue.push(temp);
	}
	temp.setState("BLOCKED");
	findEvent();
}

//function for completing core request
void System::coreCompletionRoutine(int coreNum) {
	int processFinished = coreList[coreNum].getProcessID();

	request del = processList[processFinished].getRequest();
	coreAccess += del.getRequestTime();

	processList[processFinished].deleteRequest();

	if (readyQueue.empty()) {
		coreList[coreNum].setCoreFree();
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = readyQueue.front();
		readyQueue.pop();
		request req = temp.getRequest();
		coreList[coreNum].setCoreBusy(req.getRequestTime() + clockTime, temp.getProcessNum());
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for completing ssd completion
void System::ssdCompletionRoutine() {
	int processFinished = ssd.getProcessID();

	request del = processList[processFinished].getRequest();
	sumSSD += del.getRequestTime();
	totalSSD++;

	processList[processFinished].deleteRequest();

	if (ssdQueue.empty()) {
		ssd.setSSDfree();
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = ssdQueue.front();
		ssdQueue.pop();
		request req = temp.getRequest();
		ssd.setSSDbusy(req.getRequestTime() + clockTime, temp.getProcessNum());
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for input device completion
void System::inputCompletionRoutine() {
	int processFinished = inputD.getProcessID();

	request del = processList[processFinished].getRequest();

	processList[processFinished].deleteRequest();

	if (inputQueue.empty()) {
		inputD.setInputFree();
		nextRequestRoutine(processList[processFinished]);
	}
	else {
		process temp = inputQueue.front();
		inputQueue.pop();
		request req = temp.getRequest();
		inputD.setInputBusy(req.getRequestTime() + clockTime, temp.getProcessNum());
		nextRequestRoutine(processList[processFinished]);
	}
}

//function for when a process first arrives where it sends the first core request to its appropriate position in the System
void System::processArrivalRoutine(process & temp) {
	clockTime = processList[temp.getProcessNum()].getArrivalTime();
	cout << endl;
	printProcesses(processesStarted);
	processesStarted++;
	coreRequestRoutine(temp);
}

//prints out the status of all other processess when a processes is completed
void System::processCompletionRoutine(int x) {
	cout << endl;
	cout << "Process " << x << " terminated at time " << clockTime << " ms" << endl;
	processList[x].setState("TERMINATED");
	for (int i = 0; i < numProcesses; i++) {
		if (processList[i].getState() != "N/A") {
			if (processList[i].getState() != "TERMINATED" || i == x) {
				cout << "Process " << i << " is " << processList[i].getState() << endl;
			}
		}
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
	cout << "Total elapsed time: " << clockTime << " ms" << endl;
	cout << "Core Utilization: ";
	printf("%.2f", (coreAccess / clockTime) * 100);
	cout << " percent" << endl;
	cout << "SSD Utilization: ";
	printf("%.2f", (sumSSD / clockTime) * 100);
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
