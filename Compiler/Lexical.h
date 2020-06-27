#pragma once
#include <string>
#include <vector>
using namespace std;


enum Symbol {
	key_while = 1,
	key_if,
	key_else,
	key_switch,
	key_case,
	Identifier,		// 标识符
	Number,			// 数字
	Op_plus,		// +
	Op_minus,		// -
	Op_multi,		// *
	Op_lessEqual,	// <=
	Op_less,		// <
	Op_equal,		// ==
	Op_assign,		// =
	Op_semicolon	// ;
};

struct SymbolValuePair {
	Symbol symbol;
	string value;

	SymbolValuePair(Symbol _symbol, string _value) {
		symbol = _symbol;
		value = _value;
	}
};


// 词法分析主函数
vector<SymbolValuePair*> LexicalParser(string text);

// 测试主程序
void main_lexical_test();
