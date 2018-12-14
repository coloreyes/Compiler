#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "funcdraw.h"

typedef double(*FuncPtr)(double);
struct ExprNode				//语法树结点类型
{
	enum Token_Type OpCode;
							//PLUS, MINUS, MUL, DIV, POWER, FUNC, CONST_ID等
	union
	{
		struct { ExprNode *Left, *Right; } CaseOperator;
		struct { ExprNode *Child; FuncPtr MathFuncPtr; } CaseFunc;
		double CaseConst;
		double * CaseParmPtr;
	} Contect;
};

extern void Parser(char * SrcFilePtr);		//语法分析器对外的接口

#endif