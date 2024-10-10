#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#define DEBUG
#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#endif
#define SIZE_STRUCT 48
#define SIZE_CANARY 8
#define BASE 4
#define POISON -666
#define CANARY1 -6666
#define CANARY2 6667
#ifndef COLORS_H__
#define COLORS_H__

#define RED    "\x1B[31m"
#define GREEN  "\x1B[32m"
#define WHITE  "\x1B[37m"

#define PRINT_RED(        s, ...) printf (       RED   s WHITE, ##__VA_ARGS__)
#define PRINT_GREEN(        s, ...) printf (       GREEN s WHITE, ##__VA_ARGS__)
#define PRINT_ERROR(file, s, ...) fprintf(file, RED   s WHITE, ##__VA_ARGS__) // some to do

#endif //COLORS_H__

/*enum change {
    DECR = -1,
    INCR = 1
};*/

typedef double type;

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

/*ON_DEBUG(typedef struct debug {
    const char* name;
    const char* file;
    const size_t line;
    NUM_ERROR err;
} debug_t;)*/

typedef struct {
    double parrot1;
    type* data;   
    size_t size;
    size_t capacity;
    unsigned int h_t;
    unsigned int h_a; 
    double parrot2;
} stack_t;

static void stackAlignment                        (stack_t* Stk);
static void stackInit                             (stack_t* Stk); 
static stack_t stackPush                          (stack_t* Stk, type num); 
static stack_t* stackRealloc                      (stack_t* Stk, size_t change);
static type stackPop                              (stack_t* Stk);
static void stackDtor                             (stack_t* Stk);
static NUM_ERROR stackVerify                      (stack_t* Stk);
static void stackDump                             (stack_t Stk, const char* file, const size_t line);
static void stackPrint                            (stack_t Stk);
static inline unsigned int murMurScramble         (unsigned int k);
static unsigned int murMur                        (const void* ptr, size_t len);

int main () {
    stack_t Stk = {0};
    stackInit(&Stk);
    stackPush(&Stk, 2);
    stackPrint(Stk);
    stackPush(&Stk, 3);
    stackPrint(Stk);
    stackPop(&Stk);
    stackPrint(Stk);
    stackDtor(&Stk);
    return 0;
}

static void stackAlignment (stack_t* Stk) {
    size_t i = 1;
    while((size_t) &Stk->data[Stk->capacity] % 8 != 0) {
        Stk->data[Stk->capacity] = 0;
        *((double*) ((char*) (&Stk->data[Stk->capacity]) + i)) = CANARY2;
        i++;
    }
}

void stackInit(stack_t* Stk) {
    Stk->data = (type*) calloc(BASE + 2 * SIZE_CANARY, sizeof(type));
    if (!Stk->data) {
        PRINT_ERROR(stderr, "Unluck in allocating memory for Stk->data");   
    }
    Stk->size = 0;
    Stk->capacity = BASE;
    Stk->parrot1  = CANARY1;
    Stk->parrot2  = CANARY2;

    Stk->data[0]  = CANARY1;
    Stk->data[BASE] = CANARY2;

    Stk->data = (type*) ((size_t) Stk->data + SIZE_CANARY);
    for (int i = 1; i < BASE - 1; i++) {
        Stk->data[i] = POISON;
    }

    // TODO function {
    Stk->h_t = murMur((const unsigned char*) Stk, SIZE_STRUCT);
    Stk->h_a = murMur((const unsigned char*) Stk->data - SIZE_CANARY, 
                                                  Stk->capacity * sizeof(type) + 2 * SIZE_CANARY);
    // }

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
}

stack_t stackPush(stack_t* Stk, type num) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    if (Stk->size >= Stk->capacity) {
        Stk = stackRealloc (Stk, 2 /*TODO to constant*/ * Stk->capacity);
    }
    Stk->data[Stk->size] = num;
    Stk->size++;

    Stk->h_t = murMur((const unsigned char*) Stk, SIZE_STRUCT);
    Stk->h_a = murMur((const unsigned char*) Stk->data - SIZE_CANARY,
                                                  Stk->capacity * sizeof(type) + 2 * SIZE_CANARY);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
    return *Stk;
}

stack_t* stackRealloc (stack_t* Stk, size_t change) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    Stk->capacity += change;
    Stk->data = (type*) ((size_t) realloc((type*) ((size_t) Stk->data - SIZE_CANARY),
                          Stk->capacity * sizeof(type) + 2 * SIZE_CANARY) + SIZE_CANARY);
    Stk->data[Stk->capacity] = CANARY2;

    Stk->h_t = murMur((const unsigned char*) Stk, SIZE_STRUCT);
    Stk->h_a = murMur((const unsigned char*) Stk->data - SIZE_CANARY, 
                                                  Stk->capacity * sizeof(type) + 2 * SIZE_CANARY);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
    return Stk;
}

type stackPop (stack_t* Stk) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    if (Stk->size <= Stk->capacity / 4) {  
        Stk = stackRealloc (Stk, Stk->capacity / 4);
    }
    type res = Stk->data[Stk->size - 1];
    Stk->data[Stk->size - 1] = POISON;
    Stk->size--;

    Stk->h_t = murMur((const unsigned char*) Stk, SIZE_STRUCT);
    Stk->h_a = murMur((const unsigned char*) Stk->data - SIZE_CANARY, 
                                                  Stk->capacity * sizeof(type) + 2 * SIZE_CANARY);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
    return res;
}

void stackDtor (stack_t* Stk) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    for (unsigned int i = 0; i < Stk->size; i++) {
        Stk->data[i] = POISON;
    }
    free ((type*) ((size_t) Stk->data - SIZE_CANARY));
    Stk->size = POISON;
    Stk->capacity = POISON;
}

void stackDump (stack_t Stk, const char* file, const size_t line) {
    printf("Data pointer: %p\nSize: %d\nCapacity: %d\nFile: %s\nLine: %u\n",  
            Stk.data, Stk.size, Stk.capacity, file, line);
    printf("Left struct canary: %u\nRight struct canary: %u\nLeft array canary: %d\n", Stk.parrot1, Stk.parrot2, 
            *((int*)((size_t) Stk.data - SIZE_CANARY)));
    printf("Right array canary: %d\nStruct hash: %u\nArray hash: %u\n",
            *((int*)((size_t) Stk.data + Stk.capacity)), Stk.h_t, Stk.h_a);
}

void stackPrint(stack_t Stk) {
    for (unsigned int i = 0; i < Stk.size; i++) {
        printf("%lf\t", Stk.data[i]);
    }
    printf("\n");
}

NUM_ERROR stackVerify (stack_t* Stk) { //выравнивание 
    if (!Stk->data) {
        return BAD_DATA;        
    } else if (!Stk) {
        return BAD_STACK;
    } else if (Stk->size > pow(2, 10)) {
        return BAD_SIZE;
    } else if (Stk->capacity > pow(2, 10)) {
        return BAD_CAPACITY;
    } else if (Stk->parrot1 != CANARY1) { 
        return P1_FRIED_T;
    } else if (Stk->parrot2 != CANARY2) {
        return P2_FRIED_T;
    } else if (Stk->h_t != murMur((const unsigned char*) Stk, SIZE_STRUCT)) {
        return BAD_HASH_T;
    } else if (*((type*)((size_t) Stk->data - SIZE_CANARY)) != CANARY1) {
        return P1_FRIED_A;
    } else if (*((type*)((size_t) Stk->data + Stk->capacity * sizeof(type) )) != CANARY2) {
        return P2_FRIED_A;
    } else if (Stk->h_a != murMur((const unsigned char*) Stk->data - SIZE_CANARY, 
               Stk->capacity * sizeof(type) + 2 * SIZE_CANARY)) {
        return BAD_HASH_A;
    } else {
        return NO_BADS;
    }
}

static inline unsigned int murMurScramble(unsigned int k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

unsigned int murMur(const void* ptr, size_t len) { // 
    const unsigned char* key = (const unsigned char*) ptr;
	unsigned int h = 0;
    unsigned int k;
    for (size_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(int));
        key += sizeof(unsigned int);
        h ^= murMurScramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    h ^= murMurScramble(k);
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}