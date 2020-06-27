#include "pch.h"
#include "Lexical.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;


string* Keywords = new string[5]{ "while", "if", "else", "switch", "case" };
Symbol* Key2Symbol = new Symbol[5]{ key_while, key_if, key_else, key_switch, key_case };


bool isAlpha(char ch) {
	return (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z');
}

bool isNumber(char ch) {
	return ch >= '0'&&ch <= '9';
}

bool isBlank(char ch) {
	return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

// 判断标识符是否是关键词
Symbol judgeKey(string identifier) {
	int len = Keywords->size();
	for (int i = 0; i < len; i++) {
		if (Keywords[i] == identifier)
			return Key2Symbol[i];
	}
	return Identifier;
}

// 对常数的判别
int findNumberEdge(string& text, int begin) {
	int status = 0, len = text.length();

	for (int i = begin; i < len; i++) {
		char ch = text[i];
		switch (status) {
		case 0:
			if (ch >= '0' && ch <= '9')
				status = 1;
			break;

		case 1:
			if (ch >= '0' && ch <= '9')
				status = 1;
			else if (ch == 'E')
				status = 4;
			else if (ch == '.')
				status = 2;
			else
				status = 7;
			break;

		case 2:
			if (ch >= '0' && ch <= '9')
				status = 3;
			break;

		case 3:
			if (ch >= '0' && ch <= '9')
				status = 3;
			else if (ch == 'E')
				status = 4;
			else
				status = 7;
			break;

		case 4:
			if (ch >= '0' && ch <= '9')
				status = 6;
			else if (ch == '+' || ch == '-')
				status = 5;
			break;

		case 5:
			if (ch >= '0' && ch <= '9')
				status = 6;
			break;

		case 6:
			if (ch >= '0' && ch <= '9')
				status = 6;
			else
				status = 7;
			break;

		case 7:
			return i - 1;	// [begin, i)
		}

	}
	
	return len - 1;			// [begin, len)
}


// 词法分析主函数
vector<SymbolValuePair*> LexicalParser(string text) {
	int i = 0, len = text.length();
	vector<SymbolValuePair*> table;

	while (i < len) {
		// 去掉空白
		while (i < len && isBlank(text[i]))
			i++;

		char ch = text[i];

		// 字母 -> 标识符
		if (isAlpha(ch)) {
			int _end = i + 1;

			while (_end < len && (isAlpha(text[_end]) || isNumber(text[_end])))
				_end++;
			string identifier = text.substr(i, _end - i);

			table.push_back(new SymbolValuePair(judgeKey(identifier), identifier));
			i = _end;
			continue;
		}

		// 数字 or +- -> 常数 or 操作符+-
		else if (isNumber(ch) || ch == '+' || ch == '-') {
			// 判断是操作符+-
			if (ch == '+' && !isNumber(text[i + 1])) {
				table.push_back(new SymbolValuePair(Op_plus, "+"));
				i++;
				continue;
			}
			else if (ch == '-' && !isNumber(text[i + 1])) {
				table.push_back(new SymbolValuePair(Op_minus, "-"));
				i++;
				continue;
			}

			// 确定是常数，常数的判断放到函数 findNumberEdge 中
			int begin = (ch == '+' || ch == '-') ? i + 1 : i;
			int _end = findNumberEdge(text, begin);

			table.push_back(new SymbolValuePair(Number, text.substr(i, _end - i)));
			i = _end;
			continue;
		}

		// 其他操作符
		switch (ch)
		{
		case '*':
			table.push_back(new SymbolValuePair(Op_multi, "*"));
			i++;
			break;

		case '<':
			if (text[i + 1] == '=') {
				table.push_back(new SymbolValuePair(Op_lessEqual, "<="));
				i += 2;
			}
			else {
				table.push_back(new SymbolValuePair(Op_less, "<"));
				i++;
			}
			break;

		case '=':
			if (text[i + 1] == '=') {
				table.push_back(new SymbolValuePair(Op_equal, "=="));
				i += 2;
			}
			else {
				table.push_back(new SymbolValuePair(Op_assign, "="));
				i++;
			}
			break;

		case ';':
			table.push_back(new SymbolValuePair(Op_semicolon, ";"));
			i++;
			break;

		default:
			i++;
			break;
		}
	}

	return table;
}


void main_lexical_test() {
	string program = "while case  value =23;\n test=+322.22 ; hello = -3.45E+12;";
	vector<SymbolValuePair*> table;

	table = LexicalParser(program);

	for (SymbolValuePair* item : table) {
		cout << "<" << item->symbol << ",\t" << item->value << ">" << endl;
	}
}
