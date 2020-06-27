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
	Identifier,		// ��ʶ��
	Number,			// ����
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


// �ʷ�����������
vector<SymbolValuePair*> LexicalParser(string text);

// ����������
void main_lexical_test();
