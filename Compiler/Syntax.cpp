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

	string program = " \
			while (A<B) do\
				begin\
					if m>=n then a:=a+1\
					else\
						while k=h do x:=x+2;\
					m:=n+x*(m+y)\
				end#~\
		";

	LexicalParser lexParser;
	lexParser.Parse(program);
	lexParser.ShowTables();

	SyntaxParser_SLR parser;
	parser.LogExpList();
	parser.Parse(lexParser.SVTable, lexParser.VarTable);
	parser.LogFourList();
}


SyntaxParser_SLR::SyntaxParser_SLR()
{
	InitExpList();
	InitTable();
}

void SyntaxParser_SLR::InitExpList()
{
	//ExpList.push_back(new Expression({ S, { E } }));
	//ExpList.push_back(new Expression({ E, { E, Symbol::plus, E } }));
	//ExpList.push_back(new Expression({ E, { E, Symbol::times, E } }));
	//ExpList.push_back(new Expression({ E, { lparent, E, rparent } }));
	//ExpList.push_back(new Expression({ E, { variable } }));		// variable | const_int
	//ExpList.push_back(new Expression({ E, { const_int } }));


	// 从文件中读取文法
	ifstream file(GrammarPath);

	if (!file.is_open()) {
		cout << "文件读取错误：" << GrammarPath << endl;
		return;
	}

	char line[200];

	// 读取内容
	while (file.getline(line, 200)) {
		vector<string> s = Split(line, ' ');
		if (s.size() == 1 && s[0] == "EOF")	break;
		if (s.size() <= 1)	continue;

		Expression* exp = new Expression();
		exp->head = StringToSymbol(s[0]);
		for (int i = 1, len = s.size(); i < len; i++)
			exp->tail.push_back(StringToSymbol(s[i]));
		ExpList.push_back(exp);
	};
	file.close();
}

void SyntaxParser_SLR::InitTable()
{
	// 从文件中读取分析表
	ifstream file(SLRTablePath);
	vector<vector<string>> table;

	if (!file.is_open()) {
		cout << "文件读取错误：" << SLRTablePath << endl;
		return;
	}

	char line[200];

	// 读取表头
	file.getline(line, 200);
	vector<string> s = Split(line, '\t');
	for (int i = 1, len = s.size(); i < len; i++) {
		TableHead[StringToSymbol(s[i])] = i - 1;
	}

	// 读取内容
	while (file.getline(line, 200)) {
		s = Split(line, '\t');
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
		cout << StateToStr(tbItem.state) << " " << tbItem.value << endl;

		if (tbItem.state == 1) {
			Shift(words[i++], tbItem.value);	// i++
		}
		else if (tbItem.state == 2) {
			Reduce(tbItem.value);				// i 不动
		}
		else if (tbItem.state == 3) {
			Reduce(0);							// 执行语义子程序 case 0
			cout << "=====> ACC" << endl;
			return;
		}
		else if (tbItem.state == 4) {
			cout << "=====> 错误（GOTO）" << endl;
			return;
		}
		else {
			cout << "=====> 出错" << endl;
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
		node->place = -1;
		break;
	case rop:
		node->val = word.value;
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

void SyntaxParser_SLR::Backpatch(int head, int newAddr)
{
	int p = head;
	while (p != -1) {
		int next = ParseInt(FourList[p].res);
		FourList[p].res = to_string(newAddr);
		p = next;
	}
}

int SyntaxParser_SLR::Merge(int headA, int headB)
{
	if (headB == -1) 
		return headA;

	int p = headB, res;
	while ((res = ParseInt(FourList[p].res)) != -1)
		p = res;
	FourList[p].res = to_string(headA);
	return headB;
}

void SyntaxParser_SLR::LogFourList()
{
	cout << "=== 四元式 ==============================" << endl;
	for (int i = 0, len = FourList.size(); i < len; i++) {
		cout << left << setw(4) << i << "("
			<< left << setw(10) << FourList[i].op + ","
			<< left << setw(10) << FourList[i].argA + ","
			<< left << setw(10) << FourList[i].argB + ","
			<< FourList[i].res << ")" << endl;
	}
	cout << "========================================" << endl;
}

void SyntaxParser_SLR::LogStackInfo() {
	static int count = 0;
	cout << left << setw(3) << ++count << "|";
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
	cout << left << setw(45) << str.str();

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
	cout << left << setw(60) << str.str();
}

void SyntaxParser_SLR::LogExpList()
{
	cout << "== ExpList ==============================" << endl;
	for (int i = 0, len = ExpList.size(); i < len; i++) {
		cout << " " << i << "\t" << SymbolToString(ExpList[i]->head) << " -> ";
		for (Symbol s : ExpList[i]->tail)
			cout << SymbolToString(s) << " ";
		cout << endl;
	}
	cout << "=========================================" << endl;
}

string SyntaxParser_SLR::StateToStr(int state)
{
	static string state2Str[5]{ "出错","shift","reduce","ACC","goto" };
	return state2Str[state];
}

void SyntaxParser_SLR::SematicSubroutine(int expIndex, vector<Node*>& nodes)
{
	switch (expIndex)
	{
	case 0:			// FINAL -> S
		Backpatch(nodes[1]->chain, NextFourID());
		break;

	case 1:			// S->if_else S
		nodes[0]->chain = Merge(nodes[1]->chain, nodes[2]->chain);
		break;

	case 2:			// if_else->ifB then S else
		Emit("j", "_", "_", "-1");
		Backpatch(nodes[1]->chain, NextFourID());
		nodes[0]->chain = Merge(nodes[3]->chain, NextFourID() - 1);
		break;

	case 3:			// ifB->if B
		Backpatch(nodes[2]->tc, NextFourID());
		nodes[0]->chain = nodes[2]->fc;
		break;

	case 4:			// S->WB do S
		Backpatch(nodes[3]->chain, nodes[1]->quad);
		Emit("j", "_", "_", to_string(nodes[1]->quad));
		nodes[0]->chain = nodes[1]->chain;
		break;

	case 5:			// WB->W B
		Backpatch(nodes[2]->tc, NextFourID());
		nodes[0]->chain = nodes[2]->fc;
		nodes[0]->quad = nodes[1]->quad;
		break;

	case 6:			// W->while
		nodes[0]->quad = NextFourID();
		break;

	case 7:			// S->begin L end
		nodes[0]->chain = nodes[2]->chain;
		break;

	case 8:			// S->A
		nodes[0]->chain = -1;
		break;

	case 9:			// L->LS L
		nodes[0]->chain = nodes[2]->chain;
		break;

	case 10:		// LS->L ;
		Backpatch(nodes[1]->chain, NextFourID());
		break;

	case 11:		// L->S
		nodes[0]->chain = nodes[1]->chain;
		break;

	case 12:		// A->i := E
		Emit(":=", nodes[3]->nameOrVal(), "_", nodes[1]->nameOrVal());
		break;

	// B 布尔表达式 =============================================================
	case 13:		// B->BA B
		nodes[0]->tc - nodes[2]->tc;
		nodes[0]->fc = Merge(nodes[1]->fc, nodes[2]->fc);
		break;

	case 14:		// B->BO B
		nodes[0]->fc = nodes[2]->fc;
		nodes[0]->tc = Merge(nodes[1]->tc, nodes[2]->tc);
		break;

	case 15:		// B->not B
		nodes[0]->tc = nodes[2]->fc;
		nodes[0]->fc = nodes[2]->tc;
		break;

	case 16:		// B->(B)
		nodes[0]->tc = nodes[2]->tc;
		nodes[0]->fc = nodes[2]->fc;
		break;

	case 17:		// B->i rop i
		nodes[0]->tc = NextFourID();
		nodes[0]->fc = NextFourID() + 1;
		Emit("j" + RopToString(nodes[2]->val), nodes[1]->nameOrVal(), nodes[3]->nameOrVal(), "-1");
		Emit("j", "_", "_", "-1");
		break;

	case 18:		// B->i
		nodes[0]->tc = NextFourID();
		nodes[0]->fc = NextFourID() + 1;
		Emit("jnz", nodes[1]->nameOrVal(), "_", "-1");
		Emit("j", "_", "_", "-1");
		break;

	case 19:		// BA->B and
		Backpatch(nodes[1]->tc, NextFourID());
		nodes[0]->fc = nodes[1]->fc;
		break;

	case 20:		// BO->B or
		Backpatch(nodes[1]->fc, NextFourID());
		nodes[0]->tc = nodes[1]->tc;
		break;

	// E 算术表达式 =============================================================
	case 21:		// E->E+E
		nodes[0]->place = NewTemp();
		nodes[0]->name = VarTable[nodes[0]->place];
		Emit("+", nodes[1]->nameOrVal(), nodes[3]->nameOrVal(), nodes[0]->name);
		break;

	case 22:		// E->E*E
		nodes[0]->place = NewTemp();
		nodes[0]->name = VarTable[nodes[0]->place];
		Emit("*", nodes[1]->nameOrVal(), nodes[3]->nameOrVal(), nodes[0]->name);
		break;

	case 23:		// E->(E)
		nodes[0]->place = nodes[1]->place;
		nodes[0]->name = nodes[1]->name;
		break;

	case 24:		// E->i		即 E->variable
		nodes[0]->place = nodes[1]->place;
		nodes[0]->name = nodes[1]->name;
		break;

	case 25:		// E->ci	即 E->const_int
		nodes[0]->val = nodes[1]->val;
		nodes[0]->place = -1;
		break;

	default:
		cout << "规约错误" << endl;
		break;
	}
}
