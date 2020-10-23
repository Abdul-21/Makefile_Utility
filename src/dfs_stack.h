#ifndef __DFS_STACK_H
#define __DFS_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "graph.h"

//Implement a stack for DFS

typedef struct stack_t{
  int size;
  char **targets;
}stack_t;

stack_t *createStack();
void push();
char* pop();
char* peek();
target findblock();
target* getdep();
void updateblock();
int parsemakefile();
#endif
