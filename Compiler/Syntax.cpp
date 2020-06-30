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
	//SyntaxParser parser;
	//parser.Parse("i*(i+i)#");
	//parser.Parse("i*(i+i#");

	string program = " A +12 * C#";

	LexicalParser lexParser;
	lexParser.Parse(program);
	lexParser.ShowTables();

	SyntaxParser_SLR parser;
	parser.Parse(lexParser.SVTable, lexParser.VarTable);
	parser.LogFourList();
}


SyntaxParser_SLR::SyntaxParser_SLR()
{
	ExpListInit();
	InitTableHead();
	InitTable();
}

void SyntaxParser_SLR::ExpListInit()
{
	ExpList.push_back(new Expression({ S, { E } }));
	ExpList.push_back(new Expression({ E, { E, Symbol::plus, E } }));
	ExpList.push_back(new Expression({ E, { E, Symbol::times, E } }));
	ExpList.push_back(new Expression({ E, { lparent, E, rparent } }));
	ExpList.push_back(new Expression({ E, { variable } }));		// variable | const_int
	ExpList.push_back(new Expression({ E, { const_int } }));
}

void SyntaxParser_SLR::InitTableHead()
{
	vector<Symbol> heads{ 
		const_int, variable, Symbol::plus, times, lparent, rparent, sharp, E 
	};
	for (int i = 0, len = heads.size(); i < len; i++)
		TableHead[heads[i]] = i;
}

void SyntaxParser_SLR::InitTable()
{
	// 从文件中读取分析表
	ifstream file(SLRTablePath);
	vector<vector<string>> table;

	if (!file.is_open()) {
		cout << "文件读取错误" << endl;
		return;
	}

	char line[200];
	file.getline(line, 200);	// 去掉表头

	while (file.getline(line, 200)) {
		vector<string> s = Split(line, '\t');
		s.erase(s.begin());
		table.push_back(s);
	};
	file.close();

	int row = table.size(), col = table[0].size();
	Table = new TableItem_SLR*[table.size()];
	for (int i = 0; i < row; i++)
		Table[i] = new TableItem_SLR[col];

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (table[i][j][0] == 's') {
				Table[i][j].state = 1;
				Table[i][j].value = ParseInt(table[i][j].substr(1));
			}
			else if (table[i][j][0] == 'r') {
				Table[i][j].state = 2;
				Table[i][j].value = ParseInt(table[i][j].substr(1));
			}
			else if (table[i][j][0] == 'A') {
				Table[i][j].state = 3;
				Table[i][j].value = 0;
			}
			else if (table[i][j][0] >= '0'&&table[i][j][0] <= '9') {
				Table[i][j].state = 4;
				Table[i][j].value = ParseInt(table[i][j]);
			}
		}
	}
}

void SyntaxParser_SLR::InitStacks()
{
	while (!TheStack.empty())
		TheStack.pop();
	while (!StateStack.empty())
		StateStack.pop();
	// push push
	StateStack.push(0);
	TheStack.push(new Node{ sharp });
}

void SyntaxParser_SLR::Parse(vector<SVPair>& words, vector<string> varTable)
{
	InitStacks();
	VarTable = varTable;

	if (words[words.size() - 1].symbol != sharp)
		words.push_back({ sharp, 0 });

	cout << "==== Parse ============================" << endl;
	for (int i = 0, len = words.size(); i < len;) {
		TableItem_SLR tbItem = Table[StateStack.top()][TableHead[words[i].symbol]];	// 查表
		LogStackInfo();
		cout << '(' << tbItem.state << ',' << tbItem.value << ')' << endl;

		if (tbItem.state == 1) {
			Shift(words[i++], tbItem.value);	// i++
		}
		else if (tbItem.state == 2) {
			Reduce(tbItem.value);				// i 不动
		}
		else if (tbItem.state == 3) {
			cout << "ACC" << endl;
			return;
		}
		else if (tbItem.state == 4) {
			cout << "错误（GOTO）" << endl;
			return;
		}
		else {
			cout << "出错" << endl;
			return;
		}
	}
}

void SyntaxParser_SLR::Shift(SVPair word, int nextState)
{
	Node* node = new Node();
	node->symbol = word.symbol;
	TheStack.push(node);
	StateStack.push(nextState);

	switch (word.symbol)
	{
	case variable:
		node->place = word.value;			// 变量在变量表中的位置
		node->name = VarTable[node->place];	// 变量名
		break;
	case const_int:
		node->val = word.value;
		break;
	default:
		break;
	}
}

void SyntaxParser_SLR::Reduce(int expIndex)
{
	int len = ExpList[expIndex]->tail.size();
	vector<Node*> nodes;

	// 状态栈和语义栈出栈 && 建立 nodes
	for (int i = 0; i < len; i++) {
		nodes.push_back(TheStack.top());
		TheStack.pop();
		StateStack.pop();
	}
	nodes.push_back(new Node{ ExpList[expIndex]->head });
	reverse(begin(nodes), end(nodes));
	
	// 执行产生式对应的语义子程序
	SematicSubroutine(expIndex, nodes);

	// 根据规约之后的非终结符 来 新状态入栈 && 语义栈入栈
	GoTo(nodes[0]);
}

void SyntaxParser_SLR::GoTo(Node* node)
{
	// 查表（GOTO）
	TableItem_SLR tbItem = Table[StateStack.top()][TableHead[node->symbol]];
	TheStack.push(node);
	StateStack.push(tbItem.value);
}

int SyntaxParser_SLR::NewTemp()
{
	VarTable.push_back("$_T" + to_string(++tempVarCount));
	return VarTable.size() - 1;
}

void SyntaxParser_SLR::Emit(string op, string A, string B, string res)
{
	FourList.push_back({ op, A, B, res });
}

void SyntaxParser_SLR::LogFourList()
{
	cout << "========================================" << endl;
	for (Four f : FourList) {
		cout << f.str() << endl;
	}
	cout << "========================================" << endl;
}

void SyntaxParser_SLR::LogStackInfo() {
	static int count = 0;
	cout << ++count << '\t';
	stringstream str;

	stack<int> _states;
	while (!StateStack.empty()) {
		_states.push(StateStack.top());
		StateStack.pop();
	}
	while (!_states.empty()) {
		int temp = _states.top();
		StateStack.push(temp);
		_states.pop();
		str << temp << ' ';
	}
	cout << left << setw(20) << str.str();

	str.str("");	// clear
	stack<Node*> _nodes;
	while (!TheStack.empty()) {
		_nodes.push(TheStack.top());
		TheStack.pop();
	}
	while (!_nodes.empty()) {
		Node* temp = _nodes.top();
		TheStack.push(temp);
		_nodes.pop();
		str << SymbolToString(temp->symbol) << ' ';
	}
	cout << left << setw(20) << str.str();
}

void SyntaxParser_SLR::SematicSubroutine(int expIndex, vector<Node*>& nodes)
{
	switch (expIndex)
	{
	case 0:

		break;

	case 1:
		nodes[0]->place = NewTemp();
		nodes[0]->name = VarTable[nodes[0]->place];
		Emit("+", nodes[1]->nameOrVal(), nodes[3]->nameOrVal(), nodes[0]->name);
		break;

	case 2:
		nodes[0]->place = NewTemp();
		nodes[0]->name = VarTable[nodes[0]->place];
		Emit("*", nodes[1]->nameOrVal(), nodes[3]->nameOrVal(), nodes[0]->name);
		break;

	case 3:
		nodes[0]->place = nodes[1]->place;
		break;

	case 4:
		nodes[0]->place = nodes[1]->place;
		nodes[0]->name = nodes[1]->name;
		break;

	case 5:
		nodes[0]->val = nodes[1]->val;
		break;

	default:
		break;
	}
}
