#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define DEBUG
#ifdef DEBUG
    #define ON_DEBUG(code) code
#endif
#define BASE 4
#define POISON -666
#ifndef COLORS_H__
#define COLORS_H__

#define RED    "\x1B[31m"
#define GREEN  "\x1B[32m"
#define WHITE  "\x1B[37m"

#define PRINT_RED(  s, ...) printf (RED   s WHITE, ##__VA_ARGS__)
#define PRINT_GREEN(s, ...) printf (GREEN s WHITE, ##__VA_ARGS__)

#endif //COLORS_H__

enum change {
    DECR = -1,
    INCR = 1
};

typedef double type;

enum num_error {
    low_size = -1,
};

typedef struct {
    type* data;   // add status of existing stack
    size_t size;
    size_t capacity;
    ON_DEBUG (const char* name;)
    ON_DEBUG (const char* file;)
    ON_DEBUG (size_t line;)
} stack_t;

void init              (stack_t* stk);
stack_t push           (stack_t* stk, type num); 
stack_t* real          (stack_t* stk, enum change param);
stack_t pop            (stack_t* stk);
void dtor              (stack_t* stk);
void verify            (stack_t stk);
ON_DEBUG(void dump     (stack_t stk);)
void print             (stack_t stk);

int main () {
    stack_t stk = {};
    init(&stk);
    push(&stk, 2);
    print(stk);
    push(&stk, 3);
    print(stk);
    pop(&stk);
    print(stk);
    dtor(&stk);
    return 0;
}

void init(stack_t* stk) {
    stk->data = (type*) calloc(BASE, sizeof(type));
    stk->size = 0;
    stk->capacity = BASE;
    for (int i = 0; i < BASE; i++) {
        stk->data[i] = POISON;
    }
    dump(*stk);
}

stack_t push(stack_t* stk, type num) {
    if (stk->size == stk->capacity) {
        stk = real (stk, INCR);
    }
    stk->data[stk->size] = num;
    stk->size++;
    dump(*stk);
    return *stk;
}

stack_t* real (stack_t* stk, enum change param) {
    if (param == INCR) {
        stk->capacity *= 2;
    } else if (param == DECR) {
        stk->capacity /= 2;
    }
    stk->data = (type*) realloc(stk->data, stk->capacity * sizeof(type));
    printf("meow");
    dump(*stk);
    return stk;
}

stack_t pop (stack_t* stk) {
    if (stk->size == stk->capacity / 2) {
        stk = real (stk, DECR);              //return - deleted or not?
    }
    stk->size--;

    stk->data[stk->size] = POISON;
    dump(*stk);
    return *stk;
}

void dtor (stack_t* stk) {
    assert(stk);
    for (int i = 0; i < stk->size; i++) {
        stk->data[i] = POISON;
    }
    free (stk->data);
    stk->size = POISON;
    stk->capacity = POISON;
    dump(*stk);
}

ON_DEBUG(void dump (stack_t stk) {
    printf("Data pointer: %p\nSize: %d\nCapacity: %d\nName: %s\nFile: %s\nLine: %u\n", 
            stk.data, stk.size, stk.capacity, stk.name, __FILE__, __LINE__);
})

void print(stack_t stk) {
    for (int i = 0; i < stk.size; i++) {
        printf("%lf\t", stk.data[i]);
    }
    printf("\n");
}