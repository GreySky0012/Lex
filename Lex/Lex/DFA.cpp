#include "DFA.h"
#include "NFA.h"
#include "StatementTable.h"

int DFAState::_nextType = 2;

DFAState::DFAState(DFAState* from, char path){
	for each (Statement* s in from->_states)
	{
		Statement* end = s->checkEdge(path);
		if (end){
			addState(end);
		}
	}
	closure();
}

bool DFAState::equal(const DFAState*const state)
{
	if (_states.size() != state->_states.size()){
		return false;
	}
	else{
		for each (Statement* s in _states)
		{
			if (find(state->_states.begin(), state->_states.end(), s) == state->_states.end()){
				return false;
			}
		}
	}
	return true;
}

vector<char> DFAState::getPath(){
	vector<char> path;
	for each (Statement* s in _states)
	{
		auto _path = s->getPath();
		for each (char c in _path)
		{
			if (c != ' '&&find(path.begin(), path.end(), c) == path.end()){
				path.push_back(c);
			}
		}
	}
	return path;
}

void DFAState::checkTerminal(){
	for each (Statement* s in _states)
	{
		if (s->_action.compare("") == 0){
			_type = 1;
			return;
		}
	}
}

bool DFAState::checkType(DFAState* state){
	if (_switch.size() != state->_switch.size()){
		return false;
	}
	if (_switch.size() == 0){
		return false;
	}
	if (getAction().compare(state->getAction()) != 0){
		return false;
	}
	for each(auto p in _switch)
	{
		if (state->_switch.find(p.first) == state->_switch.end()){
			return false;
		}
		if (p.second != state->_switch[p.first]){
			return false;
		}
	}
	return true;
}

void DFAState::closure(){
	bool ret = true;
	while (ret)
	{
		ret = false;
		for(int i = 0; i < _states.size();i++)
		{
			Statement* end = _states[i]->checkEdge(' ');
			if (end&&addState(end)){
				ret = true;
			}
		}
	}
}

string DFAState::getAction()
{
	for each (Statement* s in _states)
	{
		if (s->_action.compare("") != 0){
			return s->_action;
		}
	}
	return "";
}

map<char, int> DFAState::getSwitch(){
	map<char, int> s;
	for each (auto p in _switch)
	{
		s.insert(pair<char, int>(p.first, p.second->_type));
	}
	return s;
}

DFA::DFA(NFA* nfa)
{
	Statement* s = nfa->getStart();
	DFAState* i0 = new DFAState(s);
	_start = i0;
	for each (Edge e in s->edges)
	{
		i0->_states.push_back(e._end);
	}
	_states.push_back(i0);
	for (int i = 0; i < _states.size(); i++){
		auto paths = _states[i]->getPath();
		for each (char c in paths)
		{
			DFAState* end = new DFAState(_states[i], c);
			bool x = true;
			for each (DFAState* s in _states)
			{
				if (s->equal(end)){
					x = false;
					_states[i]->addSwitch(c, s);
					break;
				}
			}
			if (x){
				_states.push_back(end);
				_states[i]->addSwitch(c, end);
			}
		}
	}
}

DFAState* DFA::findType(int type){
	if (type >= DFAState::_nextType){
		return nullptr;
	}
	for each (DFAState* s in _states){
		if (s->_type == type){
			return s;
		}
	}
}

void DFA::Minimize()
{
	for each (DFAState* s in _states)
	{
		s->checkTerminal();
	}
	checkType();
}

void DFA::checkType(){
	vector<DFAState*> types;
	types.push_back(_states[0]);
	bool y = true;
	while (y)
	{
		y = false;
		for (int i = 1; i < _states.size(); i++)
		{
			bool x = false;
			for each (auto t in types)
			{
				if (t->_type == _states[i]->_type){
					_states[i]->_type = t->_type;
					x = true;
					break;
				}
			}
			if (!x){
				types.push_back(_states[i]);
				continue;
			}
			x = false;
			for each (auto t in types)
			{
				if (t == _states[i]){
					x = true;
					break;
				}
				if (t->checkType(_states[i])){
					_states[i]->_type = t->_type;
					x = true;
					break;
				}
			}
			if (!x){
				_states[i]->_type = _states[i]->_nextType++;
				types.push_back(_states[i]);
				y = true;
			}
		}
	}
}

StatementTable DFA::getTable()
{
	StatementTable table(DFAState::_nextType,_start->_type);
	for (int i = 0; i < DFAState::_nextType; i++)
	{
		auto ds = findType(i);
		auto sw = ds->getSwitch();
		string action = "";
		for each (auto s in ds->_states)
		{
			if (s->_action.compare("")){
				action = s->_action;
				break;
			}
		}
		table.addState(i, sw, action);
	}
	return table;
}