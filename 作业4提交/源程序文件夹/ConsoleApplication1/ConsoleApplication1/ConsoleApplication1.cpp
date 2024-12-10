#include<iostream>
#include<string>
#include<math.h>
#include<map>

using namespace std;

enum TokenID {
	IF, END, REPEAT, UNTIL, ELSE, READ, WRITE, THEN, ASSIGN, SEMI, ID, NUMBER, ADD, SUB, MUL, DIV, LBRACKET, RBRACKET, ENDFILE,
	//添加while循环
	WHILE, DO, ENDDO, FALSEENTRY, TRUEENTRY,
	//添加算术表达式的运算符号
	SUB_ASSIGN,// -=
	ADD_ASSIGN,// +=
	MOD, // %
	POW, // ^
	//添加比较运算符号
	GRE,//大于
	LEQ,//小于等于
	GEQ,//大于等于
	UEQ //不等于
};

map<string, TokenID>keywords;


struct TokenStru  // Token 结构
{
	TokenID  ID;
	int val;
	char op;
	char word[20];
};

struct OperandsNode  // 四元组操作数结构
{
	int flag;   // 操作数是值=0，还是临时变量=1，空=2
	int val;     // 操作数为值时则存放值
	char NewVar[10];  // 操作数为临时变量时则存放临时变量名
};

//添加：用于记录下一个四元组编号
struct entry
{
	int index;
	TokenStru token;
	OperandsNode x;
	OperandsNode y;
	OperandsNode z;
};

//添加比较运算符
OperandsNode rop_exp();
OperandsNode exp();
OperandsNode term();
//添加乘方
OperandsNode power();
OperandsNode factor();

TokenStru token;

//添加： key 是四元组编号，value 是 entry 结构
map<int, entry> gen_map;

int varOrder = 1;  // 记录当前准备产生的临时变量序号
//添加：记录下一个即将形成的四元组编号
int NextGen = 1;
//添加：in_while
bool in_while = false;

char buffer[255];
int pos = 0;

void GetToken()
{
	int s;

	while (buffer[pos] == 32 || buffer[pos] == 9)  // spacebar or tab 
		pos++;

	if ((buffer[pos] >= '0') && (buffer[pos] <= '9'))  // digit
	{
		s = 0;
		while (buffer[pos] >= '0' && buffer[pos] <= '9')
		{
			s = s * 10 + buffer[pos] - '0';   pos++;
		}
		token.ID = NUMBER; token.val = s;
	}
	else if (isalpha(buffer[pos]))
	{
		int len = 0;
		while (isalpha(buffer[pos]) || isdigit(buffer[pos]) || buffer[pos] == '_')
		{
			token.word[len] = buffer[pos];
			len++;
			pos++;
		}
		token.word[len] = '\0';

		map<string, TokenID>::iterator it;
		it = keywords.find(token.word);    // key word
		if (it != keywords.end())
			token.ID = keywords[token.word];
		else token.ID = ID;

	}
	else if (buffer[pos] != '\0')
	{
		switch (buffer[pos])
		{
		case '+':
		{
			//判断+=
			if (buffer[pos + 1] == '=')
			{
				pos++; token.ID = ADD_ASSIGN; strcpy(token.word, "+="); break;
			}
			// 判断+
			else
			{
				token.ID = ADD; token.op = buffer[pos];  break;
			}
		}
		case '-':
		{
			//判断-=
			if (buffer[pos + 1] == '=')
			{
				pos++; token.ID = SUB_ASSIGN; strcpy(token.word, "-="); break;
			}
			// 判断-
			else
			{
				token.ID = SUB; token.op = buffer[pos];  break;
			}
		}
		case '*': token.ID = MUL; token.op = buffer[pos];  break;   // *
		case '/': token.ID = DIV; token.op = buffer[pos];  break;   // /
			//添加求余
		case'%':token.ID = MOD; token.op = buffer[pos]; break; // %
			//添加乘方
		case'^':token.ID = POW; token.op = buffer[pos]; break; // ^
		case '(': token.ID = LBRACKET; token.op = buffer[pos];  break;
		case ')': token.ID = RBRACKET; token.op = buffer[pos];  break;
		case ';': token.ID = SEMI; token.op = buffer[pos];  break;
		case ':': if (buffer[pos + 1] == '=')
		{
			pos++; token.ID = ASSIGN; strcpy(token.word, ":="); break;
		}
				else cout << " Error Input at: " << pos + 1; exit(1);
		case '$': token.ID = ENDFILE; break;
		case'>':
		{
			//判断 >=
			if (buffer[pos + 1] == '=')
			{
				pos++; token.ID = GEQ; strcpy(token.word, ">="); break;
			}
			//判断 >
			else
			{
				token.ID = GRE; token.op = buffer[pos];  break;
			}
		}
		case '<':
		{
			//判断 <>
			if (buffer[pos + 1] == '>')
			{
				pos++; token.ID = UEQ; strcpy(token.word, "<>"); break;
			}
			//判断 <=
			else if (buffer[pos + 1] == '=')
			{
				pos++; token.ID = LEQ; strcpy(token.word, "<="); break;
			}
			else
			{
				cout << " Error Input at: " << pos + 1; exit(1);
			}
		}
		default:  cout << " Error Input at: " << pos + 1; exit(1);
		}
		pos++;
	}
	else
	{
		cin.getline(buffer, 255);
		pos = 0;
		GetToken();
	}
	if (token.ID == ENDFILE) exit(1);
}

void match(TokenID expecttokenid)
{
	if (token.ID == expecttokenid)
		GetToken();
	else { cout << " Error Input at position: " << pos + 1; exit(1); }

}

void OutputOperand(OperandsNode x)  // 输出一个操作数
{

	cout << " , ";
	if (x.flag == 0)   // 根据其值输出值还是临时变量名作为操作数
		cout << x.val;
	else if (x.flag == 1)
		cout << x.NewVar;
	else cout << " ";
}

void OutputOP(TokenStru token)
{
	//添加：判断真出口假出口
	if (token.ID == GEQ || token.ID == UEQ || token.ID == LEQ ||
		token.ID == GRE)
		cout << "J";
	//更改这里，输出算法表达式的运算符号
	//输出扩充的比较运算符，注意不能包含大于（因为大于是操作符，只有一个字符）
	if (token.ID == ASSIGN || token.ID == READ || token.ID == WRITE || token.ID == SUB_ASSIGN || token.ID == ADD_ASSIGN || token.ID == LEQ || token.ID == GEQ || token.ID == UEQ
		|| token.ID == FALSEENTRY || token.ID == TRUEENTRY)
		cout << token.word;   // 输出操作码
	else
		cout << token.op;   // 输出操作码，即运算符
}

//添加：输出因为while的假出口编号不能确定导致的延后输出四元组
void output_later(int start_index, int end_index) {
	for (int i = start_index; i < end_index; i++) {
		cout << "(" << i << ") ";
		cout << "(";
		OutputOP(gen_map[i].token);   // 输出操作码，即运算符
		OutputOperand(gen_map[i].x);  // 第一个操作数
		OutputOperand(gen_map[i].y);   // 第二个操作数
		OutputOperand(gen_map[i].z);   // 第三个操作数
		cout << " ) ";
		cout << endl;
	}
}

void Gen(TokenStru token, OperandsNode x, OperandsNode y, OperandsNode z)  // 产生一条四元组
{
	entry entry;
	entry.index = NextGen;
	entry.token = token;
	entry.x = x;
	entry.y = y;
	entry.z = z;

	//存储四元组到map
	gen_map[NextGen++] = entry;
	//如果当前生成的四元组是在while里的，不进行输出
	if (!in_while) {
		//添加：每次输出的时候,顺便输出编号
		cout << "(" << NextGen - 1 << ")" << " ";
		cout << "(";
		OutputOP(token);   // 输出操作码，即运算符
		OutputOperand(x);  // 第一个操作数
		OutputOperand(y);   // 第二个操作数
		OutputOperand(z);   // 第三个操作数
		cout << " ) ";
		cout << endl;
	}
}

OperandsNode rop_exp()
{
	//扩充比较表达式,比较表达式的运算优先级最低的，所以要新开一个函数在exp的前面
	TokenStru op;
	char str[5];
	OperandsNode x, y, z;

	x = exp();   // 获取一个操作数

	//只有遇到了比较符号才需要进入这个循环，也就是说进入这个循环的时候会出现要处理“真出口”和“假出口”的情况，这里处理生成真出口的情况
	while (token.ID == GRE || token.ID == GEQ || token.ID == LEQ || token.ID == UEQ)
	{
		op = token;
		match(token.ID);
		y = exp();    // 获取一个操作数

		//更改这里，让z成为指示编号的位置，初始时置空
		z.flag = 0;
		z.val = NextGen + 2;//下下个编号才是真出口的内容

		Gen(op, x, y, z);    //生成一个四元组
		x = z;
	}
	return x;
}

OperandsNode exp()
{
	TokenStru op;
	char str[5];
	OperandsNode x, y, z;

	x = term();   // 获取一个操作数

	while (token.ID == ADD || token.ID == SUB)
	{
		op = token;
		match(token.ID);
		y = term();    // 获取一个操作数

		z.flag = 1; strcpy(z.NewVar, "t");  // 产生一个新的临时变量
		_itoa(varOrder, str, 10); strcat(z.NewVar, str);

		varOrder++;// 产生了一个新临时变量后，序号+1
		Gen(op, x, y, z);    //生成一个四元组
		x = z;
	}
	return x;
}

OperandsNode term()
{
	TokenStru op;

	char str[5];
	OperandsNode x, y, z;

	x = power();// 获取一个操作数

	while (token.ID == MUL || token.ID == DIV || token.ID == MOD)//添加求余
	{
		op = token;
		match(token.ID);
		y = power();// 获取一个操作数

		z.flag = 1; strcpy(z.NewVar, "t");   // 产生一个新的临时变量
		_itoa(varOrder, str, 10); strcat(z.NewVar, str);

		varOrder++;  // 产生了一个新临时变量后，序号+1
		Gen(op, x, y, z);  //生成一个四元组
		x = z;
	}
	return x;
}

OperandsNode power()
{
	//和实验三一样的，因为乘方是右结合的，所以需要进行递归
	TokenStru op;

	char str[5];
	OperandsNode x, y, z;

	x = factor();// 获取一个操作数

	while (token.ID == POW)
	{
		op = token;
		match(token.ID);
		y = power();// 获取一个操作数

		z.flag = 1; strcpy(z.NewVar, "t");   // 产生一个新的临时变量
		_itoa(varOrder, str, 10); strcat(z.NewVar, str);

		varOrder++;  // 产生了一个新临时变量后，序号+1
		Gen(op, x, y, z);  //生成一个四元组
		x = z;
	}
	return x;
}

OperandsNode factor()
{
	OperandsNode x;

	switch (token.ID)
	{
	case LBRACKET:  match(LBRACKET);
		x = rop_exp();   // 产生一个操作数
		match(RBRACKET);
		break;
	case NUMBER:    x.flag = 0; x.val = token.val;  // 产生一个操作数
		match(NUMBER);
		break;
	case ID:        x.flag = 1; strcpy(x.NewVar, token.word);  // 产生一个操作数
		match(ID);
		break;
	default: { cout << "error input at position: " << pos + 1;  exit(1); }

	}
	return x;
}

void statement(void);

void stmt_sequence(void)
{
	statement();
	while ((token.ID != ENDFILE) && (token.ID != END) &&
		(token.ID != ELSE) && (token.ID != UNTIL) && (token.ID != ENDDO))// 添加enddo
	{
		match(SEMI);
		statement();
	}
	return;
}

void assign_stmt(void)
{
	OperandsNode x, y, z;
	TokenStru op;

	y.flag = 2;
	z.flag = 1;
	strcpy(z.NewVar, token.word);

	match(ID);
	op = token;
	//更改这里，匹配算术运算符号和比较运算符号
	switch (token.ID) {
	case ASSIGN:match(ASSIGN); break;
	case ADD_ASSIGN:match(ADD_ASSIGN); break;
	case SUB_ASSIGN:match(SUB_ASSIGN); break;
	case GRE:match(GRE); break;
	case LEQ:match(LEQ); break;
	case GEQ:match(GEQ); break;
	case UEQ:match(UEQ); break;
	}


	x = rop_exp();
	Gen(op, x, y, z);  //生成一个四元组

	return;
}

void read_stmt(void)
{
	OperandsNode x, y, z;
	TokenStru op;

	op = token;
	match(READ);

	x.flag = y.flag = 2;
	z.flag = 1;
	strcpy(z.NewVar, token.word);
	match(ID);

	Gen(op, x, y, z);  //生成一个四元组

	return;
}

void write_stmt(void)
{

	OperandsNode x, y, z;
	TokenStru op;

	op = token;
	match(WRITE);

	x.flag = y.flag = 2;

	z = rop_exp();

	Gen(op, x, y, z);  //生成一个四元组

	return;

}

void repeat_stmt(void)
{
	//补全repeat语法，语法的规则是repeat stmt-sequence until exp
	match(REPEAT);
	stmt_sequence();
	match(UNTIL);
	rop_exp();
	return;
}

void if_stmt(void)
{
	match(IF);
	rop_exp();
	match(THEN);
	stmt_sequence();
	if (token.ID == ELSE) {
		match(ELSE);
		stmt_sequence();
	}
	match(END);
	return;
}

void while_stmt(void)
{
	//添加while语法，语法的规则是while exp do stmt-sequence enddo
	match(WHILE);
	rop_exp();
	//生成假出口的语句
	entry false_entry;
	false_entry.token.ID = FALSEENTRY;
	false_entry.index = NextGen;
	strcpy(false_entry.token.word, "J");
	in_while = true; //设置判断，当前进入了while循环
	match(DO);
	stmt_sequence();
	match(ENDDO);
	//生成真出口的语句
	entry true_entry;
	true_entry.token.ID = TRUEENTRY;
	true_entry.index = NextGen;
	strcpy(true_entry.token.word, "J");
	true_entry.z.flag = 0;
	true_entry.z.val = false_entry.index - 1;//因为在本次的实验中，不要求and or的判断，所以真出口总是在假出口的上一个编号
	gen_map[NextGen++] = true_entry;
	//回填真出口
	false_entry.z.flag = 0;
	false_entry.z.val = NextGen;
	gen_map[false_entry.index] = false_entry;
	output_later(false_entry.index, NextGen); //将延迟的输出进行输出
	in_while = false;//while语句完成，设回false
	return;
}

void statement(void)
{
	switch (token.ID)
	{
	case IF:  if_stmt(); break;
	case REPEAT:repeat_stmt(); break;
	case ID:assign_stmt(); break;
	case READ:  read_stmt(); break;
	case WRITE: write_stmt(); break;

		//为if语句添加end关键字
	case END:break;
	case ELSE:break;

		// 添加while循环
	case WHILE:while_stmt(); break;
	case ENDDO:break;
	default: cout << "unexpected token";
		GetToken();
		break;
	}
	return;
}

void init()
{
	keywords["if"] = IF;

	//为if语句添加then,end关键字
	keywords["then"] = THEN;
	keywords["END"] = END;

	keywords["else"] = ELSE;
	keywords["read"] = READ;
	keywords["write"] = WRITE;

	//添加while循环
	keywords["while"] = WHILE;
	keywords["do"] = DO;
	keywords["enddo"] = ENDDO;
}

void main()
{
	init();

	cin.getline(buffer, 255);
	pos = 0;

	GetToken();

	stmt_sequence();

	if (token.ID == ENDFILE)
		cout << "The result is correct! " << endl;
	else cout << "error input at position: " << pos + 1;

	system("pause");

}