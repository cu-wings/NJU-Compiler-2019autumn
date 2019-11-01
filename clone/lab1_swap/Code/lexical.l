%option yylineno
%{
#define YYSTYPE treeNode* 
#include "syntax.tab.h"
#include "tree.h"
#define PRINT(token)\
	printf(#token);\
	printf(" %s\n",yytext);

//#define YYSTYPE treeNode*
#define RETURN_TOKEN(token)\
	yylval = (YYSTYPE)malloc(sizeof(treeNode)); \
	yylval->name = toStr(#token); \
	yylval->line = yylineno; \
	yylval->is_terminal = 1; \
	if(!strcmp(#token, "INT")) yylval->i_val = toInt(yytext); else \
	if(!strcmp(#token, "FLOAT")) yylval->f_val = toFloat(yytext); else \
	yylval->s_val = toStr(yytext); \
	return token;
//extern YYLTYPE yylloc;
int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
extern int errorState;
%}
letter [_a-zA-Z]
digit [0-9]
hexhead "0x"|"0X"
hexbody [0-9a-fA-F]
octhead 0
octbody [0-7]
dechead [1-9]
decbody [0-9]
nonhexbody [g-zG-Z]
nonoctbody [8-9]
nondecbody [_a-zA-Z]
dot "."
minus "-"

%%
"int"|"float" {/*PRINT(TYPE)*/ RETURN_TOKEN(TYPE)}
"struct" {/*PRINT(STRUCT)*/ RETURN_TOKEN(STRUCT)}
"return" {/*PRINT(RETURN)*/ RETURN_TOKEN(RETURN)}
"if"	{/*PRINT(IF)*/ RETURN_TOKEN(IF)}
"else"	{/*PRINT(ELSE)*/ RETURN_TOKEN(ELSE)}
"while"	{/*PRINT(WHILE)*/ RETURN_TOKEN(WHILE)}
{digit}+{dot}{digit}+ {/*PRINT(FLOAT)*/ RETURN_TOKEN(FLOAT)}
{letter}+({digit}|{letter})* {
	//PRINT(ID)
	RETURN_TOKEN(ID)
	  }
{hexhead}{hexbody}*{nonhexbody}+{hexbody}* {
	errorState = 1;
	printf("Error type A at Line %d: Illegal hex number \'%s\'\n",
			yylineno, yytext);
					  }
{octhead}{octbody}*{nonoctbody}+{octbody}* {
	errorState = 1;
	printf("Error type A at Line %d: Illegal oct number \'%s\'\n",
			yylineno, yytext);
					   }
{dechead}{decbody}*{nondecbody}+{decbody}* {
	errorState = 1;    
	printf("Error type A at Line %d: Illegal dec number \'%s\'\n",    	
	       		yylineno, yytext);                                            
                                           }
{hexhead}{hexbody}*|{dechead}{decbody}*|{octhead}{octbody}* { 
	//PRINT(INT) 
	RETURN_TOKEN(INT) 
	 }
";" {RETURN_TOKEN(SEMI)}
"," {RETURN_TOKEN(COMMA)}
"=" {/*PRINT(ASSIGNOP)*/ RETURN_TOKEN(ASSIGNOP)}
>=|<=|==|!=|>|< {/*PRINT(RELOP)*/ RETURN_TOKEN(RELOP)}
"+" { /*printf("PLUS\n");*/ RETURN_TOKEN(PLUS)}
"-" { /*printf("MINUS\n");*/  RETURN_TOKEN(MINUS)}
"*" { /*printf("STAR\n");*/  RETURN_TOKEN(STAR)}
"/" { /*printf("DIV\n");*/  RETURN_TOKEN(DIV)}
"&&" {/*PRINT(AND)*/ RETURN_TOKEN(AND)}
"||" {/*PRINT(OR)*/ RETURN_TOKEN(OR)}
"." {/*PRINT(DOT)*/ RETURN_TOKEN(DOT)}
"!" {/*PRINT(NOT)*/ RETURN_TOKEN(NOT)}
"(" { RETURN_TOKEN(LP)}
")" { RETURN_TOKEN(RP)}
"[" { RETURN_TOKEN(LB)}
"]" { RETURN_TOKEN(RB)}
"{" {/*PRINT(LC)*/ RETURN_TOKEN(LC)}
"}" {/*PRINT(RC)*/ RETURN_TOKEN(RC) }

(\t)+ {}
[" "]+ {}
[\n\r] {yycolumn = 1;}
. {
	errorState = 1;
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
			yylineno, yytext);
}
%%

