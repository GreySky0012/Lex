#pragma once
#include <map>
#include <vector>

using namespace std;

class StatementTable{
	map<char, int*> _table;
	string* _action;
	int _stateNum;
	int _start;
public:
	StatementTable(int stateNum,int start) :_stateNum(stateNum),_start(start){ _action = new string[_stateNum]; }
	void addState(int state, map<char, int> path,string action);
	string createCode();
};