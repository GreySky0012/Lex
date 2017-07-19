#include<iostream>
#include<fstream>
#include<cstdlib>
#include<stdio.h>
#include<string>
#include<map>
#include<vector>
#include"NFA.h"
#include "DFA.h"
#include "StatementTable.h"

using namespace std;

//define the statement in the grammer text
const int ERROR = -1;
const int BEGIN = 1;
const int END = 2;
ifstream ifile;
ofstream ofile;
map<string, string>::iterator iter;

map<string, string>reTable;//存储正则表达式
map<string, string>reaction;//存储模式与动作
vector<string> actionTable;

struct RE{
	string id;
	string re;
}RE;

//skip the next newline character
void skipNewline(){
	char nextch = ifile.get();
	while (nextch == '\n'){
		nextch = ifile.get();
	}
	ifile.seekg(-1, ios::cur);
}

//check the next letter of %(check is now letter %)
int check(char ch)
{
	if (ch == '%')
	{
		char nextch = ifile.get();
		switch (nextch)
		{
		case '{':return BEGIN;
		case '}':
		case '%':return END;
		default:ifile.seekg(-1, ios::cur);
			break;
		}
	}
	return ERROR;
}

//check is the char letter or number
bool IsLetterNum(char ch)
{
	if ((ch <= '9'&&ch >= '0') || (ch <= 'z'&&ch >= 'a') || (ch <= 'Z'&&ch >= 'A'))
	{
		return true;
	}
	return false;
}

//conver the rule into REs
bool compleRe(string &re)
{
	int int_count;
	string str;
	string temp;
	map<string, string>::iterator iter;
	int i = 0,j;
	int offset;
	char ch = re[0],start,end;
	while (ch != ' ')
	{
		ch = re[i];
		switch (ch)
		{
		case'[':
			str.append(1, '(');
			++i;
			break;
		case']':
			str.append(1, ')');
			++i;
			break;
		case'-':
			start = re[i - 1];
			end = re[i + 1];
			str.erase(str.length() - 1, 1);
			if (IsLetterNum(re[i - 2]))
			{
				str.append(1, '|');
			}
			while (start<=end)
			{
				str.append(1, start++);
				str.append(1, '|');
			}
			i = i + 2;
			break;
		case'{':
			offset = re.find_first_of('}', i);
			for (j = i + 1; j<offset; j++)
			{
				temp.append(1, re[j]);
			}
			iter = reTable.find(temp);
			int_count = reTable.count(temp);
			if (int_count <= 0)
			{
				return false;
			}
			if (iter != reTable.end())
			{
				str.append(1, '(');
				str.append(iter->second);
				str.append(1, ')');
			}
			i = offset+1;
			temp = "";
			break;
		case'*':
			//str.append(1, re[i - 1]);
			str.append(1, '*');
			i++;
			break;
		case'|':
			str.append(1, '|');
			++i;
			break;
		case'"':
			offset = re.find_first_of('"', i + 1);
			temp = re.substr(i + 1, offset - i - 1);
			str.append(1, '(');
			str.append(temp);
			str.append(1, ')');
			i = offset+1;
			break;
		default:
			str.append(1, ch);
			i++;
			break;
		}
	}
	re = str;
	return true;
}

//read the definition segment and write it into the cpp file
void readDefinition(){
	int state = BEGIN;
	while (!ifile.eof() && state != END)
	{
		char nextch = ifile.get();
		if (nextch == '%')
		{
			state = check(nextch);
			continue;
		}
		//if there are some newline characters,write only one
		if (nextch == '\n')
		{
			skipNewline();
		}
		ofile.put(nextch);
	}
	cout << "The definition segment has been finished" << endl;
}

//read the rule segment and store these into the RE map
void readRule(){
	int state;

	cout << "The rule segment begin" << endl;
	state = BEGIN;
	string id, re;
	while (!ifile.eof() && state != END)
	{
		char nextch = ifile.get();
		//handle the newline character
		if (nextch == '\n')
		{
			continue;
		}
		//check the end of this segment
		if (nextch == '%')
		{
			state = check(nextch);
			if (state == ERROR)
			{
				cerr << "There is some error" << endl;
			}
			continue;
		}
		else
		{
			ifile.seekg(-1, ios::cur);
			ifile >> id >> re;
			cout << id << endl;
			cout << re << endl;
			re = re + ' ';
			compleRe(re);
			RE.id = id;
			RE.re = re;
		}
		reTable.insert(pair<string, string>(RE.id, RE.re));//put the RE into the map
		ifile.get();
	}

	for (iter = reTable.begin(); iter != reTable.end(); iter++)
	{
		cout << iter->first << " " << iter->second << endl;
	}
	cout << "The rule RE expression finished" << endl;
}

//relplace the re with terminal symbol
string replaceRE(string RE)
{
	map<string, string>::iterator i = reTable.find(RE);
	if (i == reTable.end()){
		return RE;
	}
	else
	{
		string final = "";
		string re = i->second;
		int nowIndex = 0;
		for each (char c in re)
		{
			switch (c){
			case '(':
				final.append('(', 1);
				while (true)
				{
					int f_or = re.find_first_of('|', nowIndex);
					int f_back = re.find_first_of(')', nowIndex);
					string theRE = "";
					if (f_or < f_back)
					{
						for (int i = nowIndex + 1; i < f_or; i++)
						{
							theRE.append(re.at(i), 1);
						}
						final += replaceRE(theRE);
						final.append('|', 1);
						nowIndex = f_or+1;
					}
					else
					{
						for (int i = nowIndex + 1; i < f_back; i++)
						{
							theRE.append(re.at(i), 1);
						}
						final += replaceRE(theRE);
						final.append(')', 1);
						nowIndex = f_back+1;
						break;
					}
				}
				break;
			case '*':
				final.append('*', 1);
			}
			nowIndex++;
		}
		return final;
	}
}

//read the keyword segment and write it into the cpp
void readKeyword(){
	int state = BEGIN;
	cout << "The keyword segment begin" << endl;
	pair<string, string>pi;
	while (!ifile.eof())
	{
		char nextch = ifile.get();
		if (nextch == '\n')
		{
			continue;
		}
		if (nextch == '%')
		{
			state = check(nextch);
			if (state == END)
			{
				break;
			}
		}
		ifile.seekg(-1, ios::cur);
		string str;
		string re, action;
		getline(ifile, str);
		string delim = " ";

		size_t offset = str.find_first_of(delim);
		re = str.substr(0, offset);
		while (str[offset] == ' ') offset++;
		action = str.substr(offset, str.size() - offset);
		//pi.first = replaceRE(re);
		if (reTable.find(re) == reTable.end())
		{
			pi.first = re;
		}
		else
		{
			pi.first = reTable[re];
		}
		pi.second = action;
		reaction.insert(pi);
		actionTable.push_back(action);
	}


	for (iter = reaction.begin(); iter != reaction.end(); iter++)
	{
		cout << iter->first << " " << iter->second << endl;
	}
}

int main()
{
	ofile.open("LexOut.cpp", ios::out);
	ifile.open("LexGrammer.txt", ios::in | ios::_Nocreate);
	if (!ifile)
	{
		cerr << "File could not be opened" << endl;
		exit(1);
	}
	char ch;
	ch = ifile.get();

	int state;

	//please sure the LexGrammer.txt is with the right format
	state = check(ch);

	if (state != BEGIN)
	{
		cerr << "The formation of the inputfile is wrong" << endl;
		exit(1);
	}
	
	readDefinition();

	skipNewline();

	readRule();

	skipNewline();

	readKeyword();

	vector<NFA*> nfas;
	for each(auto re in reaction){
		nfas.push_back(new NFA(re.first, re.second));
	}

	NFA* nfa = NFA::merge(nfas);
	DFA dfa(nfa);
	dfa.Minimize();
	auto table = dfa.getTable();

	ofile << table.createCode();

	ifile.get();
	while (!ifile.eof())
	{
		char nextch = ifile.get();
		if (nextch != EOF)
			ofile.put(nextch);
	}
	ifile.close();
	ofile.close();
	cout << "已完成" << endl;
	system("pause");
	return 0;
}

