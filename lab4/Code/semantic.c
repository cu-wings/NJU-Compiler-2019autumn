#include "semantic.h"
void initHarshTable()
{
    for(int i = 0; i < MAX_TABLE; i++)
    {
        symbolTable[i] = NULL;//= (SymbolTable)malloc(sizeof(SymbolTable_));
        /*symbolTable[i]->name = NULL;
        symbolTable[i]->line = 0;
        symbolTable[i]->next = NULL;
        symbolTable[i]->type = NULL;
        symbolTable[i]->defined = false;
        */
    }
    //lab3 add: read and write
    Type ret = (Type)malloc(sizeof(Type_));
    ret->kind = BASIC;
    ret->u.basic = 0;
    Type func = (Type)malloc(sizeof(Type_));
    func->kind = FUNCTION;
    func->u.function.param = NULL;
    func->u.function.ret = ret;
    addSymbol("read", func, 0, true);

    Type _write_param_ = (Type)malloc(sizeof(Type_));
    _write_param_->kind = BASIC;
    _write_param_->u.basic = 0;
    addSymbol("_write_param_", _write_param_, 0, true);

    Type func2 = (Type)malloc(sizeof(Type_));
    func2->kind = FUNCTION;
    FieldList param = (FieldList)malloc(sizeof(FieldList_));
    param->name = (char*)malloc(sizeof(char)*7);
    param->name = "_write_param_";
    param->tail = NULL;
    param->type = ret;
    func2->u.function.param = param;
    func2->u.function.ret = ret;
    addSymbol("write", func2, 0, true);
    //Type find = getType("write");
    //printf("%d %d\n", find->kind, find->u.function.ret->kind);
}

int symbolLine(char* name)
{
    int harshCode = harsh(name);
    SymbolTable current = symbolTable[harshCode];
    while(current)
    {
        if(!strcmp(current->name, name))
        {
            return current->line;
        }
    }
    return 0;
}

SymbolTable newSymbol(char* name, Type type, int line, bool defined)
{
    SymbolTable temp;
    temp = (SymbolTable)malloc(sizeof(SymbolTable_));
    //temp->name = (char*)malloc(sizeof(name));
    //strcpy(temp->name,name);
    temp->name = name;
    //temp->type = (Type)malloc(sizeof(Type_));
    //memcpy(temp->type,type,sizeof(Type_));
    temp->type = type;
    temp->line = line;
    temp->defined = defined;
    return temp;
}

unsigned int harsh(char* name)
{
    unsigned int val = 0;
    unsigned int i;
    for(; *name; ++name)
    {
        val = (val << 2) + *name;
        if(i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

char* stradd(char* str1, char* str2)
{
    char* msg = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(msg, str1);
    strcat(msg, str2);
    if(SEDEBUG) printf("%s\n",str2);
    return msg;
}

bool sameType(Type t1, Type t2)
{
    if(!t1 || !t2)
    {
        if(t1 == t2)
            return  true;
        else
            return false;
    }
    if(t1->kind == t2->kind)
    {
        if(t1->kind == BASIC)
        {
            if(t1->u.basic == t2->u.basic)
                return true;
            else
                return false;
        }
        else if(t1->kind == ARRAY)
        {
            if(sameType(t1->u.array.elem, t2->u.array.elem) && t1->u.array.size == t2->u.array.size)
                return true;
            else
                return false;
        }
        else if(t1->kind == STRUCTURE)  //have same name
        {
            FieldList f1 = t1->u.structure, f2 = t2->u.structure;
            /*while(f1 != NULL && f2 != NULL)
            {
                if(!sameType(f1->type, f2->type) || strcmp(f1->name,f2->name))
                    return false;
                f1 = f1->tail;
                f2 = f2->tail;
            }
            if(f1 == NULL && f2 == NULL)
                return true;
            */
            if(!strcmp(f1->name, f2->name))
                return true;
            else
                return false;
        }
        else if(t1->kind == FUNCTION)        //kind == FUNCTION
        {
            Type ret1 = t1->u.function.ret, ret2 = t2->u.function.ret;
            FieldList para1 = t1->u.function.param, para2 = t2->u.function.param;
            if(sameType(ret1, ret2))
            {
                while(para1 != NULL && para2 != NULL)
                {
                    if(!sameType(para1->type, para2->type))
                        return false;
                    para1 = para1->tail;
                    para2 = para2->tail;
                }
                if(para1 == NULL && para2 == NULL)
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else    //kind == STRUCTVAR
        {
            FieldList f1 = t1->u.structure, f2 = t2->u.structure;
            if(!strcmp(f1->name, f2->name))
                return true;
            else
                return false;
        }
    }
    else
        return false;
    //return true;
}

void addSymbol(char* name, Type type, int line, bool defined)
{
    unsigned int harshCode = harsh(name);
    if(SEDEBUG) printf("%d\n", harshCode);
    if(!type)
        return;
    if(symbolTable[harshCode] == NULL)  //the first place is empty pointer
    {
        /*symbolTable[harshCode] = (SymbolTable)malloc(sizeof(SymbolTable_));
        symbolTable[harshCode]->name = (char*)malloc(sizeof(name));
        strcpy(symbolTable[harshCode]->name,name);
        type = (Type)malloc(sizeof(Type_));
        memcpy(symbolTable[harshCode]->type,type,sizeof(Type_));
        symbolTable[harshCode]->line = line;
        symbolTable[harshCode]->defined = defined;
        */
        symbolTable[harshCode] = newSymbol(name, type, line, defined);
    }
    else
    {
        //SymbolTable pre = symbolTable[harshCode];
        SymbolTable current = symbolTable[harshCode];
        bool flag = false;  //flag means if the symbol appeared
        //if(pre)
        while(current != NULL)
        {
            if(strcmp(name,current->name) == 0)     //this symbol appeared
            {
                if(current->defined == true && defined == true)
                {
                    if((current->type->kind == ARRAY || current->type->kind == BASIC || current->type->kind == STRUCTVAR || current->type->kind == STRUCTURE)
                    && (type->kind == ARRAY || type->kind == BASIC || type->kind == STRUCTVAR))
                        serror(stradd("Redefined Variable: ", name), line, 3);
                    else if(current->type->kind == FUNCTION && type->kind == FUNCTION)
                        serror(stradd("Redefined Function: ", name), line, 4);
                    else if((current->type->kind == ARRAY || current->type->kind == BASIC || current->type->kind == STRUCTVAR || current->type->kind == STRUCTURE)
                    && (type->kind == STRUCTURE))
                        serror(stradd("DupLicated name: ", name), line, 16);
                }
                else if(current->defined == false && defined == false)  
                {
                    if(!sameType(current->type, type))
                    {
                        //two function with the same name declared but not defined
                        if(current->type->kind == FUNCTION && type->kind == FUNCTION) 
                        {
                            serror(stradd("Inconsistant declaration of function: ", name), line, 19);
                        }
                    }
                }
                else if(current->defined == false && defined == true) //this symbol declared but now defined
                {
                    if(!sameType(current->type, type))
                    {
                        //one function defined with different declaration
                        if(current->type->kind == FUNCTION && type->kind == FUNCTION)
                            serror(stradd("Inconsistant declaration of function: ", name), line, 19);
                    }
                    else    //only the param of functions will do this
                    {
                        current->defined = true;
                        flag = true;
                        break;
                    }
                }
                else
                {
                    if(!sameType(current->type, type))
                    {
                        //one function defined with different declaration
                        if(current->type->kind == FUNCTION && type->kind == FUNCTION) 
                        {
                            serror(stradd("Inconsistant declaration of function: ", name), line, 19);
                        }
                    }
                }
                flag = true;
                break;
            }
            //pre = pre->next;
            current = current->next;
        }
        if(!flag)   //this symbol didn't appear, add it to symboltable
        {
            /*SymbolTable temp = (SymbolTable)malloc(sizeof(SymbolTable_));
            temp = (SymbolTable)malloc(sizeof(SymbolTable_));
            temp->name = (char*)malloc(sizeof(name));
            strcpy(temp,name);
            type = (Type)malloc(sizeof(Type_));
            memcpy(temp->type,type,sizeof(Type_));
            temp->line = line;
            temp->defined = defined;
            */
            SymbolTable temp = newSymbol(name, type, line, defined);  
            temp->next = symbolTable[harshCode]->next;
            symbolTable[harshCode]->next = temp;
        
        }
    }
}

Type getType(char* name)
{
    unsigned int harshCode = harsh(name);
    SymbolTable temp = symbolTable[harshCode];
    if(SEDEBUG) if(temp) printf("%s\n",temp->name);
    bool flag = false;
    while(temp != NULL)
    {
        if(!strcmp(temp->name, name))
            return temp->type;
        temp = temp->next;
    }
    return NULL;
}

void serror(char* msg, int line, int errorType)
{
    serrorState = 1;
    printf("Error type %d at Line %d: %s.\n", errorType, line, msg);
}

void tranverseTree(treeNode* treeRoot)
{
    initHarshTable();
    //treeRoot->child->name
    serrorState = 0;
    if(treeRoot != NULL)
    {
        if(treeRoot->child) //Program ->ExtDefList
        {
            if(!strcmp(treeRoot->child->name, "ExtDefList"))
            {
                ExtDefList(treeRoot->child);
            }
        }
    }
    CheckFunc();
}

void CheckFunc()
{
    for(int i = 0; i < MAX_TABLE; i++)
    {
        SymbolTable temp = symbolTable[i];
        while(temp)
        {
            if(temp->type->kind == FUNCTION && temp->defined == false)
            {
                serror(stradd("Undefined function: ", temp->name), temp->line, 18);
            }
            temp = temp->next;
        }
    }
}

void ExtDefList(treeNode* root)
{
    if(SEDEBUG) printf("ExtDefList\n");
    if(root)
    {
        if(root->child)
        {
            if(!strcmp(root->child->name, "ExtDef")) //ExtDefList -> ExtDef ExtDefList
            {
                ExtDef(root->child);
                ExtDefList(root->child->next);
            }
        }
        //ExtDefList -> empty
    }
}

void ExtDef(treeNode* root)
{
    if(SEDEBUG) printf("ExtDef\n");
    if(root)
    {
        Type type = Specifier(root->child);
        if(type)
        {
            if(!strcmp(root->child->next->name, "ExtDecList"))  //ExtDef -> Specifier ExtDecList SEMI
            {
                ExtDecList(root->child->next, type);
            }
            else
            {
                if(!strcmp(root->child->next->name, "FunDec"))
                {
                    if(!strcmp(root->child->next->next->name, "CompSt")) //ExtDef -> Specifier FunDec CompSt
                    {
                        FunDec(root->child->next, type, true);
                        CompSt(root->child->next->next, type, true);
                    }
                    else    //ExtDef -> Specifier FucDec SEMI
                        FunDec(root->child->next, type, false);
                }
                else    //ExtDef -> Specifier SEMI
                {
                    //empty
                }
            }
        }
    }
}

Type Specifier(treeNode* root)
{
    if(SEDEBUG) printf("Specifier\n");
    if(root)
    {
        if(!strcmp(root->child->name, "TYPE"))  //Specifier -> TYPE
        {
            Type type = (Type)malloc(sizeof(Type_));
            type->kind = BASIC;
            if(!strcmp(root->child->s_val, "int"))
                type->u.basic = 0;
            else
                type->u.basic = 1;
            return type;
        }
        else    //Specifier -> StructSpecifier
            return StructSpecifier(root->child);
    }
    else
        return NULL;
}


void FunDec(treeNode* root, Type ret, bool defined)
{
    if(SEDEBUG) printf("FunDec\n");
    if(root)
    {
        Type func = (Type)malloc(sizeof(Type_));
        func->kind = FUNCTION;
        func->u.function.ret = ret;
        if(!strcmp(root->child->next->next->name, "VarList"))   //FunDec -> ID LP VarList RP
        {
            func->u.function.param = VarList(root->child->next->next, func);
        }
        else    //FunDec -> ID LP RP
        {
            func->u.function.param = NULL;
        }
        if(SEDEBUG) printf("%d\n",defined);
        if(defined)
        {
            FieldList temp = func->u.function.param;
            while(temp)
            {
                //unsigned int harshCode = harsh(temp->name);
                addSymbol(temp->name, temp->type, root->child->line, true);
                temp = temp->tail;
            }
        }
        addSymbol(root->child->s_val, func, root->child->line, defined);
    }
}

FieldList VarList(treeNode* root, Type headType)
{
    if(root)
    {
        FieldList temp;
        if(root->child->next)   // VarList -> ParamDec COMMA VarList
        {
            temp = ParamDec(root->child, headType);
            temp->tail = VarList(root->child->next->next, headType);
        }
        else    //VarList -> ParamDec
        {
            temp = ParamDec(root->child, headType);
            temp->tail = NULL;
        }
        return temp;
    }
    else
        return NULL;
}

FieldList ParamDec(treeNode* root, Type headType)
{
    if(root)
    {
        FieldList temp;
        Type type = Specifier(root->child);
        temp = VarDec_Structure(root->child->next, type, headType);
    }
    else
        return NULL;
}


Type StructSpecifier(treeNode* root)
{
    if(SEDEBUG) printf("StructSpecifier\n");
    //if(SEDEBUG) printf("%s\n", root->child->next->name);
    //if(SEDEBUG) if(root != NULL) printf("NULL!\n");
    if(root)
    {
        //strcmp(root->child->next->name, "OptTag");
        //if(SEDEBUG) if(root) printf("NOT NULL!\n");
        if(!strcmp(root->child->next->name, "OptTag"))
        {
            if(SEDEBUG) printf("OptTag\n");
            Type type = (Type)malloc(sizeof(Type_));
            type->kind = STRUCTURE;
            type->u.structure = NULL;
            char* name;
            if(root->child->next->child)    //OptTag -> ID
            {   
                //name = (char*)malloc(sizeof(root->child->next->child->s_val) + 1);
                //strcpy(name, root->child->next->child->s_val);
                name = root->child->next->child->s_val;
                if(SEDEBUG) printf("%s\n", name);
            }
            DefList_Structure(root->child->next->next->next, type);
            addSymbol(name, type, root->line, true);
            Type varType = (Type)malloc(sizeof(Type_));
            varType->kind = STRUCTVAR;
            varType->u.structure = (FieldList)malloc(sizeof(FieldList_));
            varType->u.structure->name = name;
            varType->u.structure->type = type;
            varType->u.structure->tail = NULL;
            return varType;
        }
        else        //StructSpecifier -> STRUCT Tag
        {
            if(SEDEBUG) printf("Tag\n");
            //if(SEDEBUG) printf("%s\n",root->child->next->child->s_val);
            Type type = (Type)malloc(sizeof(Type_));
            //type = getType(root->child->next->child->s_val);
            type->kind = STRUCTVAR;
            type->u.structure = (FieldList)malloc(sizeof(FieldList_));
            type->u.structure->name = root->child->next->child->s_val;
            type->u.structure->type = getType(root->child->next->child->s_val);
            type->u.structure->tail = NULL;
            if(type->u.structure->type == NULL)
                serror(stradd("Undefined structure: ", root->child->next->child->s_val), root->child->next->line, 17);
            return type;
        }
    }
    else
        return NULL;
}

FieldList DefList_Structure(treeNode* root, Type headType)
{
    if(SEDEBUG) printf("DefList\n");
    if(root)
    {
        FieldList temp = Def_Structure(root->child, headType);
        /*if(temp)
        {
            while(temp->tail)
            {
                temp = temp->tail;
            }
        }*/
        //temp->tail = (FieldList)malloc(sizeof(FieldList_));
        temp->tail = DefList_Structure(root->child->next, headType);
        return temp;
    }
    else
        return NULL;
}

FieldList Def_Structure(treeNode* root, Type headType)
{
    if(SEDEBUG) printf("Def\n");
    if(root)
    {
        FieldList temp; //= (FieldList)malloc(sizeof(FieldList_));
        Type type = Specifier(root->child);
        temp = DecList_Structure(root->child->next, type, headType);
        return temp;
    }
    else
        return NULL;
}

FieldList DecList_Structure(treeNode* root, Type type, Type headType)
{
    if(SEDEBUG) printf("DecList_Structure\n");
    if(root)
    {
        FieldList temp; // = (FieldList)malloc(sizeof(FieldList_));
        temp = Dec_Structure(root->child, type, headType);
        if(root->child->next)
        {
            temp->tail = DecList_Structure(root->child->next->next, type, headType);
        }
        return temp;
    }
    else
        return NULL;
}

FieldList Dec_Structure(treeNode* root, Type type, Type headType)
{
    if(root)
    {
        FieldList temp; // = (FieldList)malloc(sizeof(FieldList_));
        if(root->child->next)   //Dec -> VarDec ASSIGNOP Exp
        {
            temp = VarDec_Structure(root->child, type, headType);
            serror(stradd("Initial field: ", temp->name), root->line, 15);
        }
        else        //Dec -> VarDec
        {
            temp = VarDec_Structure(root->child, type, headType);
        }
        return temp;
    }
    else
        return NULL;
}

FieldList VarDec_Structure(treeNode* root, Type type, Type headType)
{
    if(root)
    {
        FieldList temp = (FieldList)malloc(sizeof(FieldList_));
        temp->type = type;
        temp->tail = NULL;

        treeNode* currentNode = root->child;
        while(currentNode)
        {
            if(!strcmp(currentNode->name, "ID"))
            {
                //temp->name = (char*)malloc(sizeof(currentNode->s_val));
                //strcpy(temp->name, currentNode->s_val);
                temp->name = currentNode->s_val;
                break;
            }
            /*if the VarDec is array*/
            Type tmp_type = (Type)malloc((sizeof(Type_)));
            tmp_type->kind = ARRAY;
            tmp_type->u.array.size = currentNode->next->next->i_val;
            tmp_type->u.array.elem = temp->type;
            temp->type = tmp_type;
            currentNode = currentNode->child;
        }
        if(headType->kind == FUNCTION)
            addSymbol(temp->name, temp->type, root->line, false); // the param of function are defined in FunDec
        else
        {
            FieldList tmp = headType->u.structure;
            bool flag = true;
            while(tmp)
            {
                if(temp->name && tmp->name)
                {
                    if(!strcmp(temp->name,tmp->name))
                    {
                        serror(stradd("Redefined field: ", temp->name), root->line, 15);
                        flag = false;
                    }
                }
                tmp = tmp->tail;
            }
            if(flag)
                addSymbol(temp->name, temp->type, root->line, true);  // the field of structure are defined here
            if(headType->u.structure == NULL)
                headType->u.structure = temp;
        }
        return temp;
    }
    else
        return NULL;
}