#ifndef COMPLIER_ASMCODE_H_
#define COMPLIER_ASMCODE_H_
#include "intercode.h"
#define ASMDEBUG 0
#define REGNUM 32
typedef struct BasicBlock BasicBlock;
typedef struct FunctionBlock FunctionBlock;
typedef struct RegDescripter RegDescripter;
typedef struct VarDescripter VarDescripter;
typedef struct VarInBlock VarInBlock;
typedef struct UseLine UseLine;

struct RegDescripter {
    Operand* op;
};

struct UseLine {
    int line;
    UseLine* next;
};

struct VarDescripter {
    Operand op;
    bool* reg;
    int offset; // the offset of $fp
};

struct VarInBlock {
    VarDescripter *var;
    UseLine* useLine;
    VarInBlock* next;
};

struct BasicBlock {
    VarInBlock* vars;
    InterCodes* head;
    InterCodes* tail;
    BasicBlock *next;
};

struct FunctionBlock {
    VarInBlock* varList;
    int offset;
    BasicBlock* blockList;
    FunctionBlock* next;
};

bool regs[REGNUM];
bool *v0;// = &regs[2];
bool *v1;
bool *a0;// = &regs[3];
bool *a1;
bool *a2;
bool *a3;
bool *t0;
bool *t8;
bool *t9;


FunctionBlock* functions;
VarDescripter* varList;
BasicBlock *firstBlock;

void initBlock();
void initRegs();
void translateAsm(char* outputName);
char* getReg(Operand op, BasicBlock* block, FILE* fp, int line, int varPlace);
void saveAll(BasicBlock* block, FILE* fp);
char* regChar(bool* reg);
int regInt(char* reg);
void testOut(InterCodes* temp,FILE* fp);

#endif