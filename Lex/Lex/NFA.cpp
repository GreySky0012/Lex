#include "NFA.h"

NFA::NFA(string RE,string action) :_RE(RE)
{
	_start = new Statement(0);
	Statement* end = new Statement(1, action);
	_states.push_back(_start);
	_states.push_back(end);
	_statementNum += 2;
	structRE(RE, _start, end);
}

NFA::NFA()
{
	_start = new Statement(0);
	_states.push_back(_start);
	_statementNum++;
}

void NFA::structMul(string re1, string re2, Statement* start, Statement* end)
{
	Statement* s = new Statement(_statementNum++);
	_states.push_back(s);
	structRE(re1, start, s);
	structRE(re2, s, end);
}

void NFA::structStar(string re, Statement* start, Statement* end){
	string RE = re.substr(1, re.length()-3);
	Statement* s = new Statement(_statementNum++);
	_states.push_back(s);
	start->addEdge(Edge(start, s, ' '));
	s->addEdge(Edge(s, end, ' '));
	structRE(RE, s, s);
}

void NFA::structRE(string re, Statement* start, Statement* end)
{
	if (re.length() == 0){
		return;
	}
	if (re.length() == 1)
	{
		start->addEdge(Edge(start,end,re.at(0)));
		return;
	}
	if (re.at(0) == '('){
		int front = 1;
		int nowIndex = 0;
		while (front != 0){
			nowIndex++;
			int f_front = re.find_first_of('(', nowIndex);
			int f_back = re.find_first_of(')', nowIndex);
			if (f_front == -1){
				f_front = f_back + 1;
			}
			if (f_front < f_back){
				front++;
				nowIndex = f_front;
			}
			else
			{
				front--;
				nowIndex = f_back;
			}
		}
		nowIndex++;
		if (re.length() == nowIndex){
			structRE(re.substr(1, re.length() - 2), start, end);
			return;
		}
		if (re.at(nowIndex) == '*'){
			if (re.length() == nowIndex + 1){
				structStar(re, start, end);
				return;
			}
			else if(re.at(nowIndex+1) == '|'){
				string RE1 = re.substr(0, nowIndex + 1);
				string RE2 = re.substr(nowIndex+2);
				structRE(RE1, start, end);
				structRE(RE2, start, end);
				return;
			}
			else
			{
				string RE1 = re.substr(0, nowIndex + 1);
				string RE2 = re.substr(nowIndex + 1);
				structMul(RE1, RE2, start, end);
				return;
			}
		}
		if (re.at(nowIndex) == '|'){
			structRE(re.substr(0, nowIndex), start, end);
			structRE(re.substr(nowIndex+1), start, end);
			return;
		}
		structMul(re.substr(0, nowIndex), re.substr(nowIndex), start, end);
		return;
	}
	else
	{
		if (re.at(1) == '*'){
			if (re.length() == 2){
				structStar(re, start, end);
				return;
			}
			else if (re.at(2) == '|'){
				string RE1 = re.substr(0, 2);
				string RE2 = re.substr(3);
				structRE(RE1, start, end);
				structRE(RE2, start, end);
				return;
			}
			else
			{
				string RE1 = re.substr(0, 2);
				string RE2 = re.substr(2);
				structMul(RE1, RE2, start, end);
				return;
			}
		}
		else if (re.at(1) == '|')
		{
			structRE(re.substr(0, 1), start, end);
			structRE(re.substr(2), start, end);
			return;
		}
		else
		{
			structMul(re.substr(0, 1), re.substr(1), start, end);
			return;
		}
	}
}

void NFA::mergeNFA(NFA* nfa){
	_start->addEdge(Edge(_start, nfa->_start, ' '));
	int offset = _statementNum;
	for each (Statement* s in nfa->_states)
	{
		s->_id += offset;
	}
	_states.insert(_states.end(), nfa->_states.begin(), nfa->_states.end());
	_statementNum = _states.size();
}