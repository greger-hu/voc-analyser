#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <conio.h>
using namespace std;
class voc_analyser
{
private:
	
	
public:
	vector<string*> rules;
	void add(string rule);
	string match(string state, string input);
	void read();
	vector<string> check(string str);

	static vector<string> divide_text(string str, char ch);
};
vector<string> voc_analyser::divide_text(string str, char ch)
{
	vector<string> words;
	int p = 0, p1;
	while ((p1 = str.find(ch, p)) != string::npos)
	{
		words.push_back(str.substr(p, p1 - p));
		p = p1 + 1;
	}
	if (p < str.size())
		words.push_back(str.substr(p));
	return words;
}
void voc_analyser::add(string rule)
{
	string left = rule.substr(0, rule.find('='));
	string right = rule.substr(rule.find('=') + 1);
	//假设已将|和\|，<和\<，>和\>区分，假设不存在重复产生式。
	vector<string> formulas = divide_text(right, '|');
	for (int i = 0; i != formulas.size(); i++)
	{
		string *formula3 = new string[3];
		string formula = formulas[i];
		formula3[0] = left;
		vector<string> tmp = divide_text(formula, '<');
		if (tmp.size() == 1 || (tmp.size() == 2 && formula[0] == '<'))
		{
			formula3[1] = formula;
			formula3[2] = "<#>";
			//检查是否已存在..
		}
		else if(tmp.size() == 2 && formula[0] != '<')
		{
			 formula3[1] = tmp[0];
			 formula3[2] = "<" + tmp[1];
		}
		else
		{
			int p = formula.find('>');
			formula3[1] = formula.substr(0, p + 1);
			formula3[2] = formula.substr(p + 1);
		}
		rules.push_back(formula3);
	}
}
void voc_analyser::read()
{
	fstream fs("rules.txt");
	string rule;
	while(getline(fs, rule))
	{
		add(rule);
	}
}
string voc_analyser::match(string state, string input)
{
	//检查input是否为内置的<数字>/<字母>
	bool digit = false, alpha = false;
	if(input.find_first_not_of("0123456789") == string::npos)
	{
		digit = true;
	}
	if(input.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") == string::npos)
	{
		alpha = true;
	}
	//假设是单值映射
	//检查匹配的规则。
	for(int i = 0; i != rules.size(); i++)
	{
		if(rules[i][0] == state && rules[i][1] == input)
		{
			return rules[i][2];
		}
		if(digit)
		{
			if(rules[i][0] == state && rules[i][1] == "<数字>")
			{
				return rules[i][2];
			}
		}
		if(alpha)
		{
			if(rules[i][0] == state && rules[i][1] == "<字母>")
			{
				return rules[i][2];
			}
		}
	}
	return "";
}
vector<string> voc_analyser::check(string str)
{
	vector<string> result;//词,类型对
	//检查str词法是否正确。
	//约定<div>表示分隔符，用户可自定义，但是非终结符必须用<div>
	//确定分隔符
	/*string div = " ";//默认为空格
	for(int i = 0; i != rules.size(); i++)
	{
		if(rules[i][0] == "<div>")
		{
			div = rules[i][1];
		}
	}*/
	string type;
	string buf;
	string state;
	for(int i = 0; i != str.size(); i++)
	{
		bool initState = state.empty();
		bool ifMatch = false;
		if(initState)
		{
			for(int j = 0; j != rules.size(); j++)
			{
				if(!match(rules[j][0], string(1, str[i])).empty())
				{
					type = rules[j][0];
					buf += str[i];
					state = rules[j][2];
					ifMatch = true;
					break;
				}
			}
		}
		else
		{
			if(!match(state, string(1, str[i])).empty())
			{
				buf += str[i];
				state = match(state, string(1, str[i]));
				ifMatch = true;
			}
		}
		//没有匹配的字符直接跳过。
		if(!ifMatch)
		{
			if(!state.empty())
			{
				string m = match(state, "<null>");
				if(m == "<#>")
				{
					result.push_back(buf + "," + type);
				}
			}
			buf = "";
			state = "";
			type = "";
		}
		else
		{
			//如果state变为<#>，表示匹配成功。
			if(state == "<#>")
			{
				result.push_back(buf + "," + type);
				buf = "";
				state = "";
				type = "";
			}
		}
	}
	return result;
}
void main()
{
	voc_analyser va;
	va.read();
	fstream fs("test.cpp");
	stringstream ss;
	ss << fs.rdbuf();
	string code = ss.str();
	vector<string> res = va.check(code);
	for(int i = 0; i != res.size(); i++)
	{
		cout << res[i] << endl;
	}
	getch();
}