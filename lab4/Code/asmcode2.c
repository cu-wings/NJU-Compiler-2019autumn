#include "asmcode.h"

void testVar(FILE* fp, VarInBlock* vars)
{
    VarInBlock* temp = vars;
    fprintf(fp, "block:\n");
    while (temp)
    {
        switch (temp->var->op.kind)
        {
        case VARIABLE:
            fprintf(fp, "op = v%d,", temp->var->op.u.var.no);
            break;
        case TEMPVAR:
            fprintf(fp, "op = t%d,", temp->var->op.u.no);
            break;
        default:
            break;
        }
        fprintf(fp, "offset:%d ",temp->var->offset);
        UseLine* use = temp->useLine;
        fprintf(fp, "use: ");
        while(use)
        {
            fprintf(fp, "%d ",use->line);
            use = use->next;
        }
        fprintf(fp, "\n");
        temp = temp->next;
    }
    
}

void saveRegsA(FILE* fp, int arg_num)
{
    if(arg_num > 4)
        arg_num = 4;
    for(int i = 0; i < arg_num; i++)
    {
        fprintf(fp, "  addi $sp, $sp, -4\n");
        fprintf(fp, "  sw $a%d 0($sp)\n", i);
    }
}

void loadRegsA(FILE* fp, int arg_num)
{
    if(arg_num > 4)
        arg_num = 4;
    for(int i = 0; i < arg_num; i++)
    {
        fprintf(fp, "  lw $a%d 0($sp)\n", arg_num - i - 1);
        fprintf(fp, "  addi $sp, $sp, 4\n");
    }
}

void clearArray(FILE* fp, BasicBlock* currentBlock, InterCodes* temp, char* reg1, char* reg2, char* reg3)
{
    InterCode code = temp->code;
    switch (code.kind)//ASSIGN, ADD, SUB, MUL, DIVIDE, ADDR, VALUE, VALTOVAL, LABEL, FUNC, GOTO, IFGOTO, RET, DEC, ARG, CALL, PARAM, READ, WRITE
    {
    case ASSIGN:
        if(code.u.twoOp.left.type == ADDRESS)
        {
            fprintf(fp, "  sw $%s, %d\n", reg1, code.u.twoOp.right.u.value);
        }
        
        break;
    case ADD:
        break;
    case SUB:
        
        break;
    case MUL:
        break;
    case DIVIDE:
        
        break;
    case GOTO:

        break;
    case CALL:
        
        break;
    case RET:
        break;
    case IFGOTO:
        
        break;
    case VALTOVAL:
        break;
    case LABEL:
        
        break;
    case FUNC:
        
        break;
    case ARG:
        
        break;      
    case READ:
        
        break;
    case WRITE:
        
        break;
    case DEC:
        
        break;
    case PARAM:
        
        break;
    default:
        break;
    }
}

int getArgNum(char* name)
{
    int ret = 0;
    Type func = getType(name);
    FieldList list =  func->u.function.param;
    while(list)
    {
        ret++;
        list = list->tail;
    }
    return ret;
}

void translateAsm(char* outputName)
{
    FILE* fp = fopen(outputName, "w");
    fprintf(fp,".data\n_prompt: .asciiz \"Enter an integer: \"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n"
    "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n"
    "write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
    initRegs();
    initBlock();
    FILE* testFp = fopen("testOut.ir", "w");
    FILE* testBlockVar = fopen("testVar.txt", "w");
    FunctionBlock* currentFunc = functions;
    if(ASMDEBUG) printf("func offset:%d\n", functions->varList->var->offset);
    while(currentFunc)
    {
        BasicBlock* currentBlock = currentFunc->blockList;
        if(ASMDEBUG) fprintf(testBlockVar, "funcVar:\n");
        if(ASMDEBUG) testVar(testBlockVar, currentFunc->varList);
        if(ASMDEBUG) fprintf(testBlockVar, "\n");
        bool save = true;
        while(currentBlock) //travel all basic blocks
        {
            if(ASMDEBUG) testVar(testBlockVar, currentBlock->vars);
            InterCodes* temp = currentBlock->head;  //from head to tail
            int arg_num = 0;
            int line = 1;
            while(temp != currentBlock->tail->next)
            {
                if(ASMDEBUG) testOut(temp, testFp);
                InterCode code = temp->code;
                char *reg1, *reg2, *reg3;
                switch (code.kind)//ASSIGN, ADD, SUB, MUL, DIVIDE, ADDR, VALUE, VALTOVAL, LABEL, FUNC, GOTO, IFGOTO, RET, DEC, ARG, CALL, PARAM, READ, WRITE
                {
                case ASSIGN:
                    reg1 = getReg(code.u.twoOp.left, currentBlock, fp, line);
                    char* leftReg = reg1;
                    if(code.u.twoOp.left.type == ADDRESS && code.u.twoOp.left.kind == TEMPVAR)
                        leftReg = regChar(v1);
                    if(code.u.twoOp.right.kind == CONSTANT)
                    {
                        fprintf(fp, "  li $%s, %d\n", leftReg, code.u.twoOp.right.u.value);
                    }
                    else
                    {
                        fprintf(fp, "  move $%s, $%s\n", leftReg, getReg(code.u.twoOp.right,currentBlock,fp, line));
                    }
                    if(code.u.twoOp.left.type == ADDRESS && code.u.twoOp.left.kind == TEMPVAR)
                        fprintf(fp, "  sw $%s, 0($%s)\n", leftReg, reg1);
                    break;
                case ADD:
                    if(code.u.threeOp.op1.kind != CONSTANT && code.u.threeOp.op2.kind != CONSTANT)
                    {
                        fprintf(fp, "  add $%s, $%s, $%s\n", getReg(code.u.threeOp.result, currentBlock, fp, line), getReg(code.u.threeOp.op1, currentBlock, fp, line), getReg(code.u.threeOp.op2, currentBlock, fp, line));
                    }
                    else if(code.u.threeOp.op1.kind == CONSTANT && code.u.threeOp.op2.kind != CONSTANT)
                    {
                        fprintf(fp, "  addi $%s, %d, $%s\n", getReg(code.u.threeOp.result, currentBlock, fp, line), code.u.threeOp.op1.u.value, getReg(code.u.threeOp.op2, currentBlock, fp, line));
                    }
                    else if(code.u.threeOp.op1.kind != CONSTANT && code.u.threeOp.op2.kind == CONSTANT)
                    {
                        fprintf(fp, "  addi $%s, $%s, %d\n", getReg(code.u.threeOp.result, currentBlock, fp, line), getReg(code.u.threeOp.op1, currentBlock, fp, line), code.u.threeOp.op2.u.value);
                    }
                    else
                    {
                        fprintf(fp, "  addi $%s, %d, %d\n", getReg(code.u.threeOp.result, currentBlock, fp, line), code.u.threeOp.op1.u.value, code.u.threeOp.op2.u.value);
                    }     
                    break;
                case SUB:
                    if(code.u.threeOp.op1.kind != CONSTANT && code.u.threeOp.op2.kind != CONSTANT)
                    {
                        fprintf(fp, "  sub $%s, $%s, $%s\n", getReg(code.u.threeOp.result, currentBlock, fp, line), getReg(code.u.threeOp.op1, currentBlock, fp, line), getReg(code.u.threeOp.op2, currentBlock, fp, line));
                    }
                    else if(code.u.threeOp.op1.kind == CONSTANT && code.u.threeOp.op2.kind != CONSTANT)
                    {
                        fprintf(fp, "  sub $%s, %d, $%s\n", getReg(code.u.threeOp.result, currentBlock, fp, line), code.u.threeOp.op1.u.value, getReg(code.u.threeOp.op2, currentBlock, fp, line));
                    }
                    else if(code.u.threeOp.op1.kind != CONSTANT && code.u.threeOp.op2.kind == CONSTANT)
                    {
                        fprintf(fp, "  addi $%s, $%s, %d\n", getReg(code.u.threeOp.result, currentBlock, fp, line), getReg(code.u.threeOp.op1, currentBlock, fp, line), -code.u.threeOp.op2.u.value);
                    }
                    else
                    {
                        fprintf(fp, "  addi $%s, %d, %d\n", getReg(code.u.threeOp.result, currentBlock, fp, line), code.u.threeOp.op1.u.value, -code.u.threeOp.op2.u.value);
                    }     
                    break;
                case MUL:
                    reg1 = getReg(code.u.threeOp.op1, currentBlock, fp, line);
                    reg2 = getReg(code.u.threeOp.op2, currentBlock, fp, line);
                    reg3 = getReg(code.u.threeOp.result, currentBlock, fp, line);
                    fprintf(fp, "  mul $%s, $%s, $%s\n", reg3 , reg1, reg2);
                    if(code.u.threeOp.op1.kind == CONSTANT) regs[regInt(reg1)] = false;
                    if(code.u.threeOp.op2.kind == CONSTANT) regs[regInt(reg2)] = false;
                    if(code.u.threeOp.result.kind == CONSTANT) regs[regInt(reg3)] = false;
                    break;
                case DIVIDE:
                    reg1 = getReg(code.u.threeOp.op1, currentBlock, fp, line);
                    reg2 = getReg(code.u.threeOp.op2, currentBlock, fp, line);
                    reg3 = getReg(code.u.threeOp.result, currentBlock, fp, line);
                    fprintf(fp, "  div $%s, $%s\n", reg1, reg2);
                    fprintf(fp, "  mflo $%s\n", reg3);
                    if(code.u.threeOp.op1.kind == CONSTANT) regs[regInt(reg1)] = false;
                    if(code.u.threeOp.op2.kind == CONSTANT) regs[regInt(reg2)] = false;
                    if(code.u.threeOp.result.kind == CONSTANT) regs[regInt(reg3)] = false;
                    break;
                case GOTO:
                    fprintf(fp, "  #saveAll:\n");
                    saveAll(currentBlock, fp);
                    fprintf(fp, "  #endsave\n");
                    fprintf(fp, "  j label%d\n", code.u.oneOp.op.u.no);
                    save = false;
                    break;
                case CALL:
                    //saveRegsA(fp, arg_num);
                    fprintf(fp, "  #saveAll:\n");
                    saveAll(currentBlock, fp);
                    fprintf(fp, "  #endsave\n");
                    //save = false;
                    if(strcmp("main", code.u.twoOp.right.u.funcName))
                        fprintf(fp, "  jal %s_\n", code.u.twoOp.right.u.funcName);
                    else
                        fprintf(fp, "  jal %s\n", code.u.twoOp.right.u.funcName);                        
                    fprintf(fp, "  move $%s, $v0\n", getReg(code.u.twoOp.left, currentBlock, fp, line));
                    //loadRegsA(fp, arg_num);
                    arg_num = 0;
                    break;
                case RET:
                    save = false;
                    fprintf(fp, "  move $v0, $%s\n", getReg(code.u.oneOp.op, currentBlock, fp, line));
                    fprintf(fp, "  lw $ra, 4($fp)\n");
                    fprintf(fp, "  lw $fp, 0($fp)\n");
                    fprintf(fp, "  addi $sp, $sp, %d\n", currentFunc->offset + 8);
                    fprintf(fp, "  jr $ra\n");
                    break;
                case IFGOTO:
                    reg1 = getReg(code.u.ifGoto.op1, currentBlock, fp, line);
                    reg2 = getReg(code.u.ifGoto.op2, currentBlock, fp, line);
                    fprintf(fp, "  #saveAll:\n");
                    saveAll(currentBlock, fp);
                    fprintf(fp, "  #endsave\n");
                    save = false;
                    if(!strcmp(code.u.ifGoto.relop,"=="))
                    {
                        fprintf(fp, "  beq ");
                    }
                    else if(!strcmp(code.u.ifGoto.relop,"!="))
                    {
                        fprintf(fp, "  bne ");
                    }
                    else if(!strcmp(code.u.ifGoto.relop,">"))
                    {
                        fprintf(fp, "  bgt ");
                    }
                    else if(!strcmp(code.u.ifGoto.relop,"<"))
                    {
                        fprintf(fp, "  blt ");
                    }
                    else if(!strcmp(code.u.ifGoto.relop,">="))
                        fprintf(fp, "  bge ");
                    else if(!strcmp(code.u.ifGoto.relop,"<="))
                        fprintf(fp, "  ble ");
                    fprintf(fp, "$%s, $%s, label%d\n", reg1, reg2, code.u.ifGoto.op3.u.no);
                    if(code.u.ifGoto.op1.kind == CONSTANT) regs[regInt(reg1)] = false;
                    if(code.u.ifGoto.op2.kind == CONSTANT) regs[regInt(reg2)] = false;
                    fprintf(fp, "\n");
                    break;
                case VALTOVAL:
                    if(code.u.twoOp.left.type == ADDRESS)
                    {
                        fprintf(fp, "  sw $%s, 0($%s)\n", getReg(code.u.twoOp.right, currentBlock, fp, line), getReg(code.u.twoOp.left, currentBlock, fp, line));
                    }
                    else if(code.u.twoOp.right.type == ADDRESS)
                    {
                        fprintf(fp, "  lw $%s, 0($%s)\n", getReg(code.u.twoOp.left, currentBlock, fp, line), getReg(code.u.twoOp.right, currentBlock, fp, line));
                    }
                    break;
                case LABEL:
                    fprintf(fp, "label%d:\n", code.u.oneOp.op.u.no);
                    break;
                case FUNC:
                    arg_num = getArgNum(code.u.oneOp.op.u.funcName);
                    if(strcmp("main", code.u.oneOp.op.u.funcName))
                        fprintf(fp, "%s_:\n", code.u.oneOp.op.u.funcName);
                    else
                        fprintf(fp, "%s:\n", code.u.oneOp.op.u.funcName);
                    int frameSize = 8;
                    bool saveReg[32];   //save regs of s0 - s7
                    /*for(int i = 16; i < 23; i++)
                    {
                        if(regs[i] == true)
                        {
                            saveReg[i] = true;
                            frameSize += 4;
                        }
                        else
                            saveReg[i] = false;
                    }*/
                    BasicBlock* nextFunc = currentBlock;
                    VarInBlock* vars = currentBlock->vars;
                    int maxOffset = 0;
                    /*while(nextFunc->next)
                    {
                        vars = nextFunc->vars;
                        if(vars)
                        {
                            while(vars->next)
                            {
                                //printf("%d\n",vars->var->offset);
                                maxOffset = maxOffset>vars->var->offset?maxOffset:vars->var->offset;
                                vars = vars->next;
                            }
                        }
                        if(nextFunc->next->head->code.kind == FUNC)
                        {
                            break;
                        }
                        nextFunc = nextFunc->next;
                    }*/
                    if(ASMDEBUG) printf("func offset:%d\n", currentFunc->offset);
                    frameSize += currentFunc->offset;
                    
                    fprintf(fp, "  addi $sp, $sp, -%d\n", frameSize);
                    fprintf(fp, "  sw $ra, %d($sp)\n", frameSize - 4);
                    fprintf(fp, "  sw $fp, %d($sp)\n", frameSize - 8);
                    fprintf(fp, "  addi $fp, $sp, %d\n", frameSize - 8);
                    fprintf(fp, "\n");
                    break;
                case ARG:
                    arg_num++;
                    if(arg_num > 0 && arg_num <= 4)
                    {
                        reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                        //if(code.u.oneOp.op.kind == TEMPVAR)
                        fprintf(fp, "  move $a%d, $%s\n", arg_num - 1, reg1);
                        if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                        //if(code.u.oneOp.op.kind == VARIABLE)
                            //fprintf(fp, "  move $a%d, $v%d\n", arg_num, code.u.oneOp.op.u.var.no);
                    }
                    else if(arg_num > 4)
                    {
                        fprintf(fp, "  addi $sp, $sp, -4\n");
                        //if(code.u.oneOp.op.kind == TEMPVAR)
                        reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                        fprintf(fp, "  sw $%s, 0($sp)\n", reg1);
                        if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                        //if(code.u.oneOp.op.kind == VARIABLE)
                            //fprintf(fp, "  sw $v%d, 0($sp)\n", code.u.oneOp.op.u.var.no);
                    }
                    //if(temp->next->code.kind != ARG)
                        
                    break;      
                case READ:
                    fprintf(fp, "  addi $sp, $sp, -4\n");
                    fprintf(fp, "  sw $ra, 0($sp)\n");
                    fprintf(fp, "  jal read\n");
                    fprintf(fp, "  lw $ra, 0($sp)\n");
                    fprintf(fp, "  addi $sp, $sp, 4\n");
                    reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                    fprintf(fp, "  move $%s, $v0\n", reg1);
                    if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                    fprintf(fp, "\n");
                    break;
                case WRITE:
                    reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                    fprintf(fp, "  move $a0, $%s\n", reg1);
                    if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                    fprintf(fp, "  addi $sp, $sp, -4\n");
                    fprintf(fp, "  sw $ra, 0($sp)\n");
                    fprintf(fp, "  jal write\n");
                    fprintf(fp, "  lw $ra, 0($sp)\n");
                    fprintf(fp, "  addi $sp, $sp, 4\n");
                    fprintf(fp, "\n");
                    break;
                case DEC:
                    
                    break;
                case PARAM:
                    
                    fprintf(fp, "  #PARAM:\n");
                    if(arg_num > 0 && arg_num <= 4)
                    {
                        reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                        fprintf(fp, "  move $%s, $a%d\n", reg1, arg_num - 1);
                        if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                    }
                    else if(arg_num > 4)
                    {
                        //fprintf(fp, "  addi $sp, $sp, -4\n");
                        reg1 = getReg(code.u.oneOp.op, currentBlock, fp, line);
                        fprintf(fp, "  lw $%s, %d($fp)\n", reg1, 4*(arg_num - 4) + 4);
                        if(code.u.oneOp.op.kind == CONSTANT) regs[regInt(reg1)] = false;
                    }
                    fprintf(fp, "\n");
                    arg_num--;
                    //if(temp->next->code.kind != PARAM)
                        //arg_num = 0;
                    break;
                default:
                    break;
                }

                line++;
                temp = temp->next;
            }
            /*while(temp != currentBlock->tail->next)
            {
                VarInBlock* var = currentBlock->vars;
                while(var->next)
                {
                    printf("%s %s\n",var->var->op, var->useLine);
                    var = var->next;
                }
                temp = temp->next;
            }*/
            if(save)
            {
                fprintf(fp, "  #saveAll:\n");
                saveAll(currentBlock, fp);
                fprintf(fp, "  #endsave\n");
            }
            save = true;
            if(ASMDEBUG) printf("code = %d\n",currentBlock->tail->code.kind);
            currentBlock = currentBlock->next;
        }
        currentFunc = currentFunc->next;
    }
    fclose(fp);
}