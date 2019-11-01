#include "semantics.h"

extern SymbolType * int_type, * float_type;

void serror(int err_no, int line_no, const char * str) {
	printf("Error type %d at Line %d: %s.\n", err_no, line_no, str);
}

void Program(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("Program %d\n", node->line_no);
	ExtDefList(node->child);
	CheckFunc();
}

void ExtDefList(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("ExtDefList %d\n", node->line_no);
	ExtDef(node->child);
	ExtDefList(node->child->next);
}

void ExtDef(SyntaxTreeType * node) {
	if(node == NULL) return;
	if(DEBUG) printf("ExtDef %d\n", node->line_no);
	SymbolType * type = Specifier(node->child);
	if(type == NULL) return;
	if(!strcmp(node->child->next->name, "ExtDecList")) {
		ExtDecList(node->child->next, type);
	}
	if(!strcmp(node->child->next->name, "FunDec")) {
		if(!strcmp(node->child->next->next->name, "CompSt")) {
			FunDec(node->child->next, type, 1);
			CompSt(node->child->next->next, type, 1);
		} else FunDec(node->child->next, type, 0);
	}
}

SymbolType * Specifier(SyntaxTreeType * node) {
	if(node == NULL) return NULL;
	if(DEBUG) printf("Specifier %d\n", node->line_no);
	if(!strcmp(node->child->name, "TYPE")) {
		if(!strcmp(node->child->str_val, "int")) {
			return int_type;
		} else {
			return float_type;
		}
	} else {
		return StructSpecifier(node->child);
	}
}

SymbolType * StructSpecifier(SyntaxTreeType * node) {
	if(node == NULL) return NULL;
	if(DEBUG) printf("StructSpecifier %d\n", node->line_no);
	if(!strcmp(node->child->next->name, "Tag")) {
		SymbolType * tmp = getType(str_cat("struct ", node->child->next->child->str_val));
		if(tmp == NULL) {
			serror(17, node->line_no, str_cat("Undefined structure ", node->child->next->child->str_val));
		}
		return tmp;
	} else {
		SymbolType * type = newp(SymbolType);
		type->type = STRUCT_TYPE;
		list_init(&type->structure);
		SyntaxTreeType * opttag = node->child->next;
		SyntaxTreeType * deflist;
		if(!strcmp(opttag->name, "OptTag")) {
			str_cpy(type->name, str_cat("struct ", opttag->child->str_val));
			deflist = node->child->next->next->next;
		} else {
			deflist = node->child->next->next;
		}
		if(!strcmp(deflist->name, "RC")) deflist = NULL;
//return NULL;
		while(deflist != NULL) {
			Struct * s = newp(Struct);
			s->type = Specifier(deflist->child->child);
			if(s->type == NULL) {
				if(deflist->child->next != NULL) {
					deflist = deflist->child->next;
				} else break;
				continue;
			}
			SymbolType * stype = s->type;	
			SyntaxTreeType * declist = deflist->child->child->next;
			while(declist != NULL) {
				SyntaxTreeType * vardec = declist->child->child;
				bool ferr = false;
				if(vardec->next != NULL) {
					serror(15, vardec->line_no, str_cat("Redefined field ", vardec->child->str_val));
					if(declist->child->next != NULL) declist = declist->child->next->next;
					else break;
					continue;
				}	
				while(strcmp(vardec->child->name, "ID")) {
					SymbolType * arr = newp(SymbolType);
					arr->size = vardec->child->next->next->int_val;
					arr->elm = s->type;
					arr->type = ARRAY_TYPE;
					s->type = arr;
					vardec = vardec->child;
				}
				ListHead * ptr;
				list_foreach(ptr, &type->structure) {
					Struct * tmps = list_entry(ptr, Struct, list);
					if(!strcmp(tmps->name, vardec->child->str_val)) {
						serror(15, vardec->line_no, str_cat("Redefined field ", tmps->name));
						ferr = true;
						break;
					}
				}
				if(!ferr) {
					str_cpy(s->name, vardec->child->str_val);
					vardec = declist->child->child;
					list_add_before(&type->structure, &s->list);
				}
				if(declist->child->next != NULL) {
					s = newp(Struct);
					s->type = stype;
					declist = declist->child->next->next;
				} else break;
			}
			if(deflist->child->next != NULL) {
				deflist = deflist->child->next;
			} else break;
		}
		if(type->name != NULL)addType(type, node->line_no);
		return type;
	}
}

void ExtDecList(SyntaxTreeType * node, SymbolType * type) {
	if(node == NULL) return;
	if(DEBUG) printf("ExtDecList %d\n", node->line_no);
	while(node != NULL) {
		VarDec(node->child, type, 0);
		if(node->child->next != NULL) {
			node = node->child->next->next;
		} else break;
	}
}

void VarDec(SyntaxTreeType * node, SymbolType * type, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("VarDec %d\n", node->line_no);
	if(type == NULL) return;
	SymbolType * exptype = NULL;
	if(node->next != NULL && !strcmp(node->next->name, "ASSIGNOP")) {
		exptype = Exp(node->next->next);
	}
	SymbolType * t = type;

	while(node != NULL) {
		if(!strcmp(node->child->name, "ID")) {
			addSymbol(node->child->str_val, t, node->child->line_no, lv);
			break;
		}
		SymbolType * arr = newp(SymbolType);
		arr->size = node->child->next->next->int_val;
		arr->elm = t;
		arr->type = ARRAY_TYPE;
		t = arr;
		node = node->child;
	}
	if(exptype != NULL) {
		if(neqType(type, exptype)) {
			serror(5, node->line_no, "Type mismatched for assignment");
		}
	}
}


void FunDec(SyntaxTreeType * node, SymbolType * type, int dec) {
	if(node == NULL) return;
	if(DEBUG) printf("FunDec %d\n", node->line_no);
	SymbolType * t = newp(SymbolType);
	t->ret = type;
	t->type = FUNC_TYPE;
	t->dec = dec;
	list_init(&t->func);
	if(!strcmp(node->child->next->next->name, "VarList")) {
		SyntaxTreeType * varlist = node->child->next->next;
//return NULL;
		while(varlist != NULL) {
			Func * f = newp(Func);
			f->type = Specifier(varlist->child->child);
			SyntaxTreeType * vardec = varlist->child->child->next;
			if(dec) VarDec(vardec, f->type, 1);
//			bool ferr = false;
			while(strcmp(vardec->child->name, "ID")) {
				SymbolType * arr = newp(SymbolType);
				arr->size = vardec->child->next->next->int_val;
				arr->elm = f->type;
				f->type = arr;
				f->type->type = ARRAY_TYPE;
				vardec = vardec->child;
			}
			str_cpy(f->name, vardec->child->str_val);
			list_add_before(&t->func, &f->list);
			ListHead * ptr;
			list_foreach(ptr, &t->func);
			if(varlist->child->next != NULL) {
				varlist = varlist->child->next->next;
			} else break;
		}

	}
	addSymbol(node->child->str_val, t, node->line_no, 0);
}

void CompSt(SyntaxTreeType * node, SymbolType * return_type, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("CompSt %d\n", node->line_no);
	if(!strcmp(node->child->next->name, "DefList")) {
		DefList(node->child->next, lv);
		if(!strcmp(node->child->next->next->name, "StmtList")) 
			StmtList(node->child->next->next, return_type, lv);
	} if(!strcmp(node->child->next->name, "StmtList")) 
		StmtList(node->child->next, return_type, lv);
	delLv(lv);
}

void DefList(SyntaxTreeType * node, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("DefList %d\n", node->line_no);
	Def(node->child, lv);
	DefList(node->child->next, lv);
}

void Def(SyntaxTreeType * node, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("Def %d\n", node->line_no);
	SymbolType * type = Specifier(node->child);
	if(type == NULL) return;
	DecList(node->child->next, type, lv);
}

void DecList(SyntaxTreeType * node, SymbolType * type, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("DecList %d %s\n", node->line_no, node->name);
	while(node != NULL) {
		VarDec(node->child->child, type, lv);
		if(node->child->next != NULL) {
			node = node->child->next->next;
		} else break;
	}
}

void StmtList(SyntaxTreeType * node, SymbolType * return_type, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("StmtList %d\n", node->line_no);
	Stmt(node->child, return_type, lv);
	StmtList(node->child->next, return_type, lv);
}

void Stmt(SyntaxTreeType * node, SymbolType * return_type, int lv) {
	if(node == NULL) return;
	if(DEBUG) printf("Stmt %d %s\n", node->line_no, node->name);
	if(!strcmp(node->child->name, "Exp")) {
		Exp(node->child);
	} else if(!strcmp(node->child->name, "CompSt")) {
		CompSt(node->child, return_type, lv + 1);
	} else if(!strcmp(node->child->name, "RETURN")) {
		SymbolType * type = Exp(node->child->next);
		if(type != NULL && neqType(return_type, type)) {
			serror(8, node->line_no, "Type mismatched for return");
		}
	} else if(!strcmp(node->child->name, "IF") || !strcmp(node->child->name, "WHILE")) {
		SymbolType * type = Exp(node->child->next->next);
		if(type != NULL && type->type != INT_TYPE) {
			serror(7, node->line_no, "Type mismatched for operands");
		}
		SyntaxTreeType * tnode = node->child->next->next->next->next;
		Stmt(tnode, return_type, lv);
		if(tnode->next != NULL) {
			Stmt(tnode->next->next, return_type, lv);
		}
	}
}

SymbolType * Exp(SyntaxTreeType * node) {
	if(node == NULL) return NULL;
	if(DEBUG) printf("Exp %d %s\n", node->line_no, node->name);
	if(!strcmp(node->child->name, "Exp")) {
		SymbolType * type1 = Exp(node->child), * type2 = NULL;
		if(!strcmp(node->child->next->next->name, "Exp")) {
			type2 = Exp(node->child->next->next);
			if(type2 == NULL) return NULL;
		}
		if(type1 == NULL) return NULL;
		char * s = node->child->next->name;
		if(!strcmp(s, "ASSIGNOP")) {
			if(!isID(node->child->child)) {
				serror(6, node->line_no, "The left-hand side of an assignment must be a variable");
				return NULL;
			}
			if(neqType(type1, type2)) {
				serror(5, node->line_no, "Type mismatched for assignment");
				return NULL;
			}
			return type2;
		} else if(!strcmp(s, "AND") || !strcmp(s, "OR") || !strcmp(s, "RELOP")) {
			if(type1->type != INT_TYPE || type2->type != INT_TYPE) {
				serror(7, node->line_no, "Type mismatched for operands");
			}
			return type1;
		} else if(!strcmp(s, "PLUS") || !strcmp(s, "MINUS") || !strcmp(s, "STAR") || !strcmp(s, "DIV")) {
			if((type1->type != INT_TYPE && type1->type != FLOAT_TYPE) || (type2->type != INT_TYPE && type2->type != FLOAT_TYPE) || type1->type != type2->type) {
				serror(7, node->line_no, "Type mismatched for operands");
				return NULL;
			}
			return type1;
		} else if(!strcmp(s, "LB")) {
			if(type1->type != ARRAY_TYPE) {
				serror(10, node->line_no, "Variable is not an array");
				return NULL;
			}
			if(type2->type != INT_TYPE) {
				serror(12, node->line_no, "Variable is not an integer");
				return NULL;
			}
			type1 = type1->elm;
			return type1;
		} else if(!strcmp(s, "DOT")){
			return FindStructFiled(type1, node->child->next->next->str_val, node->child->next->next->line_no);
		}
	} else if(!strcmp(node->child->name, "MINUS") || !strcmp(node->child->name, "LP")) {
		return Exp(node->child->next);
	} else if(!strcmp(node->child->name, "NOT")) {
		SymbolType * type = Exp(node->child->next);
		if(type->type != INT_TYPE) {
			serror(7, node->line_no, "Type mismatched for operands");
			return NULL;
		}
		return type;
	} else if(!strcmp(node->child->name, "ID")) {
		SymbolType * type = FindSymbol(node->child->str_val);
		if(node->child->next == NULL) {
			if(type == NULL) {
				serror(1, node->line_no, str_cat("Undefined variable ", node->child->str_val));
			}
			return type;
		} else {
			if(type == NULL) {
				serror(2, node->line_no, str_cat("Undefined function ", node->child->str_val));
				return NULL;
			}
			if(type->type != FUNC_TYPE) {
				serror(11, node->line_no, "Variable is not a function");
				return NULL;
			}
			Args(node->child->next->next, type);
			return type->ret;
		}
	} else if(!strcmp(node->child->name, "INT")) {
		return int_type;
	} else if(!strcmp(node->child->name, "FLOAT")) {
		return float_type;
	}
	return NULL;
}

void Args(SyntaxTreeType * node, SymbolType * type) {
	if(node == NULL) return;
	if(DEBUG) printf("Args %d %s\n", node->line_no, node->name);
	if(strcmp(node->name, "Args")) {
		if(list_empty(&type->func)) return;
		serror(9, node->line_no, "Function is not applicable for arguments");
	} else {
		ListHead * ptr = &type->func;
		
		while(node != NULL) {
			ptr = ptr->next;
			if(ptr == &type->func) {
				serror(9, node->line_no, "Function is not applicable for arguments");
				return;
			}
			SymbolType * tmp1 = list_entry(ptr, Func, list)->type;
			SymbolType * tmp2 = Exp(node->child);
			if(neqType(tmp1, tmp2)) {
				serror(9, node->line_no, "Function is not applicable for arguments");
				return;
			}
			if(node->child->next != NULL) {
				node = node->child->next->next;
			} else break;
		}
		if(ptr->next != &type->func) {
			serror(9, node->line_no, "Function is not applicable for arguments");
		}
	}
}


