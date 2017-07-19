#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<deque>
#include<stack>
using namespace std;

class StatementTable;
class NFA;
struct Statement;

struct DFAState{
	vector<Statement*> _states;
	map<char, DFAState*> _switch;
	int _type = 0;
	static int _nextType;

	DFAState(){}
	DFAState(Statement* start)
	{
		_states.push_back(start);
		closure();
	}
	DFAState(DFAState* from,char path);

	bool addState(Statement* state)
	{
		for (int i = 0; i < _states.size(); i++){
			if (_states[i] == state){
				return false;
			}
		}
		_states.push_back(state);
		return true;
	}
	void addSwitch(char path, DFAState* end){
		if (_switch.find(path) == _switch.end()){
			_switch.insert(pair<char, DFAState*>(path, end));
		}
	}
	bool equal(const DFAState*const state);
	bool isEmpty(){ return _states.empty(); }
	vector<char> getPath();
	void checkTerminal();
	bool checkType(DFAState* state);
	map<char, int> getSwitch();
private:
	string getAction();
	void closure();
};

class DFA{
	vector<DFAState*> _states;
	void checkType();
	DFAState* findType(int type);
	DFAState* _start;
public:
	DFA(NFA* nfa);
	void Minimize();
	StatementTable getTable();
};