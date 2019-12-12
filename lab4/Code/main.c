#include "tree.h"
#include "syntax.tab.h"
#include "semantic.h"
#include "intercode.h"
extern FILE* yyin;
//extern int yylex();
extern int errorState;
extern void yyrestart(FILE* yyin);
extern treeNode *treeRoot;
int main(int argc, char** argv)
{
	if(argc <= 1) return 1;
	/*if(argc > 1){
		if(!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
	}
	while(yylex() != 0);
	//yylex();
	*/
	FILE* f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if(!errorState)
	{
		if(DEBUG) PrintTree(treeRoot,0);
		tranverseTree(treeRoot);
		if(!serrorState)
			translateTree(treeRoot);
	}
	return 0;
}
