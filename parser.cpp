#include "funcdraw.h"
#include "parser.h"

#ifndef PARSER_DEBUG
	#include "semantic.h"
#endif // PARSER_DEBUG

#ifdef PARSER_DEBUG
	#define enter(x) printf("enter in   ");	printf(x);	printf("\n");
#else
	#define enter(x)
#endif // PARSER_DEBUG

#ifdef PARSER_DEBUG
	#define back(x) printf("exit from   ");	printf(x);	printf("\n");
#else
	#define back(x)
#endif // PARSER_DEBUG

#ifdef PARSER_DEBUG
	#define call_match(x) printf("matchtoken   ");	printf(x);	printf("\n");
#else
	#define call_match(x)
#endif // PARSER_DEBUG

#ifdef PARSER_DEBUG
	#define Tree_trace(x) PrintSyntaxTree(x, 1);
#else
	#define Tree_trace
#endif // PARSER_DEBUG

#ifdef PARSER_DEBUG
	double Parameter = 0;
#else
	double Parameter = 0,					//参数T的存储空间
		   Origin_x = 0, Origin_y = 0,		//横、纵平移距离
		   Scale_x = 1, Scale_y = 1,		//横、纵比例因子
		   Rot_angle = 0;					//旋转角度
#endif // PARSER_DEBUG

static Token token;							//记号

//辅助函数声明
static void FetchToken();
static void MatchToken(enum Token_Type AToken);
static void SyntaxError(int case_of);
static void ErrMsg(unsigned LineNo, char *descrip, char *string);
static void PrintSyntaxTree(struct ExprNode * root, int indent);

//非终结符的递归子程序声明
static void Program();
static void Statement();
static void OriginStatement();
static void RotStatement();
static void ScaleStatement();
static void ForStatement();
static struct ExprNode * Expression();
static struct ExprNode * Term();
static struct ExprNode * Factor();
static struct ExprNode * Component();
static struct ExprNode * Atom();

//外部接口与语法树构造函数声明
extern void Parser(char * SrcFilePtr);
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...);

//通过词法分析器接口GetToken获取一个记号
static void FetchToken()
{
	token = GetToken();
	if (token.type == ERRTOKEN)	SyntaxError(1);
}

//匹配记号
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)	SyntaxError(2);
	FetchToken();
}

//语法错误处理
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
		case 1:	ErrMsg(LineNo, " 错误记号 ", token.lexeme);		break;
		case 2:	ErrMsg(LineNo, "不是预期信号", token.lexeme);	break;
	}
}

//打印错误信息
void ErrMsg(unsigned LineNo, char * descrip, char * string)
{
	#ifdef PARSER_DEBUG
		printf("Line No %5d:%s %s !\n", LineNo, descrip, string);
	#else
		char msg[256];
		memset(msg, 0, 256);
		sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);
	#endif // PARSER_DEBUG

	#ifdef _VC_COMPILER
		MessageBox(NULL, msg, "error!", MB_OK);
	#endif // _VC_COMPILER

	#ifdef _BC_COMPILER
		printf("%s\n", msg);
	#endif // _BC_COMPILER

		CloseScanner();
		exit(1);
}

//先序遍历并打印表达式的语法树
void PrintSyntaxTree(ExprNode * root, int indent)
{
	int temp;
	for (temp = 1; temp <= indent; temp++)	printf("\t");
	switch (root->OpCode)
	{
	case PLUS:		printf("%s\n", "+"); break;
	case MINUS:		printf("%s\n", "-"); break;
	case MUL:		printf("%s\n", "*"); break;
	case DIV:		printf("%s\n", "/"); break;
	case POWER:		printf("%s\n", "**"); break;
	case FUNC:		printf("%x\n", root->Content.FuncNode.mathFuncPtr); break;
	case CONST_ID:	printf("%f\n", root->Content.ConstNode); break;
	case T:			printf("%s\n", "T"); break;
	default:		printf("Error Tree Node !\n"); exit(0);
	}
	if (root->OpCode == CONST_ID || root->OpCode == T)		return;
	if (root->OpCode == FUNC)
		PrintSyntaxTree(root->Content.CaseFunc.child, indent + 1);
	else {
		PrintSyntaxTree(root->Content.CaseOperator.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperator.Right, indent + 1);
	}
}

//绘图语言解释器入口（与主程序的外部接口）
void Parser(char * SrcFilePtr)
{
	enter("Parser");
	if (!InitScanner(SrcFilePtr))
	{
		printf("Open Source File Error !\n");
		return;
	}
	FetchToken();
	Program();
	CloseScanner();
	back("Parser");
	return;
}

//Program的递归子程序
static void Program()
{
	enter("Program");
	while (token.type != NONTOKEN)
	{
		Statement();
		MatchToken(SEMICO);
	}
	back("Program");
}

//Statement的递归子程序
static void Statement()
{
	enter("Statement");
	switch (token.type)
	{
	case ORIGIN:	OriginStatement(); break;
	case SCALE:		ScaleStatement(); break;
	case ROT:		RotStatement(); break;
	case FOR:		ForStatement(); break;
	default:		SyntaxError(2);
	}
	back("Statement");
}

//OriginStatement的递归子程序
static void OriginStatement(void)
{
	struct ExprNode *tmp;
	enter("OriginStatement");
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	tmp = Expression();
#ifndef PARSER_DEBUG
	Origin_x = GetExprValue(tmp);
	DelExprTree(tmp);
#endif
	MatchToken(COMMA);
	tmp = Expression();
#ifndef PARSER_DEBUG
	Origin_y = GetExprValue(tmp);
	DelExprTree(tmp);
#endif
	MatchToken(R_BRACKET);
	back("OriginStatement");
}

//ScaleStatement的递归子程序
static void ScaleStatement(void)
{
	struct ExprNode *tmp;
	enter("ScaleStatement");
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	tmp = Expression();
#ifndef PARSER_DEBUG
	Scale_x = GetExprValue(tmp);
	DelExprTree(tmp);
#endif
	MatchToken(COMMA);
	tmp = Expression();
#ifndef PARSER_DEBUG
	Scale_y = GetExprValue(tmp);
	DelExprTree(tmp);
#endif
	MatchToken(R_BRACKET);
	back("ScaleStatement");
}

//RotStatement的递归子程序
static void RotStatement(void)
{
	struct ExprNode *tmp;
	enter("RotStatement");
	MatchToken(ROT);
	MatchToken(IS);
	tmp = Expression();
#ifndef PARSER_DEBUG
	Rot_angle = GetExprValue(tmp);
	DelExprTree(tmp);
#endif
	back("RotStatement");
}

//ForStatement的递归子程序
static void ForStatement(void)
{
#ifndef PARSER_DEBUG
	double Start, End, Step;
#endif
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;
	enter("ForStatement");
	MatchToken(FOR);	call_match("FOR");
	MatchToken(T);		call_match("T");
	MatchToken(FROM);	call_match("FROM");
	start_ptr = Expression();
#ifndef PARSER_DEBUG
	Start = GetExprValue(start_ptr);
	DelExprTree(start_ptr);
#endif
	MatchToken(TO);		call_match("TO");
	end_ptr = Expression();
#ifndef PARSER_DEBUG
	End = GetExprValue(end_ptr);
	DelExprTree(end_ptr);
#endif
	MatchToken(STEP);	call_match("STEP");
	step_ptr = Expression();
#ifndef PARSER_DEBUG
	Step = GetExprValue(step_ptr);
	DelExprTree(step_ptr);
#endif
	MatchToken(DRAW);	call_match("DRAW");
	MatchToken(L_BRACKET);	call_match("(");
	x_ptr = Expression();
	MatchToken(COMMA);	call_match(",");
	y_ptr = Expression();
	MatchToken(R_BRACKET);	call_match(")");
#ifndef PARSER_DEBUG
	DrawLoop(Start, End, Step, x_ptr, y_ptr);
	DelExprTree(x_ptr);
	DelExprTree(y_ptr);
#endif
	back("ForStatement");
}

//Expression的递归子程序
static struct ExprNode *Expression()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;
	enter("Expression");
	left = Term();
	while (token.type == PLUS || token.type == MINUS)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Term();
		left = MakeExprNode(token_tmp, left, right);
	}
	Tree_trace(left);
	back("Expression");
	return left;
}

//Term的递归子程序
static struct ExprNode *Term()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;
	left = Factor();
	while (token.type == MUL || token.type == DIV)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeExprNode(token_tmp, left, right);
	}
	return left;
}

//Factor的递归子程序
static struct ExprNode *Factor()
{
	struct ExprNode *left, *right;
	if (token.type == PLUS)
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS)
	{
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();
	return right;
}

//Component的递归子程序
static struct ExprNode *Component()
{
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//Atom的递归子程序
static struct ExprNode *Atom()
{
	struct Token t = token;
	struct ExprNode *address, *tmp;
	switch (token.type)
	{
	case CONST_ID:
		MatchToken(CONST_ID);
		address = MakeExprNode(CONST_ID, t.value);
		break;
	case T:
		MatchToken(T);
		address = MakeExprNode(T);
		break;
	case FUNC:
		MatchToken(FUNC);
		MatchToken(L_BRACKET);
		tmp = Expression();
		address = MakeExprNode(FUNC, t.FuncPtr, tmp);
		MatchToken(R_BRACKET);
		break;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		address = Expression();
		MatchToken(R_BRACKET);
		break;
	default:
		SyntaxError(2);
	}
	return address;
}

//生成语法树的一个节点
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...)
{
	struct ExprNode *ExprPtr = new (struct ExprNode);
	ExprPtr->OpCode = opcode;
	va_list ArgPtr;
	va_start(ArgPtr, opcode);
	switch (opcode)
	{
	case CONST_ID:
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T:
		ExprPtr->Content.CaseParmPtr = &Parameter;
		break;
	case FUNC:
		ExprPtr->Content.CaseFunc.MathFuncPtr
			= (FuncPtr)va_arg(ArgPtr, FuncPtr);
		ExprPtr->Content.CaseFunc.Child
			= (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	default:
		ExprPtr->Content.CaseOperator.Left
			= (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		ExprPtr->Content.CaseOperator.Right
			= (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	}
	va_end(ArgPtr);

	return ExprPtr;
}

