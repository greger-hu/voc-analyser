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
	//�����ѽ�|��\|��<��\<��>��\>���֣����費�����ظ�����ʽ��
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
			//����Ƿ��Ѵ���..
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
	//���input�Ƿ�Ϊ���õ�<����>/<��ĸ>
	bool digit = false, alpha = false;
	if(input.find_first_not_of("0123456789") == string::npos)
	{
		digit = true;
	}
	if(input.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") == string::npos)
	{
		alpha = true;
	}
	//�����ǵ�ֵӳ��
	//���ƥ��Ĺ���
	for(int i = 0; i != rules.size(); i++)
	{
		if(rules[i][0] == state && rules[i][1] == input)
		{
			return rules[i][2];
		}
		if(digit)
		{
			if(rules[i][0] == state && rules[i][1] == "<����>")
			{
				return rules[i][2];
			}
		}
		if(alpha)
		{
			if(rules[i][0] == state && rules[i][1] == "<��ĸ>")
			{
				return rules[i][2];
			}
		}
	}
	return "";
}
vector<string> voc_analyser::check(string str)
{
	vector<string> result;//��,���Ͷ�
	//���str�ʷ��Ƿ���ȷ��
	//Լ��<div>��ʾ�ָ������û����Զ��壬���Ƿ��ս��������<div>
	//ȷ���ָ���
	/*string div = " ";//Ĭ��Ϊ�ո�
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
		//û��ƥ����ַ�ֱ��������
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
			//���state��Ϊ<#>����ʾƥ��ɹ���
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