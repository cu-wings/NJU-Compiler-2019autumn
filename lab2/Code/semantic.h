#ifndef COMPLIER_SEMANTIC_H_
#define COMPLIER_SEMANTIC_H_
#include "tree.h"
#include "syntax.tab.h"
typedef unsigned short bool;
#define true 1
#define false 0
#define MAX_TABLE 0x3fff
#define DEBUG 1
extern treeNode *treeRoot;
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FieldList_ FieldList_;
typedef struct SymbolTable_* SymbolTable;
typedef struct SymbolTable_ SymbolTable_;
typedef struct Type_ Type_;
struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        int basic; //int and float
        struct { Type elem; int size; } array;
        FieldList structure;
        struct { Type ret; FieldList param; } function;
    }u;
};

struct FieldList_
{
    char* name;         //name of field
    Type type;          //type of field
    FieldList tail;     //next field
};

struct SymbolTable_
{
    char* name;
    Type type;
    int line;
    bool defined;
    SymbolTable next;
};

SymbolTable symbolTable[MAX_TABLE];

void initHarshTable();
unsigned int harsh(char* name);
void tranverseTree(treeNode* treeRoot);
void addSymbol(char* name, Type type, int line, bool defined);
void serror(char* msg, int line, int errorType);
SymbolTable newSymbol(char* name, Type type, int line, bool defined);
bool sameType(Type t1, Type t2);
void ExtDefList(treeNode* root);
void ExtDef(treeNode* root);
Type Specifier(treeNode* root);
void ExtDecList(treeNode* root, Type type);
void FunDec(treeNode* root, Type ret, bool defined);
void CompSt(treeNode* root, Type ret, bool defined);
void VarDec(treeNode* root, Type type);
Type StructSpecifier(treeNode* root);
Type getType(char* name);
FieldList Def_Structure(treeNode* root);
FieldList DefList_Structure(treeNode* root);
FieldList DecList_Structure(treeNode* root, Type type);
FieldList Dec_Structure(treeNode* root, Type type);
FieldList VarDec_Structure(treeNode* root, Type type);
char* stradd(char* str1, char* str2);
void CheckFunc();
FieldList VarList(treeNode* root);
FieldList ParamDec(treeNode* root);

#endif