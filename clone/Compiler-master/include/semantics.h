#ifndef _COMPLIER_SEMANTICS_H_
#define _COMPILER_SEMANTICS_H_

#include "syntax_tree.h"
#include "symbol.h"

void serror(int, int, const char *);
void Program(SyntaxTreeType *);
void ExtDefList(SyntaxTreeType *);
void ExtDef(SyntaxTreeType *);
SymbolType * Specifier(SyntaxTreeType *);
SymbolType * StructSpecifier(SyntaxTreeType *);
void ExtDecList(SyntaxTreeType *, SymbolType *);
void VarDec(SyntaxTreeType *, SymbolType *, int);
void FunDec(SyntaxTreeType *, SymbolType *, int);
void CompSt(SyntaxTreeType *, SymbolType *, int);
void DefList(SyntaxTreeType *, int);
void Def(SyntaxTreeType *, int);
void DecList(SyntaxTreeType *, SymbolType *, int);
void StmtList(SyntaxTreeType *, SymbolType *, int);
void Stmt(SyntaxTreeType *, SymbolType *, int);
SymbolType * Exp(SyntaxTreeType *);
void Args(SyntaxTreeType *, SymbolType *);
#endif // _COMPILER_SEMANTICS_H_
