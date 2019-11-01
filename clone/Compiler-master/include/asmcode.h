#ifndef COMPILER_ASMCODE_H_
#define COMPILER_ASMCODE_H_

#include "intercode.h"

void TranslateAsm();
char *GetReg(char *);
void PutInMemory(char *, char *);
int GetAddress(char *);

#endif // COMPILER_ASMCODE_H_
