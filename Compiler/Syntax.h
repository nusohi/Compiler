#pragma once
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include "Lexical.h"
#include "Utils.h"
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


// 产生式
struct Expression {
	Symbol head;
	vector<Symbol> tail;
};


// SLR 分析表表项
struct TableItem_SLR {
	int state = 0;	// 0-出错 1-移进 2-规约 3-acc 4-GOTO
	int value = 0;	// 移进的状态 | 规约的产生式序号 | GOTO 的状态
};


// 语义信息节点
struct Node {
	Symbol symbol;	// 符号类型
	string name;	// 变量名
	int place;		// 在变量表 VarTable 中的位置		常量时为-1
	int val;		// 常量的值
	int tc, fc;		// true chain & false chain
	int chain, quad;

	// 变量时返回变量名 常量时返回常量值
	string nameOrVal() { return place == -1 ? to_string(val) : name; }
};


// 四元式
struct Four {
	string op, argA, argB, res;
	string str() { return "(" + op + ",\t" + argA + ",\t" + argB + ",\t" + res + ")"; }
};


class SyntaxParser_SLR {
public:
	// 初始化
	SyntaxParser_SLR();
	void InitExpList();
	void InitTable();		// 初始化 Table
	void InitStacks();		// 初始化 stacks

	// 分析主程序
	void Parse(vector<SVPair>& words, vector<string> varTable);

private:
	// 移进 规约 GOTO
	void Shift(SVPair word, int nextState);
	void Reduce(int expIndex);
	void GoTo(Node* node);

	// 临时变量 Emit 四元式ID
	int tempVarCount = 0;
	int NewTemp();
	void Emit(string op, string A, string B, string res);
	int NextFourID() { return FourList.size(); }

	// Backpatch Merge
	void Backpatch(int head, int newAddr);
	int Merge(int headA, int headB);
public:
	// Log
	void LogFourList();
	void LogStackInfo();
	void LogExpList();

	string StateToStr(int state);// 0-出错 1-移进 2-规约 3-acc 4-GOTO

private:
	// 语义子程序
	void SematicSubroutine(int expIndex, vector<Node*>& nodes);

	// SLR 分析表
	TableItem_SLR** Table;
	map<Symbol, int> TableHead;	// 分析表表头（Symbol 与列序号的映射关系）

	vector<string> VarTable;	// 变量表 + 临时变量表
	vector<Expression*> ExpList;// 产生式表
	vector<Four> FourList;		// 四元式表

	stack<Node*> TheStack;		// 符号栈 + 语义栈
	stack<int> StateStack;		// 状态栈

private:
	string SLRTablePath = "D://code//NELO//SLR//SLR_table.txt";		/// 暂
	string GrammarPath  = "D://code//NELO//SLR//exp.txt";			/// 暂
};


void main_syntax_test();
