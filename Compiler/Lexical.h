#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;


enum Symbol {
	sy_if = 0,
	sy_then = 1,
	sy_else = 2,
	sy_while = 3,
	sy_begin = 4,
	sy_do = 5,
	sy_end = 6,
	A = 7,			// 赋值语句
	semicolon = 8,	// ;
	E = 9,			// 布尔表达式
	sharp = 10,		// #
	S = 11,			// 语句
	L = 12,			// 复合语句
	temp_var = 15,	// 临时变量
	EA = 18,		// 布尔表达式中的 B∧
	EO = 19,		// 布尔表达式中的 B∨
	plus = 34,		// +
	minus,
	times = 36,		// *
	assign = 38,	// :=
	op_and = 39,	// and
	op_or = 40,		// or
	rop = 42,		// 关系运算符	< | <= | <> 不等于 | > | >= | = 等于
	lparent = 48,	// (
	rparent = 49,	// )
	variable = 56,
	const_int = 57,	// 整常量
};


// Symbol Value Pair
struct SVPair {
	Symbol symbol;
	int value;

	SVPair(Symbol _symbol, int _value) {
		symbol = _symbol;
		value = _value;
	}
};


class LexicalParser {
public:
	bool Parse(string text);

	vector<SVPair> SVTable;
	vector<string> VarTable;
	void ShowTables();

private:
	Symbol JudgeKey(string key);
	int ParseInt(string str);
	int FindVarInTable(string var);	// 查找变量表中是否已含有某变量

	bool isAlpha(char ch);
	bool isNumber(char ch);
	bool isBlank(char ch);
	int FindNumberEdge(string text, int begin);	// 对常数的判别

	void Error();

	int KeyLength = 9;
	string Keywords[9]{ "while", "if", "then", "else", "begin", "do", "end", "and", "or" };
	Symbol Key2Symbol[9]{ sy_while, sy_if, sy_then, sy_else, sy_begin, sy_do, sy_end, op_and, op_or };
};


// 测试主程序
void main_lexical_test();
