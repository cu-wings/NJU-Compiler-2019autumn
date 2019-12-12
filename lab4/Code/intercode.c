#include "intercode.h"
/*
void adjust_code(InterCode code)
{
    int kind = code.kind;
    switch (kind)
    {
    case ASSIGN: case ADDR: case VALUE: case VALTOVAL: case CALL: case DEC:
        if(code.u.twoOp.left.type == ADDRESS)
        {
            if(code.u.twoOp.right.type == VAL)
            {
                code.u.twoOp.left.type = VAL;
                Operand op = new_op(TEMPVAR, VAL, tempvarNum++);
                new_code(VALTOVAL, code.u.twoOp.left, )
            }
        }
        break;
    case LABEL: case FUNC: case GOTO: case RET: case ARG: case PARAM: case READ: case WRITE:
        
        break;
    case ADD: case SUB: case MUL: case DIVIDE:
        
        break;
    case IFGOTO:

        break;
    default:
        break;
    }
}
*/
void new_code(int kind, ...)
{
    va_list args;
	va_start(args,kind);
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = kind;
    if(IRDEBUG) printf("kind = %d",code->kind);
    switch (kind)
    {
    case ASSIGN: case ADDR: case VALUE: case VALTOVAL: case CALL: case DEC:
        code->u.twoOp.left = va_arg(args, Operand);
        code->u.twoOp.right = va_arg(args, Operand);
        if(kind == DEC)
            code->u.twoOp.left.type = VAL;
        if(IRDEBUG)
        {
            if(code->u.twoOp.left.kind == TEMPVAR)
                printf(" temp = t%d", code->u.twoOp.left.u.no);
        }
        break;
    case LABEL: case FUNC: case GOTO: case RET: case ARG: case PARAM: case READ: case WRITE:
        code->u.oneOp.op = va_arg(args, Operand);
        if(IRDEBUG)
        {
            if(code->u.oneOp.op.kind == TEMPVAR)
                printf(" temp = t%d", code->u.oneOp.op.u.no);
        }
        break;
    case ADD: case SUB: case MUL: case DIVIDE:
        code->u.threeOp.result = va_arg(args, Operand);
        code->u.threeOp.op1 = va_arg(args, Operand);
        code->u.threeOp.op2 = va_arg(args, Operand);
        if(IRDEBUG)
        {
            if(code->u.threeOp.op1.kind == TEMPVAR)
                printf(" temp = t%d", code->u.threeOp.op1.u.no);
        }
        break;
    case IFGOTO:
        code->u.ifGoto.op1 = va_arg(args, Operand);
        code->u.ifGoto.relop = va_arg(args, char*);
        //code->u.ifGoto.relop = (char*)malloc(sizeof(char)*10);
        //strcpy(code->u.ifGoto.relop ,va_arg(args, char*));
        code->u.ifGoto.op2 = va_arg(args, Operand);
        code->u.ifGoto.op3 = va_arg(args, Operand);
        if(IRDEBUG) printf("label %d\n", code->u.ifGoto.op3.u.no);
        break;
    default:
        break;
    }
    if(IRDEBUG) printf("\n");
    InterCodes* temp = (InterCodes*)malloc(sizeof(InterCodes));
    temp->code = *code;
    temp->next = codeHead;
    temp->prev = codeTail;
    codeTail->next = temp;
    codeHead->prev = temp;
    codeTail = temp;
    va_end(args);
}

void addOpList(Operand op)
{
    Operands* temp = (Operands*)malloc(sizeof(Operands));
    temp->op = op;
    temp->next = opListHead;
    temp->prev = opListTail;
    opListTail->next = temp;
    opListHead->prev = temp;
    opListTail = temp;
}

void addExpList(InterCode code)
{
    InterCodes* temp = (InterCodes*)malloc(sizeof(InterCodes));
    temp->code = code;
    temp->next = expList->next;
    temp->prev = expList;
    expList->next->prev = temp;
    expList->next = temp;
}

Operand new_op(int kind,int type, ...)
{
    va_list args;
	va_start(args,type);
    Operand op;
    op.kind = kind;
    op.type = type;
    switch (kind)
    {
    case VARIABLE:
        op.u.var.no = va_arg(args, int);
        op.u.var.name = va_arg(args, char*);
        break;
    case TEMPVAR:  case LABELNUM:
        op.u.no = va_arg(args, int);
        break;
    case CONSTANT:
        op.u.value = va_arg(args, int);
        break;
    case FUNCNAME:
        op.u.funcName = va_arg(args, char*);
        break;
    default:
        break;
    }
    va_end(args);
    return op;
}

Operand* findVarOp(char* name)
{
    Operands* current = opListHead->next;
    while (current != opListHead)
    {
        if(current->op.kind == VARIABLE || current->op.kind == ADDRESS)
        {
            if(!strcmp(current->op.u.var.name,name))
            {
                return &(current->op);
            }
        }
        current = current->next;
    }
    return NULL;    //never return here
}

void initTranslate()
{
    tempvarNum = 1;     //t1,t2...
    varNum = 1;         //v1,v2...
    labelNum = 1;       //LABEL1,LABEL2...
    codeHead = (InterCodes*)malloc(sizeof(InterCodes));
    codeHead->next = codeHead->prev = codeHead;
    codeTail = codeHead;
    opListHead = (Operands*)malloc(sizeof(Operands));
    opListHead->next = opListHead->prev = opListHead;
    opListTail = opListHead;
    expList = (InterCodes*)malloc(sizeof(InterCodes));
    expList->next = expList->prev = expList;
    irerror = 0;
}

void translateTree(treeNode* treeRoot)
{
    if(!treeRoot) return;
    initTranslate();    //init global varaible
    if(treeRoot->child) //Program ->ExtDefList
    {
        if(!strcmp(treeRoot->child->name, "ExtDefList"))
        {
            translateExtDefList(treeRoot->child);
        }
    }
    if(!irerror)
        outputFile();
    else
    {
        printf("Cannot translate: Code contains variables of structure type.\n");
    }
    
}

void printOp(Operand op, FILE* fp)
{
    if(IRDEBUG) printf("kind = %d\n", op.kind);
    /*switch (op.type)
    {
    case ADDRESS:
        fprintf(fp, "&");
        break;
    default:
        break;
    }*/
    switch (op.kind)
    {
    case VARIABLE:
        if(op.type == ADDRESS)
            fprintf(fp, "&");
        fprintf(fp, "v%d", op.u.var.no);
        if(IRDEBUG) printf("v%d", op.u.var.no);
        break;
    case TEMPVAR:
        if(op.type == ADDRESS)
            fprintf(fp, "*");
        fprintf(fp, "t%d", op.u.no);
        if(IRDEBUG) printf("t%d", op.u.no);
        break;
    case CONSTANT:
        fprintf(fp, "#%d", op.u.value);
        if(IRDEBUG) printf("#%d", op.u.value);
        break;
    /*case ADDRESS:
        fprintf(fp, "&v%d", op.u.var.no);
        if(IRDEBUG) printf("&v%d", op.u.var.no);
        break; */
    case FUNCNAME:
        fprintf(fp, "%s", op.u.funcName);
        if(IRDEBUG) printf("%s", op.u.funcName);
        break;
    case LABELNUM:
        fprintf(fp, "%d", op.u.no);
        if(IRDEBUG) printf("label %d", op.u.no);
        break;
    default:
        break;
    }
}

void outputFile()
{
    FILE* fp = fopen("intercode.ir", "w");
    InterCodes* temp = codeHead->next;
    while(temp != codeHead)
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
        temp = temp->next;
    }
    fclose(fp);
}

void translateExtDefList(treeNode* root)
{
    if(!root) return;
    if(root->child)
    {
        if(!strcmp(root->child->name, "ExtDef")) //ExtDefList -> ExtDef ExtDefList
        {
            translateExtDef(root->child);
            translateExtDefList(root->child->next);
        }
    }
    //ExtDefList -> empty
}

void translateExtDef(treeNode* root)
{
    if(!root) return;
    if(!strcmp(root->child->next->name, "ExtDecList"))  //ExtDef -> Specifier ExtDecList SEMI
    {
        translateExtDecList(root->child->next);
    }
    else
    {
        if(!strcmp(root->child->next->name, "FunDec"))
        {
            if(!strcmp(root->child->next->next->name, "CompSt")) //ExtDef -> Specifier FunDec CompSt
            {
                translateFunDec(root->child->next);
                translateCompSt(root->child->next->next);
            }
        }
        else    //ExtDef -> Specifier SEMI
        {
            //empty
        }
    }
}

void translateExtDecList(treeNode* root)
{
    if(!root) return;
    if(root->child)
    { 	
        if(root->child->next) //ExtDecList -> VarDec COMMA ExtDecList
        {
            translateVarDec(root->child); 
            translateExtDecList(root->child->next->next);
        }
        else //ExtDecList -> VarDec
        { 
            translateVarDec(root->child);
        }
    }
}

void translateFunDec(treeNode* root)
{
    if(!root) return;
    Operand op = new_op(FUNCNAME, VAL, root->child->s_val);  //function declartion
    new_code(FUNC, op);
    if(!strcmp(root->child->next->next->name, "VarList"))   //FunDec -> ID LP VarList RP
    {
        translateVarList(root->child->next->next);
    }
    else    //FunDec -> ID LP RP
    {
        //empty
    }
}

void translateVarList(treeNode* root)
{
    if(!root) return;
    if(root->child->next)   // VarList -> ParamDec COMMA VarList
    {
        translateParamDec(root->child);
        translateVarList(root->child->next->next);
    }
    else    //VarList -> ParamDec
    {
        translateParamDec(root->child);
    }
}

void translateParamDec(treeNode* root)
{
    if(!root) return;
    translateVarDecParam(root->child->next);    //ParamDec -> Specifier VarDec
}

void translateVarDecParam(treeNode* root)
{
    if(!root) return;
    while(strcmp(root->child->name,"ID")) root = root->child;
    Type type = getType(root->child->s_val);
    Operand op;
    if(type->kind == ARRAY || type->kind == STRUCTVAR)
        op = new_op(VARIABLE, ADDRESS, varNum++, root->child->s_val);
    else
        op = new_op(VARIABLE, VAL, varNum++, root->child->s_val);
    addOpList(op);
    op.type = VARIABLE;
    new_code(PARAM, op);
}

void translateCompSt(treeNode* root)
{
    if(!root) return;
    if(!strcmp(root->child->next->name, "DefList")) //CompSt -> LC DefList StmtList RC
    {
        translateDefList(root->child->next);
        if(!strcmp(root->child->next->next->name, "StmtList"))  //StmtList != NULL;
            translateStmtList(root->child->next->next);
    }
    else if(!strcmp(root->child->next->name, "StmtList"))   //DefList == NULL;
        translateStmtList(root->child->next);
}

void translateStmtList(treeNode *root)
{
    if(!root) return;
    if(root->child) 
    {
        if(!strcmp(root->child->name, "Stmt")) //StmtList -> Stmt StmtList
        {
            translateStmt(root->child);
            translateStmtList(root->child->next);
        }
        else{
        //StmtList -> empty
        }
    }
}

void translateDefList(treeNode* root)
{
    if(!root) return;
    translateDef(root->child);
    translateDefList(root->child->next);
}

void translateDef(treeNode* root)
{
    if(!root) return;
    translateDecList(root->child->next);
}

void translateDecList(treeNode* root)
{
    if(!root) return;
    translateDec(root->child);
    if(root->child->next)   //DecList -> Dec COMMA DecList
    {
        translateDecList(root->child->next->next);
    }
}

void translateDec(treeNode* root)
{
    if(!root) return;
    if(root->child->next == NULL)  
    {
		translateVarDec(root->child);
	} 
    else    //Dec -> VarDec ASSIGNOP Exp
    {
        Operand op1 = translateVarDec(root->child);
		Operand op2 = translateExp(root->child->next->next);
        new_code(ASSIGN, op1, op2);
    }
}

Operand translateVarDec(treeNode* root)
{
    Operand op;
    if(!root) return op;
    if(root->child)
    {
        if(!strcmp(root->child->name, "ID")) //VarDec -> ID
        {
            op = new_op(VARIABLE, VAL, varNum++, root->child->s_val);
            addOpList(op);
        }
        else  //VarDec -> VarDec LP INT RP
        {
            Operand op2;
            root = root->child->child;
            while(strcmp(root->name, "ID")) 
                root = root->child; //stop when VarDec -> ID
            op = new_op(VARIABLE, VAL, varNum++, root->s_val);
            addOpList(op);
            int size = getTypeSize(getType(op.u.var.name));
            op2 = new_op(CONSTANT, VAL, size);
            new_code(DEC, op, op2);
        }
    }
    return op;
}

int getTypeSize(Type type)
{
    int size = 0;
    Type temp = type;
    FieldList f;
    switch (temp->kind)
    {
    case ARRAY:
        size = temp->u.array.size;
        temp = temp->u.array.elem;
        size *= getTypeSize(temp);
        break;
    case STRUCTVAR:
        temp = temp->u.structure->type;
        size = getTypeSize(temp);
        break;
    case STRUCTURE:
        f = temp->u.structure;
        while(f)
        {
            size += getTypeSize(f->type);
            f = f->tail; 
        }
        break;
    case BASIC:
        size = 4;
        break;
    default:
        break;
    }
    return size;
}

void translateArgs(treeNode* root, FieldList field)
{
    if(!root) return;
    Operand op = translateExp(root->child);
    if(field->type->kind == ARRAY || field->type->kind == STRUCTVAR)
    {
        if(op.type == VAL)
            op.type = ADDRESS;
        else
            op.type = VAL;
    }
    if(root->child->next)   //Args -> Exp COMMA Args
    {
        translateArgs(root->child->next->next, field->tail);    
    }
    else    //Args -> Exp
    {
        
    }
    new_code(ARG, op);
}