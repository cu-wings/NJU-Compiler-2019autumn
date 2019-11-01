#include "symbol.h"
#include "semantics.h"
ListHead type_table;

ListHead symbol_table[MAX_SYMBOL];
ListHead lv_table[MAX_LV];
SymbolType * int_type, * float_type;

unsigned int hash(char * str) {
	unsigned int val = 0, i, j, l = strlen(str);
	for(j = 0; j < l; ++ j) {
		val = (val << 2) + str[j];
		if(i = val & ~MAX_SYMBOL) val = (val ^ (i >> 12)) & MAX_SYMBOL;
	}
	return val;
}

void init_symbol() {
	int i;
	list_init(&type_table);
	for(i = 0; i < MAX_SYMBOL; ++ i) list_init(&symbol_table[i]);
	for(i = 0; i < MAX_LV; ++ i) list_init(&lv_table[i]);

	int_type = newp(SymbolType);
	int_type->type = INT_TYPE;

	float_type = newp(SymbolType);
	float_type->type = FLOAT_TYPE;
}

bool isID(SyntaxTreeType * node) {
	if(!strcmp(node->name, "ID") && node->next == NULL) return true;
	if(node->next == NULL) return false;
	if(!strcmp(node->next->name, "DOT") || !strcmp(node->next->name, "LB")) return true;
	return false;
}

void addType(SymbolType * ntype, int line_no) {
	if(DEBUG) printf("addtype\n");
	SymbolType * type = getType(ntype->name);
	if(type != NULL) {
		serror(16, line_no, str_cat("Duplicated name ", ntype->name));
		return;
	}
	list_add_before(&type_table, &ntype->list);
}

SymbolType * getType(char * name) {
	if(DEBUG) printf("gettype %s\n", name);
	ListHead * ptr;
	list_foreach(ptr, &type_table) {
		SymbolType * tmp = list_entry(ptr, SymbolType, list);
		if(!strcmp(tmp->name, name)) return tmp;
	}
	return NULL;
}

void addSymbol(char * name, SymbolType * type, int line_no, int lv) {
	if(DEBUG) printf("add %s\n", name);
	unsigned int h = hash(name);
	while(!list_empty(&symbol_table[h])) {
		SymbolTable * tmp = list_entry(symbol_table[h].next, SymbolTable, list);
		if(!strcmp(tmp->name, name)){
			if(tmp->lv == lv) {
				if(type->type != FUNC_TYPE) serror(3, line_no, str_cat("Redefined variable ", name));
				else if(tmp->type->type != FUNC_TYPE || tmp->type->dec && type->dec) 
					serror(4, line_no, str_cat("Redefined function ", name));
				else if(neqFunc(tmp->type, type)) serror(19, line_no, str_cat("Inconsistent declaration of function ", name));
				else {
					tmp->type->dec |= type->dec;
					return;
				}
			} else break;
		};
		return;
		h ++;
	}
	SymbolTable * tmp = newp(SymbolTable);
	tmp->lv = lv;
	tmp->type = type;
	tmp->line_no = line_no;
	str_cpy(tmp->name, name);
	list_add_after(&symbol_table[h], &tmp->list);
	list_add_after(&lv_table[lv], &tmp->lv_list);
}

bool neqFunc(SymbolType * t1, SymbolType * t2) {
	if(neqType(t1->ret, t2->ret)) return true;
	ListHead * ptr1, * ptr2 = t2->func.next;
	list_foreach(ptr1, &t1->func) {
		if(ptr2 == &t2->func) return true;
		SymbolType * tmp1 = list_entry(ptr1, Func, list)->type;
		SymbolType * tmp2 = list_entry(ptr2, Func, list)->type;
		if(neqType(tmp1, tmp2))return true;
		ptr2 = ptr2->next;
	}
	if(ptr2 != &t2->func) return true;
	return false;

}

void CheckFunc() {
	ListHead * ptr;
	list_foreach(ptr, &lv_table[0]) {
		SymbolTable * tmp = list_entry(ptr, SymbolTable, lv_list);
		if(tmp->type->type != FUNC_TYPE) continue;
		if(!tmp->type->dec) {
			serror(18, tmp->line_no, str_cat("Undefined function ", tmp->name));
		}
	}
}

bool neqType(SymbolType * t1, SymbolType * t2) {
	if(t1 == NULL || t2 == NULL) return true;
	if(t1->type != t2->type) return true;
	if(t1->type == ARRAY_TYPE) {
		if(t1->size != t2->size) return true;
		return neqType(t1->elm, t2->elm);
	}
	if(t1->type != STRUCT_TYPE) return false;
	if(t1->name != NULL && t2->name != NULL && !strcmp(t1->name, t2->name))return false;
	ListHead * ptr1, * ptr2 = t2->structure.next;
	list_foreach(ptr1, &t1->structure) {
		if(ptr2 == &t2->structure) return true;
		SymbolType * tmp1 = list_entry(ptr1, Struct, list)->type;
		SymbolType * tmp2 = list_entry(ptr2, Struct, list)->type;
		if(neqType(tmp1, tmp2))return true;
		ptr2 = ptr2->next;
	}
	if(ptr2 != &t2->structure) return true;
	return false;
}

SymbolType * FindSymbol(char * name) {
	if(DEBUG) printf("find %s\n", name);
	unsigned int h = hash(name);
	while(!list_empty(&symbol_table[h])) {
		SymbolTable * tmp = list_entry(symbol_table[h].next, SymbolTable, list);
		if(!strcmp(tmp->name, name)) {
			return tmp->type;
		}
		h ++;
	}
	return NULL;
}

SymbolType * FindStructFiled(SymbolType * type, char * name, int line_no) {
	if(DEBUG) printf("findfiled\n");
	if(type->type != STRUCT_TYPE) {
		serror(13, line_no, "Variable is not a structure");
		return NULL;
	}
	ListHead * ptr;
	list_foreach(ptr, &type->structure) {
		Struct * tmp = list_entry(ptr, Struct, list);
		if(!strcmp(tmp->name, name)) return tmp->type;
	}
	serror(14, line_no, str_cat("Non-existent field ", name));
	return NULL;
}

int CalcFiledOffset(SymbolType * type, char * name) {
	if(DEBUG) printf("calcfiledoffset\n");
	ListHead * ptr;
	int offset = 0;
	assert(type != NULL);
	list_foreach(ptr, &type->structure) {
		Struct * tmp = list_entry(ptr, Struct, list);
		if(!strcmp(tmp->name, name)) return offset;
		offset += 4;
	}
	return -1;
}

int CalcTypeSize(SymbolType * type) {
	if(DEBUG) printf("calc type size\n");
	if(type->type == STRUCT_TYPE) {
		int size = 0;
		ListHead * ptr;
		list_foreach(ptr, &type->structure) {
			size += 4;
		}
		return size;
	} else if(type->type == ARRAY_TYPE){
		return type->size * CalcTypeSize(type->elm);
	} else return 4;
}

void delLv(int lv) {
	return;
	while(!list_empty(&lv_table[lv])) {
		SymbolTable * tmp = list_entry(lv_table[lv].next, SymbolTable, lv_list);
		list_del(&tmp->list);
		list_del(&tmp->lv_list);
	}
}
