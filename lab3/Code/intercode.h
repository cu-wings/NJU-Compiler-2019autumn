#ifndef COMPLIER_INTERCODE_H_
#define COMPLIER_INTERCODE_H_
#include "semantic.h"
int tempvarNum = 0;     //t1,t2...
int varNum = 0;         //v1,v2...
int lableNum = 0;       //LABLE1,LABLE2...

typedef struct Operand Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;
struct Operand {
    enum { TEMPVAR, CONSTANT, VARIABLE, FUNCNAME, LABLENUM} kind;
    union 
    {
        int val;
        char *funcName;
    }u;
};


struct InterCode
{
    enum { ASSIGN, ADD, SUB, MUL, DIVIDE, ADDR, VALUE, VALTOVAL, 
    LABLE, FUNC, GOTO, IFGOTO, RET, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
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
    InterCode* next;
    InterCode* prev;
};

InterCode* head = NULL;
InterCode* tail = NULL;

void initTranslate();
void translateTree(treeNode* treeRoot);
int new_temp();
int new_lable();
void new_code(int kind, ...);
#endif