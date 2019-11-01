#ifndef COMPILER_INTERCODE_H_
#define COMPILER_INTERCODE_H_

#include "common.h"
#include "symbol.h"

enum {PARAM_CODE, FUNCTION_CODE, ASSIGN_CODE, PLUS_CODE, MINUS_CODE, STAR_CODE, DIV_CODE,
	  RETURN_CODE, LABEL_CODE, GOTO_CODE, READ_CODE, WRITE_CODE, ZEROMINUS_CODE, IF_CODE,
	  ARG_CODE, CALL_CODE, DEC_CODE};
enum {ID_OP, FUNCTION_OP, INT_OP};

typedef struct Operand Operand;
typedef struct InterCode InterCode;

struct Operand {
	char * str_val;
	SymbolType * type;
	ListHead list;
};

struct InterCode {
	int type;
	ListHead list;
	ListHead op_list;
};
char * NewVar();
char * NewLabel();
Operand * NewOperand(char *);
void NewCode(int, int, ...);
char * negOperand(char *);
bool ParamSymbol(char *);
void translate_Program(SyntaxTreeType *);
void translate_ExtDefList(SyntaxTreeType *);
void translate_ExtDecList(SyntaxTreeType *);
void translate_ExtDef(SyntaxTreeType *);
void translate_FunDec(SyntaxTreeType *);
void translate_VarList(SyntaxTreeType *);
void translate_ParamDec(SyntaxTreeType *);
void translate_CompSt(SyntaxTreeType *);
void translate_DefList(SyntaxTreeType *);
void translate_Def(SyntaxTreeType *);
void translate_DecList(SyntaxTreeType *);
void translate_Dec(SyntaxTreeType *);
void translate_VarDec(SyntaxTreeType *);
void translate_StmtList(SyntaxTreeType *);
void translate_Stmt(SyntaxTreeType *);
void translate_Args(SyntaxTreeType *, ListHead *);
Operand * translate_Exp(SyntaxTreeType *);
void translate_Cond(SyntaxTreeType *, Operand *, Operand *);
Operand * CalcArrayOffset(SyntaxTreeType *);
#endif // COMPILER_INTERCODE_H_
