#ifndef COMPILER_SYNTAX_TREE_H_
#define COMPILER_SYNTAX_TREE_H_
#include "common.h"

enum {TERMINAL, NONTERMINAL};
typedef struct SyntaxTreeType {
	int line_no, type; 	//type = 0 terminal symbol, type=1 nonterminal symbol
	char * name;
	union {
		int int_val;
		float float_val;
		char * str_val;
	};
	struct SyntaxTreeType *child, *next, *prev;
} SyntaxTreeType;

void BuildTree(SyntaxTreeType **, const char *, int, ...);
void PrintTree(SyntaxTreeType *, int);
char * toStr(const char *);
int toInt(const char *);
float toFloat(const char *);
int toupper(int);
void lerror(const char *);
int fileno(FILE *);
int yylex();
int yyerror(const char *);

#endif // COMPILER_SYNTAX_TREE_H_
