#include<iostream>
#include<stdio.h>
#include"stdafx.h"
#include"scanner.h"
using namespace std;

#define TOKEN_LEN 100				// 记号最大长度
unsigned int LineNo;				// 跟踪源文件行号
static char TokenBuffer[TOKEN_LEN];		// 记号字符缓冲
static char *BufferFollow;
static FILE *in_file;
char pre_process(struct Token * pToken);
int scan_move(struct Token*pToken, char first_char);
int post_process(struct Token *pToken, int last_state);

//初始化编译器 打开文件 进度：OK
bool InitScanner(const char *FileName) {
	fopen_s(&in_file, FileName, "r");
	if (in_file != 0) {
		return true;
	}
	else {
		return false;
	}
}

//识别一个记号 进度：？
extern Token GetToken() {
	int first_char;
	int last_state = -1;
	struct Token theToken;//被识别到的记号对象
	int to_be_continue;
	do
	{
		//预处理，跳过空白字符
		first_char = pre_process(&theToken);
		if (first_char == -1)
		{
			theToken.type = NONTOKEN;
			return theToken;
		}
		//边扫描输入，边转移状态
		last_state = scan_move(&theToken, first_char);
		//后处理，根据终态所标记的记号种类信息，进行处理
		to_be_continue = post_process(&theToken, last_state);

	} while (to_be_continue != 0);
	return theToken;
}

//获得初态 进度：？
int get_start_state() {
	return 0;
}

// 执行状态转移，返回下一状态。若无转移则返回 -1. 进度：OK
int move(int state_src) {
	for (int i = 0; i<sizeof(myTrans); i++) {
		if (state_src == myTrans[i].src && strcmp(BufferFollow,myTrans[i].tag)) {
			return myTrans[i].to;
		}
	}
	return -1;
}

//终态判断。若为终态，则返回相应的记号类别，否则返回 ERRTOKEN 进度：？
Token_Type state_is_final(int state) {
	for (int i = 0; i<sizeof(myFinalStates); i++) {
		if (state == myFinalStates[i].state) {
			return myFinalStates[i].kind;
		}
	}
	return ERRTOKEN;
};

//追加文本 进度：？
void AppendTokenTxt(struct Token *pToken, int current_char) {
	pToken->lexeme[strlen(pToken->lexeme)] = current_char;
}

//转换成大写 进度：OK
char topper(char next_char) {
	if (next_char > 96 && next_char < 123) {
		next_char = next_char - 32;
	}
	printf("%c", next_char);
	return next_char;
}

//从源文件中读取一个字符 进度：OK
char GetChar(void) {
	//从源文件中读到一个字符，并返回它
	//遇到文件结束，则返回-1
	int next_char = getc(in_file);
	if (EOF == next_char)
		return -1;
	else
		//统一返回大写
		return topper(next_char);
}

//退回一个字符 进度：OK
void BackChar(char c) {
	if (c == EOF || c != '\n') {
		return;
	}
	else {
		ungetc(c, in_file);
	}
}

//跳过空格 进度：OK
char pre_process(struct Token * pToken) {
	char current_char;//当前读到字符
	memset(pToken, 0, sizeof(struct Token));//记号内存清零
	for (;;) {
		current_char = GetChar();
		if (current_char == -1) {
			return -1;
		}
		if (!isspace(current_char))
			break;
	}
	// 此时，current_char 就是记号的第一个字符
	return current_char;
}


//扫描输入字符，并进行状态转移，返回终态 进度：？
int scan_move(struct Token*pToken, char first_char)
{
	int current_state, next_state;//当前状态，下一状态
	char current_char = first_char;
	current_state = get_start_state();
	
	for (;;) {
		if (isdigit(current_char)) {
			BufferFollow = (char *)"digit";
		}
		else if (isalpha(current_char)) {
			BufferFollow = (char *)"letter";
		}
		else BufferFollow = (char *)current_char;
		next_state = move(current_state);
		if (next_state<0) {//没有转移了
			BackChar(current_char);//退回当前字符
			break;
		}
		AppendTokenTxt(pToken, current_char);//追加记号文本
		current_state = next_state;
		current_char = GetChar();
		if (current_char == -1) {//文件结束
			break;
		}
		return current_state;
	}

}
/*根据识别出的字符串在符号表中查找
若找到返回该记号
否则返回错误记号
*/
//进度：？
struct Token JudgeKeyToken(const char*c_str) {
	int count;
	struct Token err_token;
	int n = sizeof(TokenTab) / sizeof(TokenTab[0]);
	for (count = 0; count<n; ++count) {
		if (strcmp((TokenTab[count]).lexeme, c_str) == 0) {
			//printf("%f", TokenTab[count].value);
			return  TokenTab[count];
		}
			
	}
	memset(&err_token, 0, sizeof(err_token));
	err_token.type = ERRTOKEN;
	return err_token;
};

//终态判断及收尾 进度：？
int post_process(struct Token *pToken, int last_state) {
	int to_be_continue = 0;
	enum Token_Type tk; //终态对应的记号
	pToken->type = tk = state_is_final(last_state);
	switch (tk) {
	case ID://查找符号表，进一步计算记号信息
		*pToken = JudgeKeyToken(pToken->lexeme);
		break;
	case CONST_ID:
		pToken->value = atof(pToken->lexeme);//转为数值
	case COMMENT:
	{
		int c;
		while (1) {
			c = GetChar();//行注释，忽略到行尾
			if (c == '\n' || c == -1)
				break;
		}

	}
	to_be_continue = 1;
	break;
	}
	return to_be_continue;
}


//关闭编译器（文件） 进度：OK
void CloseScanner(void) {
	if (in_file != 0) {
		fclose(in_file);
	}
}


//清除缓存区 进度：OK
static void EmptyTokenString(void) {
	memset(TokenBuffer, 0, TOKEN_LEN);
}
