#ifndef FUNC_H__
#define FUNC_H__



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define DEBUG
#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#endif
#define SIZE_CANARY 8
#define BASE_SIZE 4
#define POISON -666
#define CANARY1 6666
#define CANARY2 6667
#define CAPACITY_MULTIPLIER 2

typedef long int type;
#define SPECIFICATOR "%ld"

enum NUM_ERROR {
    BAD_HASH_T,
    BAD_HASH_A,
    P1_FRIED_A,
    P2_FRIED_A, 
    P1_FRIED_T, 
    P2_FRIED_T,
    BAD_CAPACITY,
    BAD_SIZE,
    NO_BADS,
    BAD_STACK,
    BAD_DATA,
};

typedef struct {
    size_t parrot1;
    type* data;   
    size_t size;
    size_t capacity;
    unsigned int h_t;
    unsigned int h_a; 
    size_t parrot2;
} stack_t;

void stackInit                      (stack_t* Stk); 
stack_t stackPush                   (stack_t* Stk, type num); 
static stack_t* stackRealloc        (stack_t* Stk, size_t change);
type stackPop                       (stack_t* Stk);
void stackDtor                      (stack_t* Stk);
NUM_ERROR stackVerify               (stack_t* Stk);
void stackDump                      (stack_t Stk, const char* file, const size_t line);
void stackPrint                     (stack_t* Stk);
inline unsigned int murMurScramble  (unsigned int k);
unsigned int murMur                 (const void* ptr, size_t len);
void recalcHash                     (stack_t* Stk);

#endif //FUNC_H__