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
		{"",	"+TA",	"",		"",		"��",	"��"},
		{"FB",	"",		"",		"FB",	"",		""},
		{"",	"��",	"*FB",	"",		"��",	"��"},
		{"i",	"",		"",		"(E)",	"",		""}
	};

private:
	stack<char> Stack;	// ����ջ
	map<char, int> TS2Index, NS2Index;
	string Input;		// ������ַ���
	int Index;			// ��ǰ������ַ��±�

public:
	SyntaxParser();
	bool Parse(string text);
private:
	void Reset();				// ����ջ & Index
	bool TopIsNS();				// �ж�ջ��Ԫ���Ƿ�Ϊ���ս��
	bool PopAndPush();			// ����ջ����������Ӳ���ʽ

	void Log(string formula = "");
	void LogTitle();
	void Error();
	void Success();
};



void main_syntax_test();
