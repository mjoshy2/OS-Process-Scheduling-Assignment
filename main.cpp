//Name: Martin Joshy
//PSID: 1495688
//Class: Principles of Operating Systems (COSC 3360)
//Homework 1 - Process Simulation

#include <iostream>
#include <vector>
#include "system.h"
using namespace std;

int main() {
	string word, filename;
	int number;
	int processCounter = -1;
	int numberOfCores;
	vector<process> processList;

	while (cin >> word && cin >> number) {
		if (word == "NCORES") {
			numberOfCores = number;
		}

		else if (word == "NEW") {
			processCounter++;
			process temp(processCounter, number);
			processList.push_back(temp);
		}

		else {
			processList.back().addRequest(word,number);
		}
	}

	for (int i = 0; i <= processCounter; i++) {
		processList[i].print();
		cout << endl;
	}
	processCounter++;
	
	System sys(processCounter, numberOfCores, processList);

	return 0;
}