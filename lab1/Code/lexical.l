%option yylineno
%{
#include "syntax.tab.h"
#define PRINT(token)\
	printf(#token);\
	printf(" %s\n",yytext);
#define RETURN_TOKEN(token)\
	return token;
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
"int"|"float" {PRINT(TYPE) RETURN_TOKEN(TYPE)}
"struct" {PRINT(STRUCT) RETURN_TOKEN(STRUCT)}
"return" {PRINT(RETURN) RETURN_TOKEN(RETURN)}
"if"	{PRINT(IF) RETURN_TOKEN(IF)}
"else"	{PRINT(ELSE) RETURN_TOKEN(ELSE)}
"while"	{PRINT(WHILE) RETURN_TOKEN(WHILE)}
{digit}+{dot}{digit}+ {PRINT(FLOAT) RETURN_TOKEN(FLOAT)}
{letter}+({digit}|{letter})* {
	PRINT(ID)
	RETURN_TOKEN(ID)
	  }
{hexhead}{hexbody}*{nonhexbody}+{hexbody}* {
	printf("Error type A at Line %d: Illegal hex number \'%s\'\n",
			yylineno, yytext);
					  }
{octhead}{octbody}*{nonoctbody}+{octbody}* {
	printf("Error type A at Line %d: Illegal oct number \'%s\'\n",
			yylineno, yytext);
					   }
{dechead}{decbody}*{nondecbody}+{decbody}* {    
	printf("Error type A at Line %d: Illegal dec number \'%s\'\n",    	
	       		yylineno, yytext);                                            
                                           }
{hexhead}{hexbody}*|{dechead}{decbody}*|{octhead}{octbody}* { 
	PRINT(INT) 
	RETURN_TOKEN(INT) 
	 }
";" {RETURN_TOKEN(SEMI)}
"," {RETURN_TOKEN(COMMA)}
"=" {PRINT(ASSIGNOP) RETURN_TOKEN(ASSIGNOP)}
>=|<=|==|!=|>|< {PRINT(RELOP) RETURN_TOKEN(RELOP)}
"+" { printf("PLUS\n"); RETURN_TOKEN(PLUS)}
"-" { printf("MINUS\n");  RETURN_TOKEN(MINUS)}
"*" { printf("STAR\n");  RETURN_TOKEN(STAR)}
"/" { printf("DIV\n");  RETURN_TOKEN(DIV)}
"&&" {PRINT(AND) RETURN_TOKEN(AND)}
"||" {PRINT(OR) RETURN_TOKEN(OR)}
"." {PRINT(DOT) RETURN_TOKEN(DOT)}
"!" {PRINT(NOT) RETURN_TOKEN(NOT)}
"(" { RETURN_TOKEN(LP)}
")" { RETURN_TOKEN(RP)}
"[" { RETURN_TOKEN(LB)}
"]" { RETURN_TOKEN(RB)}
"{" { RETURN_TOKEN(LC)}
"}" { RETURN_TOKEN(RC) }

(\t)+ {}
[" "]+ {}
[\n\r] {}
. {
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
			yylineno, yytext);
}
%%

