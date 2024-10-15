#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "stack.h" 
int main () {
    stack_t Stk = {0};
    stackInit(&Stk);
    stackPush(&Stk, 2);
    stackPrint(&Stk);
    stackPush(&Stk, 3);
    stackPrint(&Stk);
    stackPop(&Stk);
    stackPrint(&Stk);
    stackDtor(&Stk);
    return 0;
}

