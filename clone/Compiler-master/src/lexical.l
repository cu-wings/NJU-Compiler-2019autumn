%option yylineno
%{
#include "syntax_tree.h"	
#define YYSTYPE SyntaxTreeType*
#include "syntax.h"
int lines;
extern int error_state;
int yycolumn = 1;
char errmsg[] = "Mysterious characters \' \'.";
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

#define WORK(token) \
	yylval = (YYSTYPE)malloc(sizeof(SyntaxTreeType)); \
	yylval->name = toStr(#token); \
	yylval->line_no = yylineno; \
	yylval->type = TERMINAL; \
	if(!strcmp(#token, "INT")) yylval->int_val = toInt(yytext); else \
	if(!strcmp(#token, "FLOAT")) yylval->float_val = toFloat(yytext); else \
	yylval->str_val = toStr(yytext); \
	return token;

%}
letter [_a-zA-Z]
digit [0-9]
%%
"//".*$ {/*WORK(NOTE;*/}
"/*" {
	char ch1 = 0, ch2 = input();
	while (ch2 > 0 && !((ch1 == '*')&&(ch2 == '/'))) {
		ch1 = ch2;
		ch2 = input();
	}
//	printf("%s\n", yytext);/*WORK(NOTES;*/
}
(([0-9]+{digit}*)|(0(x|X)({digit}|[a-zA-Z])+)) {WORK(INT)}
(\.|{digit})+(\.|{digit})+([eE][+-]?(\.|{digit})+)? {WORK(FLOAT)}
; {WORK(SEMI)}
, {WORK(COMMA)}
= {WORK(ASSIGNOP)}
>|<|(>=)|(<=)|(==)|(!=) {WORK(RELOP)}
\+ {WORK(PLUS)}
- {WORK(MINUS)}
\* {WORK(STAR)}
\/ {WORK(DIV)}
&& {WORK(AND)}
\|\| {WORK(OR)}
\. {WORK(DOT)}
! {WORK(NOT)}
(int)|(float) {WORK(TYPE)}
\( {WORK(LP)}
\) {WORK(RP)}
"[" {WORK(LB)}
"]" {WORK(RB)}
\{ {WORK(LC)}
\} {WORK(RC)}
struct {WORK(STRUCT)}
return {WORK(RETURN)}
read {WORK(READ)}
write {WORK(WRITE)}
if {WORK(IF)}
else {WORK(ELSE)}
while {WORK(WHILE)}
{letter}({letter}|{digit})* {WORK(ID)}
" "|\t {}
\n {lines ++; yycolumn = 1;}
. {errmsg[23] = yytext[0]; lerror(errmsg);}
%%
void lerror(const char * str) {
	printf("Error type A at Line %d: %s\n", lines, str); 
	error_state = 1;
}

