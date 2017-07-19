#include "StatementTable.h"
#include <sstream>

string itos(int i){
	ostringstream ostr;
	ostr << i;
	return ostr.str();
}

void StatementTable::addState(int state, map<char, int> path,string action){
	_action[state] = action;
	for each (pair<char,int> p in path)
	{
		map<char,int*>::iterator i = _table.find(p.first);
		if (i == _table.end()){
			int* states = new int[_stateNum];
			_table.insert(pair<char, int*>(p.first, states));
			for (int i = 0; i < _stateNum; i++){
				_table[p.first][i] = -1;
			}
			_table[p.first][state] = p.second;
		}
		else{
			(*i).second[state] = p.second;
		}
	}
}

string StatementTable::createCode(){
	string s = "";
	s += "using namespace std;\n";
	s += "const string MYERROR=\"ERROR\";\n\n";

	//text is the string needed to be recognized
	s += "string analysis(string text)\n";
	s += "{\n";

	s += "\tint state= " + itos(_start) + ";\n";


	s += "\tint i=0;\n";
	s += "\tchar ch=text[i];\n";

	s += "\twhile(i<=text.length())\n";
	s += "\t{\n";
	s += "\t\tswitch(state)\n";
	s += "\t\t{\n";
	int len = _stateNum;
	string EROR = itos(len);
	for (int unsigned i = 0; i<len; i++)   //对每一个dfa状态都有一个case
	{
		int flagofelseif = 0;
		s += ("\t\tcase " +itos(i)+ ":\n");

		s += "\t\t\t{\n";//////////////////////{

		if (_action[i].compare(""))//该状态是终止状态
		{
			s += "\t\t\t\tif(i==text.length())\n";//已经识别完成打印出相应得动作
			s += "\t\t\t\t{\n";
			s += "\t\t\t\t";
			/////////////这一段代码是为了获得DFA对应动作的代码
			string str;
			string statement = _action[i];
			int size = statement.size();
			str = statement.substr(1, size - 2);
			////////////
			s += ("\t" + str + "\n");

			s += "\t\t\t\t\tbreak;\n";
			s += "\t\t\t\t}\n";

		}
		for each(pair<char,int*> p in _table)//逐个扫描该结点的边
		{
			if (p.second[i] == -1){
				continue;
			}
			s += "\t\t\t\t";//输出对边上字母判断的if.. 语句
			char path = p.first;
			s += "if(ch=='";
			if (path == '\''||path == '\\'||path == '\"')
				s += '\\';
			s += path;
			s += "')\n";
			s += "\t\t\t\t{\n";
			s += ("\t\t\t\t\tstate=" + itos(p.second[i]) + ";\n");
			s += "\t\t\t\t\tbreak;\n";
			s += "\t\t\t\t}\n";

		}
		s += "\t\t\t\telse\n";//遇到边上没有的字母，直接报错
		s += "\t\t\t\t{\n";
		s += ("\t\t\t\t\tstate=" + EROR + ";\n");
		s += "\t\t\t\t\tbreak;\n";
		s += "\t\t\t\t}\n";


		s += "\t\t\t}\n";
	}
	s += ("\t\tcase " + EROR + ":\n");
	s += "\t\t\t\treturn  MYERROR ;\n";
	s += "\t\t}\n";
	s += "\t\ti++;\n";
	s += "\t\tif (i!=text.length())\n";
	s += "\t\tch=text[i];  //ch is the next letter to be recongnized\n";
	s += "\t}\n";
	s += "\treturn  MYERROR ;\n";
	s += "\n}\n";

	return s;
}