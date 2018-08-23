#include "process.h"

//need a default constructor to be empty since the processess are initialized by default constructors with empty processess
process::process() {
}

//returns the next request to be processesed or returns no requests, if none left
string process::getRequest() {
	if (requests.size() != 0)
		return requests[0];
	else return "no requests";
}

process::process(string x) {
	requests.push_back(x);
}

//adds a request to the processes, used when the program first reads in the file
void process::addRequest(string x) {
	requests.push_back(x);
}

//deletes the requeust at the very top
void process::deleteRequest() {
	requests.erase(requests.begin());
}

//sets processn number to x
void process::setProcessNum(int x) {
	processNumber = x;
}

//returns process number
int process::getProcessNum() {
	return processNumber;
}

//sets the state of the string to x
void process::setState(string x) {
	state = x;
}

//returns state of the processes
string process::getState() {
	return state;
}

//sets arrivalTime to x
void process::setArrivalTime(int x) {
	arrivalTime = x;
}

//returns arrivalTime
int process::getArrivalTime() {
	return arrivalTime;
}