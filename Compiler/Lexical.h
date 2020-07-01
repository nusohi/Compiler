#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Utils.h"
using namespace std;


enum Symbol {
	sy_if,
	sy_then,
	sy_else,
	sy_while,
	sy_begin,
	sy_do,
	sy_end,
	A,			// 赋值语句
	semicolon,	// ;
	B,			// 布尔表达式
	E,
	sharp,		// #
	S,			// 语句
	L,			// 复合语句
	FINAL,		
	BA,			// 布尔表达式中的 B∧
	BO,			// 布尔表达式中的 B∨
	plus,		// +
	minus,
	times,		// *
	assign,		// :=
	op_not,		// not	┐
	op_and,		// and	∧
	op_or,		// or	∨
	rop,		// 关系运算符	< | <= | <> 不等于 | > | >= | = 等于
	lparent,	// (
	rparent,	// )
	variable,
	const_int,	// 整常量

	if_else,
	ifB,
	WB,
	W,
	LS
};


string SymbolToString(Symbol sym);
Symbol StringToSymbol(string str);
string RopToString(int index);

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
	int FindVarInTable(string var);	// 查找变量表中是否已含有某变量

	bool isAlpha(char ch);
	bool isNumber(char ch);
	bool isBlank(char ch);
	int FindNumberEdge(string text, int begin);	// 对常数的判别

	void Error();

	int KeyLength = 10;
	string Keywords[10]{ "while", "if", "then", "else", "begin", "do", "end", "and", "or", "not" };
	Symbol Key2Symbol[10]{ sy_while, sy_if, sy_then, sy_else, sy_begin, sy_do, sy_end, op_and, op_or, op_not };
};


// 测试主程序
void main_lexical_test();
