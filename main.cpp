#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define DEBUG
#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#endif
#define BASE 4
#define POISON -666
#define CANARY -6666
#ifndef COLORS_H__
#define COLORS_H__

#define RED    "\x1B[31m"
#define GREEN  "\x1B[32m"
#define WHITE  "\x1B[37m"

#define PRINT_RED(  s, ...) printf (RED   s WHITE, ##__VA_ARGS__)
#define PRINT_GREEN(s, ...) printf (GREEN s WHITE, ##__VA_ARGS__)
#define PRINT_ERROR(FILE, s, ...) fprintf(stderr, RED s WHITE, ##__VA_ARGS__)

#endif //COLORS_H__

enum change {
    DECR = -1,
    INCR = 1
};

typedef double type;

enum num_error {
    p1_fried = 52, // consisitify
    p2_fried = 52,
    low_capacity = -2,
    low_size = -1,
    no_error = 0,
    bad_data = 7,
    bad_stack = 10,
};

typedef struct {
    type parrot1;
    type* data;   
    size_t size;
    size_t capacity;
    ON_DEBUG (const char* name;) // struct
    ON_DEBUG (const char* file;)
    ON_DEBUG (size_t line;)
    ON_DEBUG (num_error err;)
    type parrot2;
} stack_t;

void init              (stack_t* stk); // add stack to function name
stack_t push           (stack_t* stk, type num); 
stack_t* real          (stack_t* stk, enum change param);
stack_t pop            (stack_t* stk);
void dtor              (stack_t* stk);
num_error verify       (stack_t* stk);
ON_DEBUG(void dump     (stack_t stk, const char* file, const size_t line);)
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
    stk->parrot1 = CANARY;
    stk->parrot2 = CANARY;
    ON_DEBUG(stk->err = no_error);
    stk->data[0] = CANARY;
    stk->data[BASE - 1] = CANARY;
    for (int i = 1; i < BASE - 1; i++) {
        stk->data[i] = POISON;
    }
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }
}

stack_t push(stack_t* stk, type num) {
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }

    if (stk->size == stk->capacity) {
        stk = real (stk, INCR);
    }
    stk->data[stk->size] = CANARY; // parrot not where supposed to be
    stk->data[stk->size - 1] = num;
    stk->size++;
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }
    return *stk;
}

stack_t* real (stack_t* stk, enum change param) { // cringenaming //cringeenum
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }

    if (param == INCR) {
        stk->capacity *= 2;
    } else if (param == DECR) {
        stk->capacity /= 2;
    }
    stk->data = (type*) realloc(stk->data, stk->capacity * sizeof(type));
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }
    return stk;
}

stack_t pop (stack_t* stk) {
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }

    if (stk->size <= stk->capacity / 2) {    // change
        stk = real (stk, DECR);              //return - deleted or not?
    }
    stk->size--;
    // move parrots out of the buffer
    stk->data[stk->size - 1] = CANARY;
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }
    return *stk;
}

void dtor (stack_t* stk) {
    if (verify(stk) != no_error) {
        dump(*stk, __FILE__, __LINE__);
    }

    for (int i = 0; i < stk->size; i++) {
        stk->data[i] = POISON;
    }
    free (stk->data);
    stk->size = POISON;
    stk->capacity = POISON;
}

ON_DEBUG(void dump (stack_t stk, const char* file, const size_t line) {
    printf("Data pointer: %p\nSize: %d\nCapacity: %d\nName: %s\nFile: %s\nLine: %u\n", 
            stk.data, stk.size, stk.capacity, stk.name, file, line);
})

void print(stack_t stk) {
    for (int i = 0; i < stk.size; i++) {
        printf("%lf\t", stk.data[i]);
    }
    printf("\n");
}

num_error verify (stack_t* stk) {
    if (!stk) {
        return bad_stack;
    } else if (!stk->data) {
        return bad_data;
    } else if (stk->size < 0) {
        return low_size;
    } else if (stk->capacity < 0) {
        return low_capacity;
    } else if (stk->parrot1 != CANARY) {
        return p1_fried;
    } else if (stk->parrot2 != CANARY) {
        return p2_fried;
    }
}
static inline unsigned int murmur_32_scramble(unsigned int k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}
unsigned int murmur3_32(const unsigned char* key, size_t len, unsigned int seed)
{
	unsigned int h = seed;
    unsigned int k;
    for (size_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(int));
        key += sizeof(unsigned int);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    h ^= murmur_32_scramble(k);
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}