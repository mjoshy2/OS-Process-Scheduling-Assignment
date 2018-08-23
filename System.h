#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include "process.h"
using namespace std;

//represents the input device in the system
class inputDevice {
private:
	int timer; //variable to record how long the input device will be busy
	int processNumber; //variable to record which processes is currently holding the input device
	string state = "free"; //variable to record the state of the input device
public:

	inputDevice() {
		timer = 0;
	}

	//sets the state of the input to free and processNumber that the device holds to -1 which represents no processess.
	void setInputFree() {
		processNumber = -1;
		state = "free";
	}

	//sets the timer to x and the processNumber to pN which is the process currently in the input device, and changes state to busy
	void setInputBusy(int x, int pN) {
		timer = x;
		processNumber = pN;
		state = "busy";
	}

	//returns number of processess currently executing in the input device
	int getProcessID() {
		return processNumber;
	}

	//returns true if there is a processes in the device and false if not
	bool isFree() {
		if (state == "free")
			return true;
		return false;
	}

	//returns the time when the device will finally be free
	int getTimer() {
		return timer;
	}
};

//class to represent the SSD in the System
class SSD {
private:
	int timer;//variable to record how long the ssd will be busy
	int processNumber;//variable to record the number of process currently holding the ssd
	string state = "free"; //variable to record state of SSD
public:
	SSD() {
		timer = 0;
	}

	//returns process number of device holding the ssd
	int getProcessID() {
		return processNumber;
	}

	//sets the state of ssd to free and the processNumber to -1 to denote no process in SSD
	void setSSDfree() {
		processNumber = -1;
		state = "free";
	}

	//sets the state of ssd to busy and the timer to x and processNumber to pN
	void setSSDbusy(int x, int pN) {
		timer = x;
		processNumber = pN;
		state = "busy";
	}

	//returns false if a process is holding the ssd and true otherwise
	bool isFree() {
		if (state == "free")
			return true;
		return false;
	}

	//returns timer
	int getTimer() {
		return timer;
	}
};

//class to represnt the core in the System
class core {
private:
	int coreID; //represents the number of the core within the list of cores
	int processNumber;//represents the process number of the process holding the core
	int timer; //represnts the time the core will be busy for
	string state = "free"; //represents the state of the core
public:
	core(int x) {
		coreID = x;
		timer = 0;
	}

	//sets the core number to x and the processNumber to pN
	void setCoreNum(int x, int pN) {
		coreID = x;
		processNumber = pN;
	}

	//sets the state to busy and the timer to x and processNumber to pN
	void setCoreBusy(int x, int pN) {
		processNumber = pN;
		timer = x;
		state = "busy";
	}

	//sets core to free and the process number to -1 to denote an empty core
	void setCoreFree() {
		processNumber = -1;
		state = "free";
	}

	//returns the process number in the core
	int getProcessID() {
		return processNumber;
	}

	//returns true if state of core is free and false otherwise
	bool isFree() {
		if (state == "free")
			return true;
		return false;
	}

	//returns timer
	int getTimer() {
		return timer;
	}
};

//represents the System itself
class System {
private:
	//numCores is the number of Cores in the System
	//numProcesses is the total number of processes in the System
	//processesStarted is the number of processess that have entered the system
	int numCores, numProcesses, processesStarted = -1;
	vector<core> coreList; //coreList is a vector of core objects and represent individual cores in the system
	vector<process> processList; //processList is a vector of processes
	SSD ssd; //object of ssd class
	inputDevice inputD; //object of input device
	queue <process> readyQueue; //the ready queue is for processess that are waiting for a core
	queue <process> ssdQueue; // the ssd queue is for processess waiting for the SSD to be available
	queue <process> inputQueue; //inputQueue is for processess waiting for the input device to be free

public:
	System(int,int,vector<process>);
	void findEvent();
	int freeCores();

	void nextRequestRoutine(process &);
	
	void coreRequestRoutine(process&);
	void ssdRequestRoutine(process&);
	void inputRequestRoutine(process&);
	void processArrivalRoutine(process&);

	void coreCompletionRoutine(int);
	void ssdCompletionRoutine();
	void inputCompletionRoutine();

	void proccessCompletionRoutine(int);
	void simulationCompletionRoutine();

	void printStatusOfCores();
	void printProcesses(int);
};