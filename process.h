#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

//represents an entire process in the system
class process {
private:
	int processNumber; //holds the number of the processes
	int arrivalTime; //holds the arrival time of the processes
	bool empty; //boolean variable to represent if there are elements in the process that have not been processesed
	string state; //state of the processes
	vector<string> requests; //is a vector of all the requeusts in the process

public:
	process();
	process(string x);

	void setProcessNum(int);
	int getProcessNum();

	void setState(string);
	string getState();

	void setArrivalTime(int x);
	int getArrivalTime();

	void addRequest(string);
	string getRequest();
	void deleteRequest();

};