#pragma once
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;


class SyntaxParser {
public:
	vector<char> TS{ 'i','+','*','(',')','#' };	// Termial Symbol
	vector<char> NS{ 'E','A','T','B','F' };		// Nontermial Symbol	// A==E' B==T'
	vector<vector<string>> Table{
		{"TA",	"",		"",		"TA",	"",		""},
		{"",	"+TA",	"",		"",		"ε",	"ε"},
		{"FB",	"",		"",		"FB",	"",		""},
		{"",	"ε",	"*FB",	"",		"ε",	"ε"},
		{"i",	"",		"",		"(E)",	"",		""}
	};

private:
	stack<char> Stack;	// 符号栈
	map<char, int> TS2Index, NS2Index;
	string Input;		// 输入的字符串
	int Index;			// 当前处理的字符下标

public:
	SyntaxParser();
	bool Parse(string text);
private:
	void Reset();				// 重置栈 & Index
	bool TopIsNS();				// 判断栈顶元素是否为非终结符
	bool PopAndPush();			// 弹出栈顶并逆序添加产生式

	void Log(string formula = "");
	void LogTitle();
	void Error();
	void Success();
};



void main_syntax_test();
