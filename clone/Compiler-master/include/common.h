#ifndef COMPILER_COMMON_H_
#define COMPILER_COMMON_H_
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

//#define DEBUG 1
#define DEBUG 0
#define str_cpy(s1, s2); s1 = (char *)malloc(strlen(s2)+1); strcpy(s1, s2);
#define newp(t) (t *)malloc(sizeof(t))

typedef char bool;
#define true 1
#define false 0

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

static char * str_cat(const char * s1, const char * s2) {
	char * s = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	sprintf(s, "%s%s", s1, s2);
	return s;
}

#endif // COMPILER_COMMON_H_
