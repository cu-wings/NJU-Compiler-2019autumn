#include "asmcode.h"

FunctionBlock* createFunctionBlock(FunctionBlock* lastFunc)
{
    FunctionBlock* temp = lastFunc;
    if(!temp)
    {
        temp = (FunctionBlock*)malloc(sizeof(FunctionBlock));
        temp->blockList = NULL;
        temp->next = NULL;
        temp->varList = NULL;
        temp->offset = 0;
        return temp;
    }
    else
    {
        FunctionBlock* newBlock = (FunctionBlock*)malloc(sizeof(FunctionBlock));
        newBlock->blockList = NULL;
        newBlock->next = NULL;
        newBlock->varList = NULL;
        newBlock->offset = 0;
        temp->next = newBlock;
        temp = newBlock;
        return newBlock;
    }
}

BasicBlock* createBasicBlock(InterCodes* start, BasicBlock* lastBlock)
{
    BasicBlock* temp = lastBlock;
    if(!temp)
    {
        temp = (BasicBlock*)malloc(sizeof(BasicBlock));
        temp->head = start;
        temp->vars = NULL;
        temp->tail = NULL;
        temp->next = NULL;
        return temp;
    }
    else
    {
        BasicBlock* newBlock = (BasicBlock*)malloc(sizeof(BasicBlock));
        newBlock->head = start;
        newBlock->tail = NULL;
        newBlock->vars = NULL;
        newBlock->next = NULL;
        temp->next = newBlock;
        temp = newBlock;
        return newBlock;
    }
}

void endBasicBlock(InterCodes* end, BasicBlock* lastBlock)
{
    if(lastBlock)
        lastBlock->tail = end;
}


bool sameOp(Operand op1, Operand op2)
{
    if(op1.kind == op2.kind)
    {
        switch (op1.kind)
        {
        case TEMPVAR:
            if(op1.u.no == op2.u.no)
                return true;
            break;
        case VARIABLE:
            if(op1.u.var.no == op2.u.var.no)
                return true;
            break;
        default:
            break;
        }
    }
    return false;
}

VarInBlock* addBlockVar(VarInBlock** vars, Operand op, int offset)
{
    if(!(*vars))   //vars == NULL
    {
        *vars = (VarInBlock*)malloc(sizeof(VarInBlock));
        (*vars)->next = NULL;
        (*vars)->useLine = NULL;
        (*vars)->var = (VarDescripter*)malloc(sizeof(VarDescripter));
        (*vars)->var->op = op;
        (*vars)->var->reg = NULL;
        (*vars)->var->offset = offset;
        return (*vars);
    }
    else
    {
        VarInBlock* tempVar = (*vars);
        while(tempVar->next)
            tempVar = tempVar->next;
        VarInBlock* newVar = (VarInBlock*)malloc(sizeof(VarInBlock));
        newVar->useLine = NULL;
        newVar->next = NULL;
        newVar->var = (VarDescripter*)malloc(sizeof(VarDescripter));
        newVar->var->op = op;
        newVar->var->reg = NULL;
        newVar->var->offset = offset;
        
        tempVar->next = newVar;
        return newVar;
    }
    
}

void addUseLine(VarInBlock* var, int line)  // add next use line of the var in block
{
    UseLine* tempLine = var->useLine;
    //add new line
    if(tempLine == NULL)
    {
        tempLine = (UseLine*)malloc(sizeof(UseLine));
        tempLine->line = line;
        tempLine->next = NULL;
        var->useLine = tempLine;
    }
    else
    {
        while(tempLine->next)
            tempLine = tempLine->next;
        UseLine* newLine = (UseLine*)malloc(sizeof(UseLine));
        newLine->line = line;
        newLine->next = NULL;
        tempLine->next = newLine;
    }
}

void addBlockVarUse(FunctionBlock* lastFunc, BasicBlock* block, Operand op, int line, int *offset)
{
    VarInBlock *temp = lastFunc->varList;
    bool funcFlag = false;
    bool blockFlag = false;
    int varOffset = *offset;
    if(op.kind == VARIABLE || op.kind == TEMPVAR)
    {
        while(temp)
        {
            if(sameOp(temp->var->op, op))
            {
                funcFlag = true;
                VarInBlock* vars = block->vars;
                varOffset = temp->var->offset;
                while(vars)
                {
                    if(sameOp(vars->var->op, op))
                    {
                        blockFlag = true;
                        addUseLine(vars, line);
                    }
                    vars = vars->next;
                }
                break;
            }
            temp = temp->next;
        }
        if(!blockFlag)   // no same op, add it to block var list
        {
            //if(op.kind == VARIABLE)
              //  *offset = *offset + getTypeSize(getType(op.u.var.name));
            //else
            if(!funcFlag)   // no same op in function, add it to function var list
            {
                *offset = *offset + 4;// get correct offset
                varOffset = *offset;
                addBlockVar(&(lastFunc->varList), op, *offset);
            }
            if(ASMDEBUG) printf("op = %d, no = %d, offset:%d\n", op.kind, op.u.no, varOffset);
            VarInBlock* newVar = addBlockVar(&(block->vars), op, varOffset);
            addUseLine(newVar, line);
        }

    }
}

void initRegs()
{
    for(int i = 0;i < REGNUM;i++)
        regs[i] = false;
    //v0 = &regs[2];
    v1 = &regs[3];
    //a0 = &regs[4];
    t8 = &regs[24];
    t9 = &regs[25];
}

void initBlock()
{
    //varList = (VarDescripter*)malloc(sizeof(VarDescripter)*(tempvarNum + varNum - 2));
    FunctionBlock* lastFunc;
    lastFunc = functions = NULL;
    BasicBlock* lastBlock;
    lastBlock = firstBlock = NULL;
    int offset = 0; //init offset
    int line = 0;   //the line of block
    InterCodes* temp = codeHead->next;
    InterCodes* head = temp;    //head of a block
    bool start = true;
    while(temp != codeHead)
    {
        InterCode code = temp->code;
        /*if(start == true)
        {
            lastFunc = createFunctionBlock(lastFunc);
            if(functions == NULL)
                functions = lastFunc;
            start = false;
        }*/
        if(line == 0)   //create function block
        {
            lastBlock = createBasicBlock(temp, lastBlock);
        }
        line++;
        switch (code.kind)
        {
        case ASSIGN: case ADDR: case VALUE: case VALTOVAL: 
            addBlockVarUse(lastFunc, lastBlock, code.u.twoOp.left, line, &offset);
            addBlockVarUse(lastFunc, lastBlock, code.u.twoOp.right, line, &offset);
            break;
        case CALL:
            addBlockVarUse(lastFunc, lastBlock, code.u.twoOp.left, line, &offset);
            addBlockVarUse(lastFunc, lastBlock, code.u.twoOp.right, line, &offset);
            endBasicBlock(temp, lastBlock);
            line = 0;
            break;
        case DEC:       // create a variable's size not 4
            offset = offset + code.u.twoOp.right.u.value - 4;
            addBlockVarUse(lastFunc, lastBlock, code.u.twoOp.left, line, &offset);
            break;
        case ADD: case SUB: case MUL: case DIVIDE:
            addBlockVarUse(lastFunc, lastBlock, code.u.threeOp.op1, line, &offset);
            addBlockVarUse(lastFunc, lastBlock, code.u.threeOp.op2, line, &offset);
            addBlockVarUse(lastFunc, lastBlock, code.u.threeOp.result, line, &offset);
            break;
        case LABEL:
            //createBasicBlock(temp, lastBlock);
            //endBasicBlock(temp->prev, lastBlock);
            if(line != 1)
            {
                lastBlock = createBasicBlock(temp, lastBlock);
                /*if(firstBlock == NULL)
                    firstBlock = lastBlock;*/
                line = 1;
            }
            //continue;
            break;
        case FUNC:
            //if(!start)
                //endBasicBlock(temp->prev, lastBlock);
            if(lastFunc)    // has created function before
                lastFunc->offset = offset;
            offset = 0;
            lastFunc = createFunctionBlock(lastFunc);
            if(functions == NULL)
                functions = lastFunc;
            lastFunc->blockList = lastBlock;
            line = 1;
            //start = false;
            break;
        case GOTO:
            endBasicBlock(temp, lastBlock);
            line = 0;
            break;
        case IFGOTO:
            addBlockVarUse(lastFunc, lastBlock, code.u.ifGoto.op1, line, &offset);
            addBlockVarUse(lastFunc, lastBlock, code.u.ifGoto.op2, line, &offset);
            endBasicBlock(temp, lastBlock);
            line = 0;
            break;
        case RET:
            addBlockVarUse(lastFunc, lastBlock, code.u.oneOp.op, line, &offset);
            endBasicBlock(temp, lastBlock);
            line = 0;
            /*{
                lastFunc->offset = offset;
                if(ASMDEBUG) printf("func offset:%d\n", offset);
                line = 0;
                start = true;
                offset = 0;
            }*/
            break;
        case ARG: case PARAM: case READ: case WRITE:
            addBlockVarUse(lastFunc, lastBlock, code.u.oneOp.op, line, &offset);
            break;
        default:
            break;
        }
        if(temp->next->code.kind == FUNC || temp->next->code.kind == LABEL)
        {    
            endBasicBlock(temp, lastBlock);
            if(temp->next->code.kind == FUNC)
                lastBlock = NULL;
            line = 0;
        }
        temp = temp->next;
    }
    lastFunc->offset = offset;
}

void testOut(InterCodes* temp,FILE* fp)
{   
        switch (temp->code.kind)
        {
        case ASSIGN:
            printOp(temp->code.u.twoOp.left, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.twoOp.right, fp);
            fprintf(fp, "\n");
            break;
        case ADD:
            printOp(temp->code.u.threeOp.result, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.threeOp.op1, fp);
            fprintf(fp, " + ");
            printOp(temp->code.u.threeOp.op2, fp);
            fprintf(fp, "\n");
            break;
        case SUB:
            printOp(temp->code.u.threeOp.result, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.threeOp.op1, fp);
            fprintf(fp, " - ");
            printOp(temp->code.u.threeOp.op2, fp);
            fprintf(fp, "\n");
            break;
        case MUL:
            printOp(temp->code.u.threeOp.result, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.threeOp.op1, fp);
            fprintf(fp, " * ");
            printOp(temp->code.u.threeOp.op2, fp);
            fprintf(fp, "\n");
            break;
        case DIVIDE:
            printOp(temp->code.u.threeOp.result, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.threeOp.op1, fp);
            fprintf(fp, " / ");
            printOp(temp->code.u.threeOp.op2, fp);
            fprintf(fp, "\n");
            break;
        case ADDR:
            printOp(temp->code.u.twoOp.left, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.twoOp.right, fp);
            fprintf(fp, "\n");
            break;
        case VALUE:
            printOp(temp->code.u.twoOp.left, fp); 
            fprintf(fp, " := *");
            printOp(temp->code.u.twoOp.right, fp);
            fprintf(fp, "\n");
            break;
        case VALTOVAL:
            fprintf(fp, "*");
            printOp(temp->code.u.twoOp.left, fp); 
            fprintf(fp, " := ");
            printOp(temp->code.u.twoOp.right, fp);
            fprintf(fp, "\n");
            break;
        case LABEL:
            fprintf(fp, "LABEL label");
            printOp(temp->code.u.oneOp.op, fp);
            fprintf(fp, " : \n");
            break;
        case FUNC:
            fprintf(fp, "FUNCTION ");
            printOp(temp->code.u.oneOp.op, fp);
            fprintf(fp, " : \n");
            break;
        case GOTO:
            fprintf(fp, "GOTO label");
            printOp(temp->code.u.oneOp.op, fp); 
            fprintf(fp, "\n");
            break;
        case IFGOTO:
            fprintf(fp, "IF ");
            printOp(temp->code.u.ifGoto.op1, fp); 
            fprintf(fp, " %s ", temp->code.u.ifGoto.relop);
            printOp(temp->code.u.ifGoto.op2, fp);
            fprintf(fp, " GOTO label");
            printOp(temp->code.u.ifGoto.op3, fp);
            fprintf(fp, "\n");
            break;
        case RET:
            fprintf(fp, "RETURN ");
            printOp(temp->code.u.oneOp.op, fp); 
            fprintf(fp, "\n");
            break;
        case DEC:
            fprintf(fp, "DEC ");
            printOp(temp->code.u.twoOp.left, fp); 
            fprintf(fp, " %d\n",temp->code.u.twoOp.right.u.value);
            break;
        case ARG:
            fprintf(fp, "ARG ");
            printOp(temp->code.u.oneOp.op, fp); 
            fprintf(fp, " \n");
            break;
        case CALL:
            printOp(temp->code.u.twoOp.left, fp);
            fprintf(fp, " := CALL ");
            printOp(temp->code.u.twoOp.right, fp);
            fprintf(fp, "\n");
            break;
        case PARAM:
            fprintf(fp, "PARAM ");
            printOp(temp->code.u.oneOp.op, fp);
            fprintf(fp, "\n");
            break;
        case READ:
            fprintf(fp, "READ ");
            printOp(temp->code.u.oneOp.op, fp);
            fprintf(fp, "\n");
            break;
        case WRITE:
            fprintf(fp, "WRITE ");
            printOp(temp->code.u.oneOp.op, fp);
            fprintf(fp, "\n");
            break;
        default:
            break;
        }
        
    
}

void freeThisUse(VarInBlock* var)
{
    UseLine* thisUse = var->useLine;
    if(thisUse)
    {
        var->useLine = thisUse->next;
        thisUse->next = NULL;
        free(thisUse);
    }
}

void saveAll(BasicBlock* block, FILE* fp)
{
    VarInBlock* temp = block->vars;
    while(temp)
    {
        if(temp->var->reg)
        {
            fprintf(fp, "  sw $%s, -%d($fp)\n", regChar(temp->var->reg), temp->var->offset);
            //regs[regInt(regChar(temp->var->reg))] = false;
            *(temp->var->reg) = false;
            temp->var->reg = NULL;
            freeThisUse(temp);
        }
        temp = temp->next;
    }
}

char* regChar(bool* reg)
{
    for(int i = 0; i < REGNUM; i++)
    {
        if(reg == &regs[i])
        {
            char* s = (char*)malloc(sizeof(char)*32);
            if(i == 0)
                s = "zero";
            else if(i == 1)
                s = "at";
            else if(i >= 2 && i <= 3)
                sprintf(s, "v%d", i - 2);
            else if(i >= 4 && i <= 7)
                sprintf(s, "a%d", i - 4);
            else if(i >= 8 && i <= 15)
                sprintf(s, "t%d", i - 8);
            else if(i >= 16 && i <= 23)
                sprintf(s, "s%d", i - 16);
            else if(i >= 24 && i <= 25)
                sprintf(s, "t%d", i - 16);
            else if(i >= 26 && i <= 27)
                sprintf(s, "k%d", i - 26);
            else if(i == 28)
                sprintf(s, "gp");
            else if(i == 29)
                sprintf(s, "sp");
            else if(i == 30)
                sprintf(s, "fp");
            else if(i == 31)
                sprintf(s, "ra");    
            return s;
        }
    }
    return NULL;
}

int regInt(char* s)
{
    switch (s[0])
    {
    case 'z':
        return 0;
    case 'a':
        if(s[1] == 't')
            return 1;
        else
            return (s[1] - '0') + 4;
    case 'v':
        return (s[1] - '0') + 2;
    case 't':
        if(s[1] < '8')
            return (s[1] - '0') + 8;
        else
            return (s[1] - '0') + 16;
    case 's':
        if(s[1] == 'p')
            return 29;
        return (s[1] - '0') + 16;
    case 'k':
        return (s[1] - '0') + 26;
    case 'g':
        return 28;
    case 'f':
        return 30;
    case 'r':
        return 31;
    default:
        break;
    }
    return -1;
}

char* getReg(Operand op, BasicBlock* block, FILE* fp, int line)
{
    char* s = "";
    if(op.kind == VARIABLE || op.kind == TEMPVAR || op.kind == CONSTANT)
    {
        bool hasfreeReg = false;
        VarInBlock* temp = block->vars;
        int farthestUse = 0;
        VarInBlock* farthestVar = NULL;
        VarInBlock* thisVar = NULL;
        while(temp)
        {
            
            if(temp->var->reg)  // it is allocated a reg
            {
                if(temp->useLine->next)   // have next use
                {
                    if(farthestUse < temp->useLine->next->line && temp->useLine->next->line > line)
                    {
                        //use line before current line, can be replaced
                        farthestVar = temp;
                        farthestUse = temp->useLine->next->line;
                    }
                }
                else    // no next use
                {
                    farthestVar = temp;
                    farthestUse = __INT32_MAX__;
                }
            }
            if(op.kind == VARIABLE || op.kind == TEMPVAR)
            {
                if(sameOp(temp->var->op,op))    //find this variable
                {
                    thisVar = temp;
                    if(!temp->var->reg)     //not allocate reg yet
                    {
                        for(int i = 8; i < 24; i++)
                        {
                            if(!regs[i])     //have free reg
                            {
                                hasfreeReg = true;
                                regs[i] = true;
                                temp->var->reg = &regs[i];
                                s = regChar(temp->var->reg);
                                
                                if(temp->var->op.type == ADDRESS && temp->var->op.kind == VARIABLE)
                                    fprintf(fp, "  la $%s, -%d($fp)\n", s, temp->var->offset);  //get address
                                else
                                    fprintf(fp, "  lw $%s, -%d($fp)\n", s, temp->var->offset);
                                //freeThisUse(temp);
                                break;
                            }
                            //have no free reg
                            //wait until the loop finish, and find the farthest variable to be used
                        }
                    }
                    else    //has allocated reg
                    {
                        hasfreeReg = true;
                        s = regChar(temp->var->reg);
                        freeThisUse(temp);
                        //need to do nothing
                    }
                }
            }
            temp = temp->next;
        }
        if(op.kind == CONSTANT)
        {
            for(int i = 8; i < 25; i++)
            {
                if(!regs[i])     //have free reg
                {
                    hasfreeReg = true;
                    regs[i] = true;
                    s = regChar(&regs[i]);
                    fprintf(fp, "  li $%s, %d\n", s, op.u.value);
                    break;
                }
                //have no free reg
                //wait until the loop finish, and find the farthest variable to be used
            }
        }
        if(!hasfreeReg)   // have no free reg
        {
            if(op.kind == TEMPVAR || op.kind == VARIABLE)
            {
                // = &regs[regInt(regChar(farthestVar->var->reg))];
                
                s = regChar(farthestVar->var->reg);
                thisVar->var->reg = &regs[regInt(s)];
                fprintf(fp, "  sw $%s, -%d($fp)\n", s, farthestVar->var->offset);

                if(thisVar->var->op.type == ADDRESS && thisVar->var->op.kind == VARIABLE)
                    fprintf(fp, "  la $%s, -%d($fp)\n", s, thisVar->var->offset);  //get address
                else
                    fprintf(fp, "  lw $%s, -%d($fp)\n", s, thisVar->var->offset);
                
                freeThisUse(farthestVar);
                farthestVar->var->reg = NULL;
            }
            else
            {
                s = regChar(farthestVar->var->reg);
                fprintf(fp, "  li $%s, %d\n", s, op.u.value);
                farthestVar->var->reg = NULL;
            }
            
        }
    }
    
    return s;
}