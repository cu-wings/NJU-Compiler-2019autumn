#include "asmcode.h"
extern ListHead intercodes;

int cur_addr = 0;
int cur_param = 0;
int cur_arg = 0;
int address[MAX_SYMBOL];
int param[MAX_SYMBOL];
bool save_reg = 0;
ListHead arg_list;

void TranslateAsm() {
	printf(".data\n_prompt: .asciiz \"Enter an integer: \"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\nli $v0, 4\nla $a0, _prompt\nsyscall\nli $v0, 5\nsyscall\njr $ra\nwrite:\nli $v0, 1\nsyscall\nli $v0, 4\nla $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n");
	ListHead * ptr;
	Operand * op[4];
	int i;
	list_init(&arg_list);
	Operand * op1;
	list_foreach(ptr, &intercodes) {
		InterCode * code = list_entry(ptr, InterCode, list);
		int i = 0;
		ListHead * ptr_;
		list_foreach(ptr_, &code->op_list) {
			op[i ++]= list_entry(ptr_, Operand, list);
		}
		char *r1, *r2, *r3, *opt;
		switch(code->type) {
			case(LABEL_CODE):
//				printf("LABEL %s :\n", op[0]->str_val);
				printf("%s:\n", op[0]->str_val);
				break;
			case(FUNCTION_CODE) :
//				printf("FUNCTION %s :\n", op[0]->str_val);
				printf("%s:\n", op[0]->str_val);
				printf("addi $sp, $sp, -4\n");
				printf("sw $fp, 0($sp)\n");
				printf("move $fp, $sp\n");
				cur_addr = 0;
				cur_param = 0;
				break;
			case(ASSIGN_CODE) :
//				printf("%s := %s\n", op[0]->str_val, op[1]->str_val);
				r1 = GetReg(op[0]->str_val);
				if(op[1]->str_val[0] != '#') {
					r2 = GetReg(op[1]->str_val);
					printf("move %s, %s\n", r1, r2);
				} else {
					printf("li %s, %s\n", r1, &op[1]->str_val[1]);
				}
				PutInMemory(r1, op[0]->str_val);
				break;
			case(PLUS_CODE) :
//				printf("%s := %s + %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[1]->str_val);
				r3 = GetReg(op[2]->str_val);
				printf("add %s, %s, %s\n", r1, r2, r3);
				PutInMemory(r1, op[0]->str_val);
				break;
			case(MINUS_CODE) :
//				printf("%s := %s - %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[1]->str_val);
				r3 = GetReg(op[2]->str_val);
				printf("sub %s, %s, %s\n", r1, r2, r3);
				PutInMemory(r1, op[0]->str_val);
				break;
			case(STAR_CODE) :
//				printf("%s := %s * %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[1]->str_val);
				if(op[2]->str_val[0] != '#') {
					r3 = GetReg(op[2]->str_val);
					printf("mul %s, %s, %s\n", r1, r2, r3);
				} else {
					printf("mul %s, %s, %s\n", r1, r2, &op[2]->str_val[1]);
				}
				PutInMemory(r1, op[0]->str_val);
				break;
			case(DIV_CODE) :
//				printf("%s := %s / %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[1]->str_val);
				r3 = GetReg(op[2]->str_val);
				printf("div %s, %s\n", r2, r3);
				printf("mflo %s\n", r1);
				PutInMemory(r1, op[0]->str_val);
				break;
			case(ZEROMINUS_CODE):
//				printf("%s := #0 - %s\n", op[0]->str_val, op[1]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[1]->str_val);
				printf("sub %s, $0, %s\n", r1, r2);
				PutInMemory(r1, op[0]->str_val);
				break;
			case(GOTO_CODE):
//				printf("GOTO %s\n", op[0]->str_val);
				printf("j %s\n", op[0]->str_val);
				break;
			case(IF_CODE):
//				printf("IF %s %s %s GOTO %s\n", op[0]->str_val, op[1]->str_val, op[2]->str_val, op[3]->str_val);
				r1 = GetReg(op[0]->str_val);
				r2 = GetReg(op[2]->str_val);
				opt = op[1]->str_val;
				if(strlen(opt) == 1) {
					if(opt[0] == '>') printf("bgt ");
					else printf("blt ");
				} else {
					if(opt[0] == '=') printf("beq ");
					else if(opt[0] == '!') printf("bne ");
					else if(opt[0] == '>') printf("bge ");
					else printf("ble ");
				}
				printf("%s, %s, %s\n", r1, r2, op[3]->str_val);
				break;
			case(RETURN_CODE):
//				printf("RETURN %s\n", op[0]->str_val);
				r1 = GetReg(op[0]->str_val);
				printf("move $sp, $fp\n");
				printf("lw $fp, 0($sp)\n");
				printf("addi $sp, $sp, 4\n");
				printf("move $v0, %s\n", r1);
				printf("jr $ra\n");
				break;
			case(DEC_CODE):
//				printf("DEC %s %s\n", op[0]->str_val, op[1]->str_val);
				r1 = GetReg("");
				cur_addr += atoi(op[1]->str_val);
				printf("addi %s, $fp, -%d\n", r1, cur_addr);
				cur_addr += 4;
				printf("sw %s, -%d($fp)\n", r1, cur_addr);
				address[hash(op[0]->str_val)] = -cur_addr;
				break;
			case(ARG_CODE):
//				printf("ARG %s\n", op[0]->str_val);
				op1 = NewOperand(op[0]->str_val);
				list_add_after(&arg_list, &op1->list);
				break;
			case(CALL_CODE):
//				printf("%s := CALL %s\n", op[0]->str_val, op[1]->str_val);
				
				printf("addi $sp, $sp, -%d\n", min(cur_param, 4) * 4 + cur_addr);
				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("sw $a%d, %d($sp)\n", i, i * 4);
				}
				ListHead * p1, * p2;
				list_foreach_safe(p1, p2, &arg_list) {
					Operand * op1 = list_entry(p1, Operand, list);
					cur_arg ++;
					r1 = GetReg(op1->str_val);
					if(cur_arg <= 4) {
						list_del(p1);
						printf("move $a%d, %s\n", cur_arg - 1, r1);
					}
				}
				for(p1 = arg_list.prev; p1 != &arg_list; p1 = p1->prev) { 
					Operand * op1 = list_entry(p1, Operand, list);
					r1 = GetReg(op1->str_val);
					printf("addi $sp, $sp, -4\n");
					printf("sw %s, 0($sp)\n", r1);
				}

				printf("addi $sp, $sp, -%d\n", 4);
				printf("sw $ra, 0($sp)\n");
				printf("jal %s\n", op[1]->str_val);
				printf("lw $ra, 0($sp)\n");
				printf("addi $sp, $sp, %d\n", 4);
				printf("addi $sp, $sp, %d\n", 4 * (max(cur_arg, 4) - 4));
		
				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("lw $a%d, %d($sp)\n", i, i * 4);
				}
				printf("addi $sp, $sp, %d\n", min(cur_param, 4) * 4 + cur_addr);

				r1 = GetReg(op[0]->str_val);
				printf("move %s, $v0\n", r1);
				PutInMemory(r1, op[0]->str_val);
				cur_arg = 0;
				list_init(&arg_list);
				break;
			case(PARAM_CODE):
//				printf("PARAM %s\n", op[0]->str_val);
				cur_param ++;
				if(cur_param <= 4) param[hash(op[0]->str_val)] = cur_param;
				else address[hash(op[0]->str_val)] = 8 + 4 * (cur_param - 5);
				break;
			case(READ_CODE):
//				printf("READ %s\n", op[0]->str_val);

				r1 = GetReg(op[0]->str_val);

				printf("addi $sp, $sp, -%d\n", min(cur_param, 4) * 4 + cur_addr);
				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("sw $a%d, %d($sp)\n", i, i * 4);
				}

				printf("addi $sp, $sp, -4\nsw $ra, 0($sp)\njal read\nlw $ra, 0($sp)\naddi $sp, $sp, 4\nmove %s, $v0\n"
					, r1);
				PutInMemory(r1, op[0]->str_val);
		
				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("lw $a%d, %d($sp)\n", i, i * 4);
				}
				printf("addi $sp, $sp, %d\n", min(cur_param, 4) * 4 + cur_addr);


				break;
			case(WRITE_CODE):
//				printf("WRITE %s\n", op[0]->str_val);
				printf("addi $sp, $sp, -%d\n", min(cur_param, 4) * 4 + cur_addr);
				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("sw $a%d, %d($sp)\n", i, i * 4);
				}

				r1 = GetReg(op[0]->str_val);
				printf("move $a0, %s\naddi $sp, $sp, -4\nsw $ra, 0($sp)\njal write\nlw $ra, 0($sp)\naddi $sp, $sp, 4\n"
					, r1);

				for(i = 0; i < min(cur_param, 4); ++ i) {
					printf("lw $a%d, %d($sp)\n", i, i * 4);
				}
				printf("addi $sp, $sp, %d\n", min(cur_param, 4) * 4 + cur_addr);

				break;
		}
	}
}

int cur_reg = 0;
char *GetReg(char *id) {
	char * s = malloc(32);
	if(!param[hash(id)]) {
		sprintf(s, "$t%d", cur_reg);
		cur_reg = (cur_reg + 1) % 8;
		if(strlen(id) == 0) return s;
		if(id[0] == '*') {
			printf("lw %s, %d($fp)\n", s, GetAddress(&id[1]));
			printf("lw %s, 0(%s)\n", s, s);
		} else if(id[0] == '&') {
			printf("lw %s, %d($fp)\n", s, GetAddress(&id[1]));
		} else {
			printf("lw %s, %d($fp)\n", s, GetAddress(id));
		}
	} else {
		sprintf(s, "$a%d", param[hash(id)]-1);
	}
	return s;
}

void PutInMemory(char * reg, char * id){
	if(!param[hash(id)]) {
		if(id[0] == '*') {
			char * r1 = GetReg("");
			printf("lw %s, %d($fp)\n", r1, GetAddress(&id[1]));
			printf("sw %s, 0(%s)\n", reg, r1);
		} else printf("sw %s, %d($fp)\n", reg, GetAddress(id));
	}
};

int GetAddress(char * id) {
	if(address[hash(id)] != 0) return address[hash(id)];
	cur_addr += 4;
	address[hash(id)] = -cur_addr;
	return -cur_addr;
}
