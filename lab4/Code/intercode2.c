#include "intercode.h"

void translateStmt(treeNode *root)
{
	if(root != NULL)
	{
		if(!strcmp(root->child->name, "Exp"))
		{
			translateExp(root->child);
		}
		else if(!strcmp(root->child->name, "CompSt"))
		{
			translateCompSt(root->child);
		}
		else if(!strcmp(root->child->name, "RETURN"))
		{
			Operand op = translateExp(root->child->next);
			new_code(RET, op);
		}
		else if(!strcmp(root->child->name, "IF")) 
		{
			Operand label1 = new_op(LABELNUM, VAL, labelNum++);
			translateCond(root->child->next->next, NULL, &label1); // no true label
			translateStmt(root->child->next->next->next->next);
			if(root->child->next->next->next->next->next != NULL)
			{
				Operand label2 = new_op(LABELNUM, VAL, labelNum++);
				new_code(GOTO, label2);		//goto else code
				new_code(LABEL, label1);	//goto if=false code
				translateStmt(root->child->next->next->next->next->next->next); //else ...
				new_code(LABEL, label2);		//else=false code
			}
			else
			{
				new_code(LABEL, label1);
			}
			
		}
		else if(!strcmp(root->child->name, "WHILE")) 
		{
			Operand label1 = new_op(LABELNUM, VAL, labelNum++);
			Operand label2 = new_op(LABELNUM, VAL, labelNum++);
			new_code(LABEL, label1);
			translateCond(root->child->next->next, NULL, &label2); //no true label, iffalse goto label2
			translateStmt(root->child->next->next->next->next);
			new_code(GOTO, label1);		//continue
			new_code(LABEL, label2); //out of while
		}
	}
}

Operand translateExp(treeNode *root)
{
	if(root != NULL)
	{
		if(!strcmp(root->child->name, "Exp")) // + - * / = and or  (two operands)
		{
			Operand op1, op2;
			char* operator =  root->child->next->name;
			if(strcmp(operator, "LB"))  //except array
			{
				op1 = translateExp(root->child);
				if(!strcmp(root->child->next->next->name, "Exp")) 
				{
					op2 = translateExp(root->child->next->next);
				}
			}

			if(!strcmp(operator,"ASSIGNOP"))
			{
				new_code(ASSIGN, op1, op2);
				return op1;
			}
			else if(!strcmp(operator, "PLUS") || !strcmp(operator, "MINUS") || !strcmp(operator, "STAR") || !strcmp(operator, "DIV")) 
			{
				int kind;
				switch(operator[0]) {
					case 'P' : kind = ADD; break;
					case 'M' : kind = SUB; break;
					case 'S' : kind = MUL; break;
					case 'D' : kind = DIVIDE; break;
				}
				Operand result = new_op(TEMPVAR, VAL, tempvarNum++);
				new_code(kind, result, op1, op2);
				return result;
			}
			else if(!strcmp(operator, "AND") || !strcmp(operator, "OR") || !strcmp(operator, "RELOP"))
			{
				return new_op(TEMPVAR, VAL, -1);
				//assert(0); // translate_Cond	
			}
			else if(!strcmp(operator, "LB")) //array
			{
				treeNode* tempRoot = root;
				while(strcmp(tempRoot->child->name, "ID"))
				{
					tempRoot = tempRoot->child;
				}
				if(tempRoot->next)
				{
					if(!strcmp(tempRoot->next->name,"DOT"))
					{
						irerror = 1;
						return new_op(TEMPVAR, VAL, -1);
					}
				}
				Operand op3 = translateExp(tempRoot);	//get array op
				//op3.kind = VARIABLE;
				Type type = getType(op3.u.var.name);
				int size = 4;
				Operand prev = op3;
				if(prev.type == VAL)	// if op3 is address of array, don't need to get address
					prev.type = ADDRESS;
				else
					prev.type = VAL;
				while(strcmp(root->child->name, "ID"))
				{
					if(!strcmp(root->child->next->next->name, "Exp"))
					{
						Operand op4 = translateExp(root->child->next->next);
						Operand op5 = new_op(CONSTANT, VAL, size);
						Operand op6 = new_op(TEMPVAR, VAL, tempvarNum++);
						new_code(MUL, op6, op4, op5);
						Operand op7 = new_op(TEMPVAR, VAL, tempvarNum++);
						new_code(ADD, op7, prev, op6);
						prev = op7;
					}
					root = root->child;
					type = type->u.array.elem;
					size *= type->u.array.size;
				}
				prev.type = ADDRESS;	//prev is the address of the array element
				return prev;
			}
			else if(!strcmp(operator, "DOT")) //struct
			{
				irerror = 1;
				return new_op(TEMPVAR, VAL, -1);
			}
		}
		else if(!strcmp(root->child->name, "MINUS"))  
		{
			Operand op = new_op(CONSTANT, VAL, 0);
			Operand op1 = new_op(TEMPVAR, VAL, tempvarNum++);
			Operand op2 = translateExp(root->child->next);
			Operand op3 = new_op(TEMPVAR, VAL, tempvarNum++);
			new_code(ASSIGN, op1, op);
			new_code(SUB, op3, op1, op2);
			return op3;//return result;
		}
		else if(!strcmp(root->child->name, "NOT"))
		{
			return new_op(TEMPVAR, VAL, -1);
			//assert(0); // translate_Cond	
		}
		else if(!strcmp(root->child->name, "LP")) // LP EXP RP
		{
			return translateExp(root->child->next);
		}
		else if(!strcmp(root->child->name, "ID"))
		{
			if(root->child->next == NULL)
			{
				Type type = getType(root->child->s_val);
				if(type != NULL && (type->kind == ARRAY || type->kind==STRUCTVAR))
				{
					Operand *op = findVarOp(root->child->s_val);
					if(IRDEBUG)
					{
						if(op == NULL)
						{
							*op = new_op(VARIABLE, ADDRESS, varNum++, root->child->s_val);
							addOpList(*op);
						}
					}
					return *op;
				}
				else
				{
					Operand *op = findVarOp(root->child->s_val);
					if(IRDEBUG)
					{
						if(op == NULL)
						{
							*op = new_op(VARIABLE, VAL, varNum++, root->child->s_val);
							addOpList(*op);
						}
					}
					return *op;
				}
			}
			else //ID LP Args RP || ID LP RP
			{
				Type kind = getType(root->child->s_val);
				Operand op1, op2;
				op1 = new_op(TEMPVAR, VAL, tempvarNum++);
				op2 = new_op(FUNCNAME, VAL, root->child->s_val);
				if(!strcmp(root->child->s_val, "read"))
				{
					new_code(READ, op1);
				}
				else if(!strcmp(root->child->s_val, "write"))
				{
					if(!strcmp(root->child->next->next->child->name,"Exp"))
					{
						op1 = translateExp(root->child->next->next->child);
					}
					new_code(WRITE, op1);
				}
				else
				{
					if(!strcmp(root->child->next->next->name,"Args"))
						translateArgs(root->child->next->next, kind->u.function.param);
					new_code(CALL, op1, op2);
				}
				return op1;
			}
		}
		else if(!strcmp(root->child->name, "INT"))
		{
			Operand op = new_op(CONSTANT, VAL, root->child->i_val);
			//Operand op1 = new_op(TEMPVAR, VAL, tempvarNum++);
			//new_code(ASSIGN, op1, op);
			return op;
		}
		else if(!strcmp(root->child->name, "FLOAT"))
		{
			return new_op(TEMPVAR, VAL, -1);
			//assert(0);
		}
		
	}
}

char *Inverse_operator(char *s)
{
	char *neg_s = NULL;//= (char*)malloc(sizeof(char)*3);
	if(!strcmp(s,">"))  neg_s = "<=" ;
	if(!strcmp(s,">="))  neg_s = "<" ;
	if(!strcmp(s,"<="))  neg_s = ">" ;
	if(!strcmp(s,"<"))  neg_s = ">=" ;
	if(!strcmp(s,"=="))  neg_s = "!=" ;
	if(!strcmp(s,"!="))  neg_s = "==" ;
	if(IRDEBUG) printf("%s\n", s);
	if(IRDEBUG) printf("%s\n", neg_s);
	return neg_s;
}

void translateCond(treeNode *root, Operand *true_label, Operand *false_label)
{
	if(root != NULL)
	{
		Operand zero = new_op(CONSTANT, VAL, 0);
		if(!strcmp(root->child->name, "Exp"))
		{
			char *operator = root->child->next->name;
			if(!strcmp(operator, "ASSIGNOP"))
			{
				Operand op1 = translateExp(root->child);
				Operand op2 = translateExp(root->child->next->next);
				new_code(ASSIGN, op1, op2);
				if(true_label != NULL) 
					new_code(IFGOTO, op1, "!=", zero, *true_label);
				if(false_label != NULL) 
					new_code(IFGOTO, op1, "!=", zero, *false_label);
			}
			else if(!strcmp(operator, "PLUS") || !strcmp(operator, "MINUS") || !strcmp(operator, "STAR") || !strcmp(operator, "DIV"))   
			{
				Operand op1 = translateExp(root->child);
				Operand op2 = translateExp(root->child->next->next);
				int kind;
				switch(operator[0]) {
					case 'P' : kind = ADD; break;
					case 'M' : kind = SUB; break;
					case 'S' : kind = MUL; break;
					case 'D' : kind = DIVIDE; break;
				}
				Operand result = new_op(TEMPVAR,  VAL, tempvarNum++);
				new_code(kind, result, op1, op2);
				if(true_label != NULL) 
					new_code(IFGOTO, result, "!=", zero, *true_label);
				if(false_label != NULL) 
					new_code(IFGOTO, result, "==", zero, *false_label);
			}
			else if(!strcmp(operator, "AND"))
			{
				translateCond(root->child, NULL, false_label);
				translateCond(root->child->next->next, true_label, false_label);
			}
			else if(!strcmp(operator, "OR"))
			{
				translateCond(root->child, true_label, NULL);
				translateCond(root->child->next->next, true_label, false_label);
			}
			else if(!strcmp(operator, "RELOP"))
			{
				Operand op1 = translateExp(root->child);
				Operand op2 = translateExp(root->child->next->next);
				if(true_label != NULL) 
					new_code(IFGOTO, op1, root->child->next->s_val, op2, *true_label);
				if(false_label != NULL) 
					new_code(IFGOTO, op1, Inverse_operator(root->child->next->s_val), op2, *false_label);
			}
			else if(!strcmp(operator, "LB") || !strcmp(operator, "DOT"))
			{
				Operand op = translateExp(root);
				if(true_label != NULL) new_code(IFGOTO, op, "!=", zero, *true_label);
				if(false_label != NULL) new_code(IFGOTO, op, "==", zero, *false_label);
			}
		}
		else if(!strcmp(root->child->name, "MINUS"))  
		{
			translateCond(root->child->next, true_label, false_label);
		}
		else if(!strcmp(root->child->name, "LP"))  
		{
			translateCond(root->child->next, true_label, false_label);
		}
		else if(!strcmp(root->child->name, "NOT"))  
		{
			translateCond(root->child->next, false_label, true_label);
		}
		else if(!strcmp(root->child->name, "ID"))  
		{
			Operand op = translateExp(root);
			if(true_label != NULL) new_code(IFGOTO, op, "!=", zero, *true_label);
			if(false_label != NULL) new_code(IFGOTO, op, "==", zero, *false_label);
		}
		else if(!strcmp(root->child->name, "INT"))  
		{
			if(root->child->i_val && true_label != NULL) new_code(GOTO, *true_label);
			if(!root->child->i_val && false_label != NULL) new_code(GOTO, *false_label);
		}
		else if(!strcmp(root->child->name, "FLOAT"))  
		{
			//never comes here
		}
		return;
	}
}







