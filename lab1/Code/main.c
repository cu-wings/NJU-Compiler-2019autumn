#include <stdio.h>
#include "syntax.tab.h"
extern FILE* yyin;
extern int yylex();
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
	return 0;
}
