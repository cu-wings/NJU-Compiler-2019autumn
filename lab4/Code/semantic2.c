#include "semantic.h"

Type FindStructureFiled(Type type, char * name, int line)
{
	if(type->kind != STRUCTVAR) {
		serror("Use '.' for unstructured variables", line, 13);
		return NULL;
	}
	FieldList temp = type->u.structure->type->u.structure;
	while(temp != NULL)
	{
		if(!strcmp(temp->name, name)) return temp->type;           
		temp = temp->tail;
	}
	serror(stradd("Use undefined domain in structure ", name), line, 14);
	return NULL;
}

bool isRightValuesOnly(treeNode* root)
{
	if(SEDEBUG) printf("isRightValueOnly\n");
	if(!strcmp(root->name, "ID") && root->next == NULL) //ID
		return false;
	if(root != NULL && root->next != NULL && root->next->next != NULL)
	{
		if(root->next->next->next == NULL)
		{
			if(!strcmp(root->next->name, "DOT") && !strcmp(root->next->next->name, "ID")) //Exp DOT ID
				return false;
		}
		else
		{
			if(root->next->next->next->next == NULL && !strcmp(root->next->name, "LB") && !strcmp(root->next->next->next->name, "RB")) //Exp LB Exp RB
				return false;
		}
	}
	return true;
}

void ExtDecList(treeNode* root, Type type)
{
	if(SEDEBUG) printf("ExtDecList\n");
	if(root)
	{
		if(root->child)
		{ 	
			if(root->child->next) //ExtDecList -> VarDec COMMA ExtDecList
			{
				VarDec(root->child, type, true); 
				ExtDecList(root->child->next->next,type);
			}
			else //ExtDecList -> VarDec
			{ 
				VarDec(root->child, type, true);
			}
		}
    }	
}


void VarDec(treeNode* root, Type type, bool defined)
{
	if(SEDEBUG) printf("VarDec\n");
	if(root)
    	{
		if(root->child)
		{
			if(!strcmp(root->child->name, "ID")) //VarDec -> ID
			{
				addSymbol(root->child->s_val, type, root->child->line, defined);
			}
			else  //VarDec -> VarDec LP INT RP
			{	
				Type temp = (Type)malloc(sizeof(Type_));
				temp->kind = ARRAY;
				temp->u.array.elem = type;   //???
				temp->u.array.size = root->child->next->next->i_val;
				VarDec(root->child, temp, defined);
			}
   		}
	}
}	

void CompSt(treeNode* root, Type ret, bool defined) 
{	 
	if(SEDEBUG) printf("CompSt\n");
	if(root)
    {
		if(!strcmp(root->child->next->name, "DefList")) //CompSt -> LC DefList StmtList RC
		{
			DefList(root->child->next, defined);
			if(!strcmp(root->child->next->next->name, "StmtList"))
				StmtList(root->child->next->next, ret);
		}
		else if(!strcmp(root->child->next->name, "StmtList"))
				StmtList(root->child->next, ret);
		
	}
}


void DefList(treeNode* root, bool defined)
{
	if(SEDEBUG) printf("DefList\n");
	if(root)
    	{
		if(root->child) // DefList-> Def DefList
		{
			Def(root->child, defined);
			DefList(root->child->next, defined);
		}
		else
		{
			// DefList-> empty
		}
	}
}

void Def(treeNode* root, bool defined)
{
	if(SEDEBUG) printf("Def\n");
	if(root)
    	{
		if(root->child) //Def -> Specifier DecList SEMI
		{
			Type type = Specifier(root->child);
			if(type == NULL) 
				return;
			DecList(root->child->next, type, defined);
		}
	}
}

void DecList(treeNode *root, Type type,  bool defined) {
	if(SEDEBUG) printf("DecList\n");
	if(root)
    	{
		if(root->child) 
		{
			if(root->child->next == NULL) //DecList -> Dec
			{
				Dec(root->child, type, defined);
			}
			else   //DecList -> Dec COMMA DecList
			{
				Dec(root->child, type, defined);
				DecList(root->child->next->next, type, defined);
			}
		}
	}
}

void Dec(treeNode *root, Type type,  bool defined) {
	if(SEDEBUG) printf("Dec\n");
	if(root)
    	{
		if(root->child) 
		{
			if(root->child->next == NULL)
			{
				VarDec(root->child, type, defined);
			}
			else   //DecList -> Dec COMMA DecList
			{
				VarDec(root->child, type, defined);
				Exp(root->child->next->next);
			}
		}
	}
}

void StmtList(treeNode* root, Type ret)
{
	if(SEDEBUG) printf("StmtList\n");
	if(root)
    {
		if(root->child) 
		{
			if(!strcmp(root->child->name, "Stmt")) //StmtList -> Stmt StmtList
			{
				Stmt(root->child, ret);
				StmtList(root->child->next, ret);
			}
			else{
			//StmtList -> empty
			}
		}
	}
}

void Stmt(treeNode* root, Type ret)
{
	if(SEDEBUG) printf("Stmt\n");
	if(root)
    	{
		if(root->child)
		{
			if(!strcmp(root->child->name, "Exp")) //Stmt -> Exp SEMI
			{
				Exp(root->child);
			}
			else if(!strcmp(root->child->name, "CompSt")) //Stmt -> CompSt
			{
				CompSt(root->child, ret, true);//????????????
			}
			else if(!strcmp(root->child->name, "RETURN")) //Stmt -> RETURN Exp SEMI
			{
				Type type = Exp(root->child->next);
				if(!sameType(type, ret))
				{
					serror("Type mismatched for return", root->child->next->line, 8);
				}
			}
			else if(!strcmp(root->child->name, "IF") || !strcmp(root->child->name, "WHILE")) //Stmt -> IF LP Exp RP Stmt | IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt
			{
				Exp(root->child->next->next);
				Stmt(root->child->next->next->next->next, ret);
				if(root->child->next->next->next->next->next)  //Stmt ->IF LP Exp RP Stmt ELSE Stmt 
				{
					Stmt(root->child->next->next->next->next->next->next, ret);
				}
			}
		}
	}
}

Type Exp(treeNode* root)
{
	if(SEDEBUG) printf("Exp\n");
	if(root)
    	{
		if(root->child)
		{
			Type type1 = (Type)malloc(sizeof(Type_));
			Type type2 = (Type)malloc(sizeof(Type_));
			if(!strcmp(root->child->name, "Exp")) //Exp -> Exp ASSIGNOP Exp | Exp AND Exp | Exp OR Exp | Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp | Exp STAR Exp | Exp DIV Exp | Exp LB Exp RB | Exp DOT ID
			{
				type1 = Exp(root->child);
				if(type1 == NULL)
					return NULL;
				if(!strcmp(root->child->next->next->name, "Exp"))
				{	
					type2 = Exp(root->child->next->next);
					if(type2 == NULL)
						return NULL;
					if(!strcmp(root->child->next->name, "ASSIGNOP"))
					{
						if(isRightValuesOnly(root->child->child))
						{
							serror("An expression with only the right value appears to the left of the assignment", root->line, 6);
							//return NULL;
						}						
						if(!sameType(type1, type2))
						{
							serror("Expression type mismatch at both ends of assignment number", root->child->line, 5);
							//return NULL;
						}		
						return type2;
					}
					else if(!strcmp(root->child->next->name, "AND") || !strcmp(root->child->next->name, "OR") || !strcmp(root->child->next->name, "RELOP"))  //ASSUME2
					{
						if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != 0 || type2->u.basic != 0) 
						{
							serror("Type mismatched for operands", root->child->line, 7);
						}
						return type1;
					}
					else if(!strcmp(root->child->next->name, "PLUS") || !strcmp(root->child->next->name, "MINUS") || !strcmp(root->child->next->name, "STAR") || !strcmp(root->child->next->name, "DIV")) //ASSUME2
					{
						if(!sameType(type1, type2))
						{	
							serror("Type mismatched for operands", root->child->line, 7);
							//return NULL;
						}
						else if(type1->kind != BASIC || type2->kind != BASIC)
						{
							serror("Type mismatched for operands", root->child->line, 7);
							//return NULL;
						}
						
						return type1;
					}
					else if(!strcmp(root->child->next->name, "LB")) //Exp ->  Exp LB Exp RB
					{
						if(type1->kind != ARRAY) 
						{
							serror("Variable is not an array", root->line, 10);
							//return NULL;
						}
						if(type2-> kind != BASIC || type2->u.basic != 0) 
						{
							serror( "Variable is not an integer", root->line, 12);
							//return NULL;
						}
						type1 = type1->u.array.elem;
						return type1;
					}
				}
				else// Exp -> Exp DOT ID
				{
					return FindStructureFiled(type1, root->child->next->next->s_val, root->child->next->next->line);
				}
			}
			else if(!strcmp(root->child->name, "LP") || !strcmp(root->child->name, "MINUS")) //Exp -> LP Exp RP | MINUS Exp
			{
				return Exp(root->child->next);
			}
			else if(!strcmp(root->child->name, "NOT")) //Exp -> NOT Exp
			{
				Type type = (Type)malloc(sizeof(Type_));				
				type = Exp(root->next->child);	
				if(type->kind != BASIC)
				{
					serror("Type mismatched for operands", root->child->line, 7);
						//return NULL;
				}
				return type;
			}
			else if(!strcmp(root->child->name, "ID")) //Exp -> ID LP Args RP | ID LP RP | ID
			{
				Type type = (Type)malloc(sizeof(Type_));	
				type = getType(root->child->s_val);
				if(root->child->next == NULL)  //Exp -> ID
				{
					if(type == NULL) 
					{
						serror(stradd("Undefined variable ",root->child->s_val), root->line, 1);
					}
					return type;
				} 
				else //Exp -> ID LP Args RP | ID LP RP
				{
					if(type == NULL) 
					{
						serror(stradd("Undefined function ", root->child->s_val), root->line, 2);
						//return NULL;
					}
					else if(type->kind != FUNCTION) 
					{
						serror("Using function calls on common variables", root->line, 11);
						//return NULL;
					}

					if(!strcmp(root->child->next->next->name,"Args")) 
					{
						Args(root->child->next->next, type); 
					}
					if(type)	
						return type->u.function.ret;
					else
						return NULL;
				}
			}
			else if(!strcmp(root->child->name, "INT")) //Exp -> INT ???
			{
				Type type1 = (Type)malloc(sizeof(Type_));
				type1->kind = BASIC;
				type1->u.basic = 0;
				return type1;
			}
			else if(!strcmp(root->child->name, "FLOAT")) //Exp -> FLOAT ???
			{
				Type type1 = (Type)malloc(sizeof(Type_));
				type1->kind = BASIC;
				type1->u.basic = 1;
				return type1;
			}	
		}
	}
	return NULL;
}

void Args(treeNode* root, Type type)
{
	if(SEDEBUG) printf("Args\n");
	if(root)
    	{
		if(strcmp(root->name, "Args"))
			serror("The actual and formal parameters of a function mismatch", root->line, 9);
		else
		{
			/*if(root->child->next && !strcmp(root->child->next->name, "COMMA")) //Args -> Exp COMMA Args
			{
				Exp(root->child);
				Args(root->child->next->next, type);
			}
			else //Args -> Exp
			{ 
				Exp(root->child);
			}*/
			FieldList ptr;
			if(type)
				ptr = type->u.function.param;
			else
				ptr = NULL;
			
			while(root != NULL && ptr != NULL) {
				/*if(ptr == type->u.function.param) //Function calls itself
				{ 
					serror("The actual and formal parameters of a function mismatch", root->line, 9);
					return;
				}*/
				Type tmp1 = getType(ptr->name);
				Type tmp2 = Exp(root->child);
				if(!sameType(tmp1, tmp2)) //type of parameters differ
				{ 
					serror("The actual and formal parameters of a function mismatch", root->line, 9);
					return;
				}
				//Args -> Exp COMMA Args
				ptr = ptr->tail;
				if(root->child->next)
					root = root->child->next->next;
				else
					root = NULL;
				
			}
			if(ptr != NULL || root != NULL)  // number of parameters differ
			{
				serror("The actual and formal parameters of a function mismatch", root->line, 9);
			}
		}
	}
}

