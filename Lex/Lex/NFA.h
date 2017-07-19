#pragma once
#include<string>
#include<vector>
#include<deque>
#include<list>
#include<iostream>
using namespace std;

struct Statement;

struct Edge
{
	Statement* _start;//the start statement
	Statement* _end;//the end statement
	char _symbol;
	Edge(Statement* start, Statement* end,char symbol) :_start(start), _end(end),_symbol(symbol){}
};

struct Statement{
	string _action;//is the statement is terminal
	int _id;//the id of this statement
	vector<Edge> edges;
	Statement(int id,string action = ""):_id(id), _action(action){}
	void addEdge(Edge edge)
	{
		edges.push_back(edge);
	}

	Statement* checkEdge(char path){
		for each (Edge e in edges)
		{
			if (e._symbol == path){
				return e._end;
			}
		}
		return nullptr;
	}

	vector<char> getPath(){
		vector<char> path;
		for each (Edge e in edges)
		{
			path.push_back(e._symbol);
		}
		return path;
	}
};

class NFA
{
	string _RE;
	int _statementNum = 0;
	vector<Statement*> _states;
	Statement* _start;

	void structMul(string re1,string re2, Statement* start, Statement* end);
	void structStar(string re, Statement* start, Statement* end);
	void structRE(string re, Statement* start, Statement* end);
	void mergeNFA(NFA* nfa);
public:
	NFA();
	NFA(string RE,string action);

	Statement* getStart(){
		return _start;
	}

	static NFA* merge(vector<NFA*> nfas){
		int num = nfas.size();
		int offset = 0;
		NFA* nfa = new NFA();
		for each (NFA* n in nfas)
		{
			nfa->mergeNFA(n);
		}
		return nfa;
	}
};