#include "tree.h"

int toInt(const char *s){
	if(strcmp(s,"0") == 0)
		return 0;
	long int val = 0;
	if(s[0] == '0')
	{
		if(s[1] == 'x'|| s[1] == 'X')
		{
			val = strtol(s,NULL,16);
		}
		else
		{
			val = strtol(s,NULL,8);
		}
	}
	else
		val = atoi(s);
	return val;
}
float toFloat(const char *s){
	return strtof(s,NULL);
}
char * toStr(const char *s){
	char * tmp = malloc(sizeof(s));
	strcpy(tmp, s);
	return tmp;
}

void BuildTree(treeNode **node, const char *name,int childno, ...){
	*node = (treeNode *)malloc(sizeof(treeNode));
	treeNode *root = *node;
	root->is_terminal = 0; //nonterminal
 
	va_list args;
	int i = 0;
	va_start(args,childno);

	treeNode *child;
	for(i = 0 ; i < childno ; i++){
		child = va_arg(args, treeNode*);
		if(root->child == NULL){
			root->name = toStr(name);
			//printf("%s\n",name);
			root->line = child->line;
			root->child = child;
		}
		else{
			treeNode *temp = root->child;
			while(temp->next != NULL)
				temp = temp->next;
			temp->next = child;
			if(child != NULL)
				child->father = temp;
		}	
	}
	va_end(args);
}

void PrintTree(treeNode *root,int n){
	if(root != NULL){
		int i = 0;
		for(i = 1; i <= n; i++)
			printf("  ");
		printf("%s",root->name);
		if(root->is_terminal == 0)
			printf(" (%d)", root->line);
		else if(!strcmp(root->name, "ID") || !strcmp(root->name,"TYPE"))
			printf(": %s",root->s_val);
		else if(!strcmp(root->name, "INT"))
			printf(": %d",root->i_val);
		else if(!strcmp(root->name, "FLOAT"))
			printf(": %f",root->f_val);
		printf("\n");
		PrintTree(root->child,n+1);
		PrintTree(root->next,n);
	}
}
