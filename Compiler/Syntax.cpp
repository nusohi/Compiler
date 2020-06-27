#include "pch.h"
#include "Syntax.h"


SyntaxParser::SyntaxParser()
{
	// 初始化，建立TS、NS 与对应下标的 map
	for (int i = 0; i < TS.size(); i++)
		TS2Index[TS[i]] = i;
	for (int i = 0; i < NS.size(); i++)
		NS2Index[NS[i]] = i;
}


bool SyntaxParser::Parse(string text)
{
	Reset();
	Input = text;
	LogTitle();
	Log();

	for (Index = 0; Index < Input.size(); Index++) {
		while (Input[Index] != Stack.top()) {
			// 栈顶必须是 NS 且 PopAndPush 无错误（产生式不为空）
			if (!(TopIsNS() && PopAndPush())) {
				Error();
				return false;
			}
		}

		// 至此，栈顶符号与输入符号匹配
		Log("\t\t匹配");
		Stack.pop();
	}

	Success();
	return true;
}


void SyntaxParser::Reset()
{
	while (!Stack.empty()) Stack.pop();

	Stack.push('#');
	Stack.push('E');

	Index = 0;
}


bool SyntaxParser::TopIsNS()
{
	char top = Stack.top();
	for (char ns : NS) {
		if (ns == top)
			return true;
	}
	return false;
}


bool SyntaxParser::PopAndPush()
{
	// 查表得产生式 formula
	int i = NS2Index[Stack.top()],
		j = TS2Index[Input[Index]];
	string formula = Table[i][j];

	// 产生式为空时 判断语法出错
	if (formula == "") return false;

	string fullFormula = "";
	fullFormula.push_back(Stack.top());
	fullFormula += "->" + formula;

	Stack.pop();
	// 产生式为 A->ε 时仅 pop，否则逆序入栈
	if (formula != "ε") {
		for (int i = formula.size() - 1; i >= 0; i--)
			Stack.push(formula[i]);
	}

	Log(fullFormula);
	return true;
}


void SyntaxParser::Log(string formula)
{
	// 打印符号栈
	static stack<char> tmpStack;
	stringstream stackString;

	while (!Stack.empty()) {
		tmpStack.push(Stack.top());
		Stack.pop();
	}

	stackString << "[";
	while (!tmpStack.empty()) {
		stackString << tmpStack.top() << (tmpStack.size() == 1 ? "" : ", ");
		Stack.push(tmpStack.top());
		tmpStack.pop();
	}
	stackString << "]";
	cout  << setw(28) << left << stackString.str();

	// 打印 当前字符& 输入串 & 产生式
	cout << setw(10) << Input[Index] << setw(15) << Input.substr(Index) << '\t' << formula << endl;
}

void SyntaxParser::LogTitle()
{
	cout << endl << setw(28) << left << "符号栈"
		<<setw(10)<< "当前字符" << setw(15) << "输入串"
		<< '\t' << "所用产生式" << endl;
}

void SyntaxParser::Error()
{
	cout << "语法错误！！" << endl;
}

void SyntaxParser::Success()
{
	cout << "语法正确！！" << endl;
}



void main_syntax_test() {
	SyntaxParser parser;
	parser.Parse("i*(i+i)#");
	parser.Parse("i*(i+i#");
}