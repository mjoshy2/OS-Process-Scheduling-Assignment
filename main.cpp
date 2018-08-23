#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "System.h"

using namespace std;

//reads a string and places the seperate words in a vector and returns the vector
vector<string> split(string x) {
	vector<string> temp;
	string buffer;
	stringstream ss(x);

	while (ss >> buffer)
		temp.push_back(buffer);
	return temp;
}

int main(int argc, char*argv[]) {
	string filename = argv[0,argc-1];

	ifstream file;
	file.open(filename);
	string line;

	getline(file, line);
	vector<string> first = split(line);

	vector<process> list(10);
	int processNum = -1;
	while (getline(file, line)) {
		if (line.substr(0, 3) == "NEW") {
			processNum++;
			vector<string> temp = split(line);
			list[processNum].setArrivalTime(stoi(temp[1]));
			list[processNum].setProcessNum(processNum);
		}
		else {
			list[processNum].addRequest(line);
		}
	}

	System simulation(processNum + 1,stoi(first[1]), list);
	
	system("pause");
	return 0;
}