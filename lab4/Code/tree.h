#ifndef COMPILER_TREE_H_
#define COMPILER_TREE_H_
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

typedef struct treeNode {
	int line,is_terminal;
	char *name;
	union{
		int i_val;
		float f_val;
		char *s_val;
	};
	struct treeNode *child, *next, *father;
}treeNode;

void BuildTree(treeNode **,const char *,int, ...);
void PrintTree(treeNode *,int);
int toInt(const char *);
float toFloat(const char *);
char *toStr(const char *);

#endif
