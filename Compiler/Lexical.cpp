#include "pch.h"
#include "Lexical.h"




bool LexicalParser::isAlpha(char ch) {
	return (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z');
}

bool LexicalParser::isNumber(char ch) {
	return ch >= '0'&&ch <= '9';
}

bool LexicalParser::isBlank(char ch) {
	return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

int LexicalParser::FindNumberEdge(string text, int begin) {
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


Symbol LexicalParser::JudgeKey(string key)
{
	for (int i = 0; i < KeyLength; i++) {
		if (Keywords[i] == key)
			return Key2Symbol[i];
	}
	return variable;
}


int LexicalParser::FindVarInTable(string var)
{
	for (int i = 0, len = VarTable.size(); i < len; i++) {
		if (var == VarTable[i])
			return i;
	}
	return -1;
}

void LexicalParser::Error()
{
	cout << "语法错误！！" << endl;
}

void LexicalParser::ShowTables()
{
	cout << "词法分析\n二元式表 S-V Table 【共" << SVTable.size() << "个】" << endl;
	for (SVPair item : SVTable) {
		cout << "(" << item.symbol << ",\t" << item.value << ")"
			<< "\t" << SymbolToString(item.symbol) << endl;
	}
	cout << endl << "变量表 Var Table 【共" << VarTable.size() << "个】" << endl;
	for (int i = 0, len = VarTable.size(); i < len; i++) {
		cout << "(" << i << ",\t" << VarTable[i] << ")" << endl;
	}
	cout << endl;
}


bool LexicalParser::Parse(string text)
{
	SVTable.clear();
	VarTable.clear();

	int i = 0, len = text.length();
	while (i < len) {
		// 去掉空白
		while (i < len && isBlank(text[i])) i++;

		char ch = text[i];

		// 字母 -> 标识符 or 变量
		if (isAlpha(ch)) {
			int _end = i + 1;
			while (_end < len && (isAlpha(text[_end]) || isNumber(text[_end])))
				_end++;
			string key = text.substr(i, _end - i);

			Symbol symbol = JudgeKey(key);
			if (symbol != Symbol::variable) {
				SVTable.push_back(SVPair(symbol, 0));
			}
			else {	// 标识符是变量，先查变量表
				int index = FindVarInTable(key);
				if (index != -1) {	// 表中已有该变量
					SVTable.push_back(SVPair(symbol, index));
				}
				else {	// 表中没有则新建
					VarTable.push_back(key);
					SVTable.push_back(SVPair(symbol, VarTable.size() - 1));
				}
			}

			i = _end;
			continue;
		}

		// 数字 -> 只考虑整数	
		else if (isNumber(ch)) {
			// 确定是常数，常数的判断放到函数 findNumberEdge 中
			int _end = FindNumberEdge(text, i);
			int value = ParseInt(text.substr(i, _end - i));

			SVTable.push_back(SVPair(const_int, value));
			i = _end;
			continue;
		}

		// 其他操作符
		switch (ch)
		{
		case '+':
			SVTable.push_back(SVPair(Symbol::plus, 0));
			i++;
			break;
		case ';':
			SVTable.push_back(SVPair(semicolon, 0));
			i++;
			break;
		case '#':
			SVTable.push_back(SVPair(sharp, 0));
			i++;
			break;
		case '*':
			SVTable.push_back(SVPair(times, 0));
			i++;
			break;
		case '(':
			SVTable.push_back(SVPair(lparent, 0));
			i++;
			break;
		case ')':
			SVTable.push_back(SVPair(rparent, 0));
			i++;
			break;

			// := 赋值符号
		case ':':
			if (text[i + 1] == '=') {
				SVTable.push_back(SVPair(assign, 0));
				i += 2;
			}
			else {
				Error();
				return false;
			}
			break;

			// rop
		case '<':	// <= < <>
			if (text[i + 1] == '=') {
				SVTable.push_back(SVPair(rop, 1));
				i += 2;
			}
			else if (text[i + 1] == '>') {
				SVTable.push_back(SVPair(rop, 2));
				i += 2;
			}
			else {
				SVTable.push_back(SVPair(rop, 0));
				i++;
			}
			break;

		case '>':	// >= >
			if (text[i + 1] == '=') {
				SVTable.push_back(SVPair(rop, 4));
				i += 2;
			}
			else {
				SVTable.push_back(SVPair(rop, 3));
				i++;
			}
			break;

		case '=':
			SVTable.push_back(SVPair(rop, 5));
			i++;
			break;

		default:
			i++;
			break;
		}
	}

	return true;
}



string SymbolToString(Symbol sym)
{
	static vector<string> Sym2Str{
		"if","then","else","while","begin" ,"do","end","A",";","B","E",
		"#","S","L","Final","B∧","B∨","+","-","*",":=","not",
		"∧","∨","rop","(",")","i","ci","if_else","ifB","WB","W","LS"
	};
	return Sym2Str[sym];
}
	
Symbol StringToSymbol(string str)
{
	static map<std::string, Symbol> Str2Symbol{
		{"if" , sy_if},
		{"then" , sy_then},
		{"else" , sy_else},
		{"while" , sy_while},
		{"begin" , sy_begin},
		{"do" , sy_do},
		{"end" , sy_end},
		{"A" , A},
		{";" , semicolon},
		{"B" , B},
		{"E" , E},
		{"#" , sharp},
		{"S" , S},
		{"L" , L},
		{"FINAL" , FINAL},
		{"BA" , BA},
		{"BO" , BO},
		{"+" , Symbol::plus},
		{"-" , Symbol::minus},
		{"*" , times},
		{":=" , assign},
		{"not" , op_not},
		{"and" , op_and},
		{"or" , op_or},
		{"rop" , rop},
		{"(" , lparent},
		{")" , rparent},
		{"i" , variable},
		{"ci" , const_int},

		{"if_else" , if_else},
		{"ifB" , ifB},
		{"WB" , WB},
		{"W" , W},
		{"LS" , LS}
	};
	return Str2Symbol[str];
}

string RopToString(int index)
{
	static string rops[6]{ "<","<=","<>",">",">=","=" };
	return rops[index];
}

// 词法分析 的 测试主程序
void main_lexical_test() {
	string program = "\
		while (a > b) do\
			begin\
				if m >= n then a := a + 1\
				else\
					while k = h do x:= x + 2;\
				m:= n + x * (m + y)\
			end#";

	LexicalParser parser;
	parser.Parse(program);
	parser.ShowTables();
}
