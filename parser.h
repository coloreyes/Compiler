#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "funcdraw.h"

typedef double(*FuncPtr)(double);
struct ExprNode				//�﷨���������
{
	enum Token_Type OpCode;
							//PLUS, MINUS, MUL, DIV, POWER, FUNC, CONST_ID��
	union
	{
		struct { ExprNode *Left, *Right; } CaseOperator;
		struct { ExprNode *Child; FuncPtr MathFuncPtr; } CaseFunc;
		double CaseConst;
		double * CaseParmPtr;
	} Contect;
};

extern void Parser(char * SrcFilePtr);		//�﷨����������Ľӿ�

#endif