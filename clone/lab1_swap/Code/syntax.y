%locations
%{
#include <stdio.h>
#include "tree.h"
#include "lex.yy.c"
//int yylex();
int errorState = 0;
treeNode *treeRoot;
#define YYSTYPE treeNode*
int  yyerror(const char *msg);
void yyrestart(FILE *yyin);
%}
%token INT FLOAT ID
%token SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

%nonassoc MISSING
%nonassoc SEMI
%nonassoc LOWER_THAN_ALL
%right ASSIGNOP
%left OR 
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT
%nonassoc COMMA LC RC TYPE STRUCT INT FLOAT RETURN IF WHILE ID
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%nonassoc EMPTY
%%
Program : ExtDefList { 
	BuildTree(&$$, "Program", 1, $1);
	treeRoot = $$; 
}
;
ExtDefList : ExtDef ExtDefList {BuildTree(&$$, "ExtDefList", 2, $1, $2);}
| /*%prec EMPTY*/ {$$ = NULL;}
;
ExtDef : Specifier ExtDecList SEMI {BuildTree(&$$, "ExtDef", 3, $1, $2, $3);}
| Specifier SEMI {BuildTree(&$$, "ExtDef", 2, $1, $2);}
| Specifier FunDec CompSt {BuildTree(&$$, "ExtDef", 3, $1, $2, $3);}
| Specifier ExtDecList %prec MISSING {yyerror("Specifier ExtDecList Missing ';'");}
| Specifier %prec MISSING {yyerror("Specifier Missing ';'");}
| error SEMI
;
ExtDecList : VarDec {BuildTree(&$$, "ExtDecList", 1, $1);}
| VarDec COMMA ExtDecList {BuildTree(&$$, "ExtDecList", 3, $1, $2, $3);}
;

Specifier : TYPE {BuildTree(&$$, "Specifier", 1, $1);}
| StructSpecifier {BuildTree(&$$, "Specifier", 1, $1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC {BuildTree(&$$, "StructSpecifier", 5, $1, $2, $3, $4, $5);}
| STRUCT Tag {BuildTree(&$$, "StructSpecifier", 2, $1, $2);}
;
OptTag : ID {BuildTree(&$$, "OptTag", 1, $1);}
| /*%prec EMPTY*/ {$$ = NULL;}
;
Tag : ID {BuildTree(&$$, "Tag", 1, $1);}
;

VarDec : ID {BuildTree(&$$, "VarDec", 1, $1);}
| VarDec LB INT RB {BuildTree(&$$, "VarDec", 4, $1, $2, $3, $4);}
| VarDec LB INT %prec MISSING {yyerror("VarDec LB INT Missing ']'");}
;
FunDec : ID LP VarList RP {BuildTree(&$$, "FunDec", 4, $1, $2, $3, $4);}
| ID LP RP {BuildTree(&$$, "FunDec", 3, $1, $2, $3);}
;
VarList : ParamDec COMMA VarList  {BuildTree(&$$, "VarList", 3, $1, $2, $3);}
| ParamDec {BuildTree(&$$, "VarList", 1, $1);}
;
ParamDec : Specifier VarDec {BuildTree(&$$, "ParamDec", 2, $1, $2);}
;

CompSt : LC DefList StmtList RC {BuildTree(&$$, "CompSt", 4, $1, $2, $3, $4);}
| LC DefList StmtList %prec MISSING {yyerror("LC DefList StmtList Missing '}'");}
| error RC /*{yyerror("Missing '}'");}*/
;
StmtList : Stmt StmtList {BuildTree(&$$, "StmtList", 2, $1, $2);}
| /*%prec EMPTY*/ {$$ = NULL;}
;
Stmt : Exp SEMI {BuildTree(&$$, "Stmt", 2, $1, $2);}
| Exp %prec MISSING {yyerror("Exp Missing ';'");}
| CompSt {BuildTree(&$$, "Stmt", 1, $1);}
| RETURN Exp SEMI {BuildTree(&$$, "Stmt", 3, $1, $2, $3);}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {BuildTree(&$$, "Stmt", 5, $1, $2, $3, $4, $5);}
| IF LP Exp RP Stmt ELSE Stmt {BuildTree(&$$, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
| WHILE LP Exp RP Stmt {BuildTree(&$$, "Stmt", 5, $1, $2, $3, $4, $5);}
| error SEMI
;

DefList : Def DefList {BuildTree(&$$, "DefList", 2, $1, $2);}
| /*%prec EMPTY*/ {$$ = NULL;}
;
Def : Specifier DecList SEMI {BuildTree(&$$, "Def", 3, $1, $2, $3);}
| Specifier DecList %prec MISSING {yyerror("Missing ';'");}
| error SEMI
;
DecList : Dec {BuildTree(&$$, "DecList", 1, $1);}
| Dec COMMA DecList {BuildTree(&$$, "DecList", 3, $1, $2, $3);}
;
Dec : VarDec {BuildTree(&$$, "Dec", 1, $1);}
| VarDec ASSIGNOP Exp {BuildTree(&$$, "Dec", 3, $1, $2, $3);}
;

Exp : Exp ASSIGNOP Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp AND Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp OR Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp RELOP Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp PLUS Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp MINUS Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp STAR Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| Exp DIV Exp {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| LP Exp RP {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| LP Exp %prec MISSING {yyerror("LP Exp Missing ')'");}
| MINUS Exp {BuildTree(&$$, "Exp", 2, $1, $2);}
| NOT Exp {BuildTree(&$$, "Exp", 2, $1, $2);}
| ID LP Args RP {BuildTree(&$$, "Exp", 4, $1, $2, $3, $4);}
| ID LP Args %prec MISSING {yyerror("ID LP Args Missing ')'");}
| ID LP RP {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| ID LP %prec MISSING {yyerror("ID LP Missing ')'");}
| Exp LB Exp RB {BuildTree(&$$, "Exp", 4, $1, $2, $3, $4);}
| Exp LB Exp %prec MISSING {yyerror("Exp LB Exp Missing ']'");}
| Exp DOT ID {BuildTree(&$$, "Exp", 3, $1, $2, $3);}
| ID %prec LOWER_THAN_ALL {BuildTree(&$$, "Exp", 1, $1);}
| INT {BuildTree(&$$, "Exp", 1, $1);}
| FLOAT {BuildTree(&$$, "Exp", 1, $1);}
| error RP
;
Args : Exp COMMA Args {BuildTree(&$$, "Args", 3, $1, $2, $3);}
| Exp %prec LOWER_THAN_ALL {BuildTree(&$$, "Args", 1, $1);}
;
%%
//#include "lex.yy.c"
int yyerror(const char* msg) {
	errorState = 1;
	printf("Error type B at Line %d: %s.\n", yylloc.first_line, msg);
	return 0;
}
