#ifndef SCANNER_H
#define SCANNER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <atlstr.h>
typedef struct Token{
	enum Token_Type  type;	// 类别，见下页
	char *lexeme;// 属性，原始输入的字符串，亦可为数组
	double value;// 属性，若记号是常数则存常数的值
	double(*FuncPtr)(double);
	// 属性，若记号是函数则存函数地址

};
enum Token_Type {

	ID, COMMENT,   // 参见正规式设计
	ORIGIN, SCALE, ROT, IS,	  // 保留字（一字一码）
	TO, STEP, DRAW, FOR, FROM, // 保留字
	T,				  // 参数
	SEMICO, L_BRACKET, R_BRACKET, COMMA,// 分隔符
	PLUS, MINUS, MUL, DIV, POWER,		// 运算符
	FUNC,				  // 函数（调用）
	CONST_ID,			  // 常数
	NONTOKEN,			  //空记号（源程序结束）
	ERRTOKEN			  // 出错记号（非法输入）
};

static Token TokenTab[] =
{
	{ CONST_ID,		(char *)	"PI",				3.1415926,		NULL },
	{ CONST_ID,		(char *)	"E",				2.71828,			NULL },
	{ T,					(char *)	"T",				0.0,					NULL },
	{ FUNC,			(char *)	"SIN",			0.0,					sin },
	{ FUNC,			(char *)	"COS",			0.0,					cos },
	{ FUNC,			(char *)	"TAN",			0.0,					tan },
	{ FUNC,			(char *)	"LN",			0.0,					log },
	{ FUNC,			(char *)	"EXP",			0.0,					exp },
	{ FUNC,			(char *)	"SQRT",		0.0,					sqrt },
	{ ORIGIN,			(char *)	"ORIGIN",		0.0,					NULL },
	{ SCALE,			(char *)	"SCALE",		0.0,					NULL },
	{ ROT,				(char *)	"ROT",			0.0,					NULL },
	{ IS,					(char *)	"IS",				0.0,					NULL },
	{ FOR,				(char *)	"FOR",			0.0,					NULL },
	{ FROM,			(char *)	"FROM",		0.0,					NULL },
	{ TO,					(char *)	"TO",			0.0,					NULL },
	{ STEP,				(char *)	"STEP",			0.0,					NULL },
	{ DRAW,			(char *)	"DRAW",		0.0,					NULL }
};

struct t_state_trans
{
	int src; // 源状态
	char tag;// 边上标记
	int to;  // 目标状态
};

static t_state_trans myTrans[] = { //全部转移
	{ 0, '*', 4 },
{ 0,'-', 7 },
{ 2,'.', 3 },
{}
}; // 基于此,实现
   // 终态的数据结构
struct  t_final_state {
	int  state;  // 终态
	Token_Type kind; // 记号类别
};
// 所有终态
static t_final_state	myFinalStates[] ={
	{ 1, ID },
{ 2, CONST_ID },
{ 3, CONST_ID },
{ 4,POWER },
{ 5,POWER },
{ 13,COMMENT },
{ 8,PLUS },
};

extern unsigned int LineNo;

/*检查绘图语言的原文件是否存在，存在，则打开文件，初始化LineNo的值为1,否则是0*/
extern bool InitScanner(const char *FileName);

/* 关闭源程序文件*/
extern void CloseScanner(void);

/* 每调用一次次函数，就获得一个记号*/
extern Token GetToken(void);

#endif
