#include "pch.h"
#include "Syntax.h"


SyntaxParser::SyntaxParser()
{
	// ��ʼ��������TS��NS ���Ӧ�±�� map
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
			// ջ�������� NS �� PopAndPush �޴��󣨲���ʽ��Ϊ�գ�
			if (!(TopIsNS() && PopAndPush())) {
				Error();
				return false;
			}
		}

		// ���ˣ�ջ���������������ƥ��
		Log("\t\tƥ��");
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
	// ���ò���ʽ formula
	int i = NS2Index[Stack.top()],
		j = TS2Index[Input[Index]];
	string formula = Table[i][j];

	// ����ʽΪ��ʱ �ж��﷨����
	if (formula == "") return false;

	string fullFormula = "";
	fullFormula.push_back(Stack.top());
	fullFormula += "->" + formula;

	Stack.pop();
	// ����ʽΪ A->�� ʱ�� pop������������ջ
	if (formula != "��") {
		for (int i = formula.size() - 1; i >= 0; i--)
			Stack.push(formula[i]);
	}

	Log(fullFormula);
	return true;
}


void SyntaxParser::Log(string formula)
{
	// ��ӡ����ջ
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

	// ��ӡ ��ǰ�ַ�& ���봮 & ����ʽ
	cout << setw(10) << Input[Index] << setw(15) << Input.substr(Index) << '\t' << formula << endl;
}

void SyntaxParser::LogTitle()
{
	cout << endl << setw(28) << left << "����ջ"
		<<setw(10)<< "��ǰ�ַ�" << setw(15) << "���봮"
		<< '\t' << "���ò���ʽ" << endl;
}

void SyntaxParser::Error()
{
	cout << "�﷨���󣡣�" << endl;
}

void SyntaxParser::Success()
{
	cout << "�﷨��ȷ����" << endl;
}



void main_syntax_test() {
	SyntaxParser parser;
	parser.Parse("i*(i+i)#");
	parser.Parse("i*(i+i#");
}