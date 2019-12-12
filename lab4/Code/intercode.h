#ifndef COMPLIER_INTERCODE_H_
#define COMPLIER_INTERCODE_H_
#include "semantic.h"
#define IRDEBUG 0
int tempvarNum;     //t1,t2...
int varNum;         //v1,v2...
int labelNum;       //LABEL1,LABEL2...

typedef struct Operand Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;
typedef struct Operands Operands;
struct Operand {
    enum { TEMPVAR, CONSTANT, VARIABLE, FUNCNAME, LABELNUM } kind;
    enum { VAL, ADDRESS } type;
    union 
    {
        struct { int no; char* name; } var; //global var or its address
        int no;     //No. of tempvar, lable
        int value;  //const value
        char *funcName;
    }u;
};


struct InterCode
{
    enum { ASSIGN, ADD, SUB, MUL, DIVIDE, ADDR, VALUE, VALTOVAL, 
    LABEL, FUNC, GOTO, IFGOTO, RET, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
    union
    {
        struct { Operand left, right; } twoOp;
        struct { Operand op1, op2, result; } threeOp;
        struct { Operand op; } oneOp;
        struct { Operand op1; char* relop; Operand op2, op3;} ifGoto; 
    }u;
};

struct InterCodes
{
    InterCode code;
    InterCodes* next;
    InterCodes* prev;
};

struct Operands
{
    Operand op;
    Operands* next;
    Operands* prev;
};

InterCodes* codeHead;
InterCodes* codeTail;
Operands* opListHead;
Operands* opListTail;
InterCodes* expList;
int irerror;

void initTranslate();
void translateTree(treeNode* treeRoot);
Operand new_op(int kind, int type, ...);
void new_code(int kind, ...);
void addOpList(Operand op);
Operand* findVarOp(char* name);
int getTypeSize(Type type);
void addExpList(InterCode code);
void outputFile();
void printOp(Operand op, FILE* fp);

void translateExtDefList(treeNode* root);
void translateExtDef(treeNode* root);
void translateExtDecList(treeNode* root);
void translateFunDec(treeNode* root);
void translateCompSt(treeNode* root);
Operand translateVarDec(treeNode* root);
void translateParamDec(treeNode* root);
void translateVarDecParam(treeNode* root);
void  translateVarList(treeNode* root);
void translateDefList(treeNode* root);
void translateDef(treeNode* root);
void translateDec(treeNode* root);
void translateDecList(treeNode* root);
void translateStmtList(treeNode *root);
void translateArgs(treeNode *root, FieldList field);

//2
Operand translateExp(treeNode *root);
void translateStmt(treeNode *root);
void translateCond(treeNode *root, Operand *true_label, Operand *false_label);
void translateCompSt(treeNode* root);
char *Inverse_operator(char *s);

#endif