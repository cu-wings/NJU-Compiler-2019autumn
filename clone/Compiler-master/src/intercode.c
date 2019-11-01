#include "intercode.h"

ListHead intercodes;
ListHead dec_table;
Operand * zero_op;
int var_num = 0;
int label_num = 0;

char * NewVar() {
	char * s = (char *)malloc(34);
	sprintf(s, "t__%d", var_num ++);
	return s;
}

char * NewLabel() {
	char * s = (char *)malloc(34);
	sprintf(s, "l__%d", label_num ++);
	return s;
}

char * negOperand(char * op) {
	char * s;
	if(!strcmp(op, "==")) {str_cpy(s, "!=");} 
	else if(!strcmp(op, "!=")) {str_cpy(s, "==");}
	else if(!strcmp(op, ">")) {str_cpy(s, "<=");}
	else if(!strcmp(op, ">=")) {str_cpy(s, "<");}
	else if(!strcmp(op, "<")) {str_cpy(s, ">=");}
	else if(!strcmp(op, "<=")) {str_cpy(s, ">");}
	return s;
}

Operand * NewOperand(char * relop) {
	Operand * op = newp(Operand);
	str_cpy(op->str_val, relop);
	return op;
}

void NewCode(int code_type, int carg, ...) {
	InterCode * code = newp(InterCode);
	code->type = code_type;
	list_init(&code->op_list);
	va_list ap;
	va_start(ap, carg);
	int i;
	for(i = 0; i < carg; ++ i) {
		Operand * op = va_arg(ap, Operand *);
		Operand * op1 = newp(Operand);
		memcpy(op1, op, sizeof(Operand));
		list_add_before(&code->op_list, &op1->list);
	}
	va_end(ap);
	list_add_before(&intercodes, &code->list);
}

bool ParamSymbol(char * name) {
	ListHead * ptr;
	list_foreach(ptr, &dec_table) {
		Operand * op = list_entry(ptr, Operand, list);
		if(!strcmp(op->str_val, name)) return true;
	}
	return false;
}

void translate_Program(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Program %d\n", node->line_no);
	zero_op = NewOperand("#0");
	list_init(&intercodes);
	list_init(&dec_table);
	NewCode(FUNCTION_CODE, 1, NewOperand("START__"));
	translate_ExtDefList(node->child);
	FILE * fp = fopen("obj/result.ir", "w");
	ListHead * ptr;
	Operand * op[4];
	list_foreach(ptr, &intercodes) {
		InterCode * code = list_entry(ptr, InterCode, list);
		int i = 0;
		ListHead * ptr_;
		list_foreach(ptr_, &code->op_list) {
			op[i ++]= list_entry(ptr_, Operand, list);
		}
		switch(code->type) {
			case(LABEL_CODE):
				fprintf(fp, "LABEL %s :\n", op[0]->str_val);
				break;
			case(FUNCTION_CODE) :
				fprintf(fp, "FUNCTION %s :\n", op[0]->str_val);
				break;
			case(ASSIGN_CODE) :
				fprintf(fp, "%s := %s\n", op[0]->str_val, op[1]->str_val);
				break;
			case(PLUS_CODE) :
				fprintf(fp, "%s := %s + %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				break;
			case(MINUS_CODE) :
				fprintf(fp, "%s := %s - %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				break;
			case(STAR_CODE) :
				fprintf(fp, "%s := %s * %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				break;
			case(DIV_CODE) :
				fprintf(fp, "%s := %s / %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				break;
			case(ZEROMINUS_CODE):
				fprintf(fp, "%s := #0 - %s\n", op[0]->str_val, op[1]->str_val);
				break;
			case(GOTO_CODE):
				fprintf(fp, "GOTO %s\n", op[0]->str_val);
				break;
			case(IF_CODE):
				fprintf(fp, "IF %s %s %s GOTO %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val, op[3]->str_val);
				break;
			case(RETURN_CODE):
				fprintf(fp, "RETURN %s\n", op[0]->str_val);
				break;
			case(DEC_CODE):
				fprintf(fp, "DEC %s %s\n", op[0]->str_val, op[1]->str_val);
				break;
			case(ARG_CODE):
				fprintf(fp, "ARG %s\n", op[0]->str_val);
				break;
			case(CALL_CODE):
				fprintf(fp, "%s := CALL %s\n", op[0]->str_val, op[1]->str_val);
				break;
			case(PARAM_CODE):
				fprintf(fp, "PARAM %s\n", op[0]->str_val);
				break;
			case(READ_CODE):
				fprintf(fp, "READ %s\n", op[0]->str_val);
				break;
			case(WRITE_CODE):
				fprintf(fp, "WRITE %s\n", op[0]->str_val);
				break;
		}
	}
	fclose(fp);
}

void translate_ExtDefList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_ExtDefList %d\n", node->line_no);
	translate_ExtDef(node->child);
	translate_ExtDefList(node->child->next);
}

void translate_ExtDef(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_ExtDef %d\n", node->line_no);
	if(!strcmp(node->child->next->name, "FunDec")) {
		translate_FunDec(node->child->next);
		translate_CompSt(node->child->next->next);
	} else if(!strcmp(node->child->next->name, "ExtDecList")) {
		translate_ExtDecList(node->child->next);
	}
}

void translate_ExtDecList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_ExtDecList %d\n", node->line_no);
	translate_VarDec(node->child);
	if(node->child->next != NULL) translate_ExtDecList(node->child->next->next);
}

void translate_FunDec(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_FunDec %d\n", node->line_no);
	Operand * op = NewOperand(node->child->str_val);
	NewCode(FUNCTION_CODE, 1, op);
	if(!strcmp(node->child->next->next->name, "VarList")) {
		translate_VarList(node->child->next->next);
	}
}

void translate_VarList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_VarList %d\n", node->line_no);
	translate_ParamDec(node->child);
	if(node->child->next != NULL) {
		translate_VarList(node->child->next->next);
	}
}

void translate_ParamDec(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_ParamDec %d\n", node->line_no);
	node = node->child->next;
	while(strcmp(node->child->name, "ID")) node = node->child;
	Operand * op = NewOperand(node->child->str_val);
	NewCode(PARAM_CODE, 1, op);
	Operand * op1 = NewOperand(node->child->str_val);
	list_add_before(&dec_table, &op1->list);
}

void translate_CompSt(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_CompSt %d\n", node->line_no);
	if(!strcmp(node->child->next->name, "DefList")) {
		translate_DefList(node->child->next);
		if(!strcmp(node->child->next->next->name, "StmtList")) 
			translate_StmtList(node->child->next->next);
	} if(!strcmp(node->child->next->name, "StmtList")) 
		translate_StmtList(node->child->next);
}

void translate_DefList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_DefList %d\n", node->line_no);
	translate_Def(node->child);
	translate_DefList(node->child->next);
}

void translate_Def(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Def %d\n", node->line_no);
	translate_DecList(node->child->next);
}

void translate_DecList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_DecList %d\n", node->line_no);
	translate_Dec(node->child);
	if(node->child->next != NULL) translate_DecList(node->child->next->next);
}

void translate_Dec(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Dec %d\n", node->line_no);
	if(node->child->next == NULL) {
		translate_VarDec(node->child);
	} else {
		translate_Exp(node);
	}
}

void translate_VarDec(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_VarDec %d\n", node->line_no);
	if(!strcmp(node->child->name, "ID")) {
		SymbolType * type = FindSymbol(node->child->str_val);
		if(type->type == STRUCT_TYPE || type->type == ARRAY_TYPE) {
			char * s = (char *)malloc(32);
			sprintf(s, "%d", CalcTypeSize(type));
			NewCode(DEC_CODE, 2, NewOperand(node->child->str_val), NewOperand(s));
		}
	} else translate_VarDec(node->child);
}

void translate_StmtList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_StmtList %d\n", node->line_no);
	translate_Stmt(node->child);
	translate_StmtList(node->child->next);
}

void translate_Stmt(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Stmt %d\n", node->line_no);
	InterCode * code = newp(InterCode);
	list_init(&code->op_list);
	if(!strcmp(node->child->name, "Exp")) {
		translate_Exp(node->child);
	} else if(!strcmp(node->child->name, "CompSt")) {
		translate_CompSt(node->child);
	} else if(!strcmp(node->child->name, "RETURN")) {
		Operand * op = translate_Exp(node->child->next);
		NewCode(RETURN_CODE, 1, op);
	} else if(!strcmp(node->child->name, "IF")) {
		Operand * l1 = NewOperand(NewLabel());
		translate_Cond(node->child->next->next, NULL, l1);
		translate_Stmt(node->child->next->next->next->next);
		if(node->child->next->next->next->next->next != NULL) {
			Operand * l2 = NewOperand(NewLabel());
			NewCode(GOTO_CODE, 1, l2);
			NewCode(LABEL_CODE, 1, l1);
			translate_Stmt(node->child->next->next->next->next->next->next);
			NewCode(LABEL_CODE, 1, l2);
		} else NewCode(LABEL_CODE, 1, l1);
	} else if(!strcmp(node->child->name, "WHILE")) {
		Operand * l1 = NewOperand(NewLabel()), * l2 = NewOperand(NewLabel());
		NewCode(LABEL_CODE, 1, l1);
		translate_Cond(node->child->next->next, NULL, l2);
		translate_Stmt(node->child->next->next->next->next);
		NewCode(GOTO_CODE, 1, l1);
		NewCode(LABEL_CODE, 1, l2);
	}
}

Operand * translate_Exp(SyntaxTreeType * node) {
	if(node == NULL) return NULL;
	if(DEBUG) printf("translate_Exp %d\n", node->line_no);
	if(!strcmp(node->child->name, "Exp") || !strcmp(node->child->name, "VarDec")) {
		Operand * op1, *op2;
		char * s = node->child->next->name;
		if(strcmp(s, "LB")) {
			op1 = translate_Exp(node->child);
			if(!strcmp(node->child->next->next->name, "Exp")) {
				op2 = translate_Exp(node->child->next->next);
			}
		}
		if(!strcmp(s, "ASSIGNOP")) {
			NewCode(ASSIGN_CODE, 2, op1, op2);
			return op1;
		} else if(!strcmp(s, "PLUS") || !strcmp(s, "MINUS") || !strcmp(s, "STAR") || !strcmp(s, "DIV")) {
			int code_type;
			switch(s[0]) {
				case 'P' : code_type = PLUS_CODE; break;
				case 'M' : code_type = MINUS_CODE; break;
				case 'S' : code_type = STAR_CODE; break;
				case 'D' : code_type = DIV_CODE; break;
			}
			Operand * op3 = NewOperand(NewVar());
			NewCode(code_type, 3, op3, op1, op2);
			return op3;
		} else if(!strcmp(s, "AND") || !strcmp(s, "OR") || !strcmp(s, "RELOP")) {
			assert(0);
		} else if(!strcmp(s, "LB")) {
			Operand * op3 = NewOperand(NewVar());
			Operand * op4 = CalcArrayOffset(node);
			while(strcmp(node->child->name, "ID")) node = node->child;
			Operand * op5 = translate_Exp(node);
			NewCode(PLUS_CODE, 3, op3, op5, op4);
			return NewOperand(str_cat("*", op3->str_val));
		} else if(!strcmp(s, "DOT")){
			Operand * op3 = NewOperand(NewVar());
			char * offset = (char *)malloc(33);
			sprintf(offset, "#%d", CalcFiledOffset(op1->type, node->child->next->next->str_val));
			NewCode(PLUS_CODE, 3, op3, op1, NewOperand(offset));
			return NewOperand(str_cat("*", op3->str_val));
		}
	} else if(!strcmp(node->child->name, "MINUS")) {
		Operand * op1 = NewOperand(NewVar());
		Operand * op2 = translate_Exp(node->child->next);
		NewCode(MINUS_CODE, 3, op1, zero_op, op2);
		return op1;
	} else if(!strcmp(node->child->name, "LP")) {
		return translate_Exp(node->child->next);
	} else if(!strcmp(node->child->name, "NOT")) {
		assert(0);
	} else if(!strcmp(node->child->name, "ID")) {
		if(node->child->next == NULL) {
			Operand * op = NewOperand(node->child->str_val);
			op->type = FindSymbol(node->child->str_val);
			if(!ParamSymbol(node->child->str_val) && op->type != NULL && 
			  (op->type->type == ARRAY_TYPE || op->type->type == STRUCT_TYPE)) {
				Operand * op2 = NewOperand(str_cat("&",op->str_val));
				op2->type = op->type;
				return op2;
			} else return op;
		} else {
			SymbolType * type = FindSymbol(node->child->str_val);
			Operand * op1 = NewOperand(NewVar()), * op2 = NewOperand(node->child->str_val);
			if(!strcmp(node->child->next->next->name, "Args")) translate_Args(node->child->next->next, type->func.next);
			NewCode(CALL_CODE, 2, op1, op2);
			return op1;
		}
	} else if(!strcmp(node->child->name, "INT")) {
		Operand * op = newp(Operand);
		op->str_val = (char *)malloc(33);
		sprintf(op->str_val, "#%d", node->child->int_val);
		Operand * op1 = NewOperand(NewVar());
		NewCode(ASSIGN_CODE, 2, op1, op);
		return op1;
	} else if(!strcmp(node->child->name, "FLOAT")) {
		assert(0);
	} else if(!strcmp(node->child->name, "READ")) {
		Operand * op = NewOperand(NewVar());
		NewCode(READ_CODE, 1, op);
		return op;
	} else if(!strcmp(node->child->name, "WRITE")) {
		Operand * op = translate_Exp(node->child->next->next);
		NewCode(WRITE_CODE, 1, op);
		return zero_op;
	}
	return NULL;
}

void translate_Args(SyntaxTreeType * node, ListHead * ptr) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Args %d\n", node->line_no);
	Func * func = list_entry(ptr, Func, list);
	Operand * op = translate_Exp(node->child);
	if(func->type->type == ARRAY_TYPE || func->type->type == STRUCT_TYPE) {
//		printf("***\n");
		if(op->str_val[0] == '*') op->str_val = &op->str_val[1];
	}
	if(node->child->next != NULL) translate_Args(node->child->next->next, ptr->next);
	NewCode(ARG_CODE, 1, op);
}

void translate_Cond(SyntaxTreeType * node, Operand * true_label, Operand * false_label) {
	if(node == NULL) return;
	if(DEBUG) printf("translate_Cond %d\n", node->line_no);
	if(!strcmp(node->child->name, "Exp")) {
		char * s = node->child->next->name;
		if(!strcmp(s, "ASSIGNOP")) {
			Operand * op1 = translate_Exp(node->child);
			Operand * op2 = translate_Exp(node->child->next->next);
			NewCode(ASSIGN_CODE, 2, op1, op2);
			if(true_label != NULL) NewCode(IF_CODE, 4, op1, NewOperand("!="), zero_op, true_label);
			if(false_label != NULL) NewCode(IF_CODE, 4, op1, NewOperand("=="), zero_op, false_label);
		} else if(!strcmp(s, "PLUS") || !strcmp(s, "MINUS") || !strcmp(s, "STAR") || !strcmp(s, "DIV")) {
			Operand * op1 = translate_Exp(node->child);
			Operand * op2 = translate_Exp(node->child->next->next);
			int code_type;
			switch(s[0]) {
				case 'P' : code_type = PLUS_CODE; break;
				case 'M' : code_type = MINUS_CODE; break;
				case 'S' : code_type = STAR_CODE; break;
				case 'D' : code_type = DIV_CODE; break;
			}
			Operand * op3 = NewOperand(NewVar());
			NewCode(code_type, 3, op3, op1, op2);
			if(true_label != NULL) NewCode(IF_CODE, 4, op3, NewOperand("!="), zero_op, true_label);
			if(false_label != NULL) NewCode(IF_CODE, 4, op3, NewOperand("=="), zero_op, false_label);
		} else if(!strcmp(s, "AND")) {
			translate_Cond(node->child, NULL, false_label);
			translate_Cond(node->child->next->next, true_label, false_label);
		} else if(!strcmp(s, "OR")) {
			translate_Cond(node->child, true_label, NULL);
			translate_Cond(node->child->next->next, true_label, false_label);
		} else if(!strcmp(s, "RELOP")) {
			Operand * op1 = translate_Exp(node->child);
			Operand * op2 = translate_Exp(node->child->next->next);
			if(true_label != NULL) NewCode(IF_CODE, 4, op1, NewOperand(node->child->next->str_val), op2, true_label);
			if(false_label != NULL) NewCode(IF_CODE, 4, op1, NewOperand(negOperand(node->child->next->str_val)), op2, false_label);
		} else if(!strcmp(s, "LB") || !strcmp(s, "DOT")) {
			Operand * op = translate_Exp(node);
			if(true_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("!="), zero_op, true_label);
			if(false_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("=="), zero_op, false_label);
		}
	} else if(!strcmp(node->child->name, "MINUS")){
		translate_Cond(node->child->next, true_label, false_label);
	}
	else if(!strcmp(node->child->name, "LP")) {
		translate_Cond(node->child->next, true_label, false_label);
	} else if(!strcmp(node->child->name, "NOT")) {
		translate_Cond(node->child->next, false_label, true_label);
	} else if(!strcmp(node->child->name, "ID")) {
		Operand * op = translate_Exp(node);
		if(true_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("!="), zero_op, true_label);
		if(false_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("=="), zero_op, false_label);

	} else if(!strcmp(node->child->name, "INT")) {
		if(node->child->int_val && true_label != NULL) NewCode(GOTO_CODE, 1, true_label);
		if(!node->child->int_val && false_label != NULL) NewCode(GOTO_CODE, 1, false_label);
	} else if(!strcmp(node->child->name, "FLOAT")) {
		assert(0);
	} else if(!strcmp(node->child->name, "READ") || !strcmp(node->child->name, "WRITE")) {
		Operand * op = translate_Exp(node);
		if(true_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("!="), zero_op, true_label);
		if(false_label != NULL) NewCode(IF_CODE, 4, op, NewOperand("=="), zero_op, false_label);
	}
	return;
}

Operand * CalcArrayOffset(SyntaxTreeType * node) {
	if(DEBUG) printf("calc array size\n");
	ListHead elm_size;
	list_init(&elm_size);
	while(node->child->next != NULL) {
		list_add_after(&elm_size, &translate_Exp(node->child->next->next)->list);
		node = node->child;
	}
	SymbolType * type = FindSymbol(node->child->str_val);
	ListHead * ptr;
	Operand * op = NULL;
	list_foreach(ptr, &elm_size) {
		Operand * op1 = list_entry(ptr, Operand, list), * op2 = NewOperand(NewVar());
		char * s = (char *)malloc(32);
		sprintf(s, "#%d", CalcTypeSize(type->elm));
		if(op == NULL) {
			op = op2;
			NewCode(STAR_CODE, 3, op2, op1, NewOperand(s));
		} else {
			NewCode(STAR_CODE, 3, op2, op1, NewOperand(s));
			NewCode(PLUS_CODE, 3, op, op, op2);
		}
		type = type->elm;
	}
	op->type = type;
	return op;
}

