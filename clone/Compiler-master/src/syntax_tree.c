#include <stdio.h>
#include "syntax_tree.h"

char * toStr(const char * str) {
	char * tmp = malloc(sizeof(str));
	strcpy(tmp, str);
	return tmp;
}

int toInt(const char * str) {
	int i;
	int val = 0;
	if(!strcmp(str, "0")) return 0;
	if(str[0] == '0') {
		if(str[1] == 'x' || str[1] == 'X') {
			for(i = 2; i < strlen(str); ++ i) {
				if(str[i] >= '0' && str[i] <= '9') {
					val = val * 16 + str[i] - '0';
				} else if(toupper(str[i]) >= 'A' && toupper(str[i]) <= 'F') {
					val = val * 16 + toupper(str[i]) - 'A' + 10;
				} else {
					lerror("Illegal hexadecimal number");
					return 0;
				}
			}
		} else {
			for(i = 1; i < strlen(str); ++ i) {
				if(str[i] >= '0' && str[i] <= '7') {
					val = val * 8 + str[i] - '0';
				} else {
					lerror("Illegal octal number");
					return 0;
				}
			}
		}
	} else val = atoi(str);
	return val;
}

float toFloat(const char * str) {
	int i = -1;
	float val = 0;
	while(++ i < strlen(str)) {
		if(str[i] == '.') {
			if(i == 0) {
				lerror("Illegal float number");
				return 0;
			}
			float div = 10.0;
			while(++i < strlen(str)) {
				if(toupper(str[i]) == 'E') {
					int e = 0;
					int flag = 0;
					if(str[i+1] == '-') {
						flag = 1;
						i ++;
					}
					while(++ i < strlen(str)) {
						if(str[i] >= '0' && str[i] <= '9')
							e = e * 10 + str[i] - '0';
						else {
							lerror("Illegal float number");
							return 0;
						}
					}
					while(e --) if(flag) val /= 10;else val *= 10;
				} else {
					if(str[i] >= '0' && str[i] <= '9') {
						val = val + (float)(str[i] - '0') / div;
						div *= 10;
					} else {
						lerror("Illegal float number");
						return 0;
					}
				}
			}
		} else {
			if(str[i] >= '0' && str[i] <= '9')
				val = val * 10 + str[i] - '0';
			else {
				lerror("Illegal float number");
				return 0;
			}
		}
	}
	return val;
}

void BuildTree(SyntaxTreeType ** node, const char * name, int carg, ...) {
	*node = (SyntaxTreeType *)malloc(sizeof(SyntaxTreeType));
	SyntaxTreeType * root = *node; 
	va_list ap;
	va_start(ap, carg);
	int i;
	SyntaxTreeType * child;
	root->type = NONTERMINAL;
	for(i = 0; i < carg; ++ i) {
		child = va_arg(ap, SyntaxTreeType *);
		if(root->child == NULL) {
			root->name = toStr(name);
			root->line_no = child->line_no;
			root->child = child;
		} else {
			SyntaxTreeType * tmp = root->child;
			while(tmp->next != NULL) tmp = tmp->next;
			tmp->next = child;
			if(child != NULL)child->prev = tmp;
		}
	}
	va_end(ap);
}

void PrintTree(SyntaxTreeType * root, int k) {
	if(root == NULL) return;
	int i;
	for(i = 1; i <= k; ++ i) printf("  ");
	printf("%s", root->name);
	if(root->type == NONTERMINAL) printf(" (%d)", root->line_no); else 
	if(!strcmp(root->name, "ID") || !strcmp(root->name, "TYPE")) printf(": %s", root->str_val); else
	if(!strcmp(root->name, "INT")) printf(": %d", root->int_val); else 
	if(!strcmp(root->name, "FLOAT")) printf(": %f", root->float_val);
	printf("\n");
	PrintTree(root->child, k+1);
	PrintTree(root->next, k);
}
