#include "stack.h"
#include "colors.h"

void recalcHash (stack_t* Stk) {
    Stk->h_t = murMur((const unsigned char*) (uintptr_t) &Stk->parrot1, 
                      (uintptr_t) &Stk->h_t - (uintptr_t)(&Stk->parrot1 + 1));
    Stk->h_a = murMur((const unsigned char*) Stk->data, 
                      Stk->capacity * sizeof(type));
}

void stackInit(stack_t* Stk) {
    unsigned int k =  8 - (uintptr_t)(Stk->data + Stk->capacity) % 8;

    if (k != 8) {
        Stk->data = (type*) calloc(BASE_SIZE * sizeof(type) + 2 * SIZE_CANARY + k, sizeof(char));
        *((size_t*)((uintptr_t) Stk->data + SIZE_CANARY + BASE_SIZE * sizeof(type))) = 0;
        *((size_t*)((uintptr_t) Stk->data + BASE_SIZE * sizeof(type) + SIZE_CANARY + k)) = CANARY2;
    } else {
        Stk->data = (type*) calloc(BASE_SIZE * sizeof(type) + 2 * SIZE_CANARY, sizeof(char));
        *((size_t*)((uintptr_t) Stk->data + SIZE_CANARY + BASE_SIZE * sizeof(type))) = CANARY2;
    }
    

    if (!Stk->data) {
        PRINT_ERROR(stderr, "Unluck in allocating memory for Stk->data");   
    }
    Stk->size = 0;
    Stk->capacity = BASE_SIZE;

    Stk->parrot1  = CANARY1;
    Stk->parrot2  = CANARY2;
    *((size_t*)Stk->data)  = CANARY1;

    Stk->data = (type*) ((uintptr_t) Stk->data + SIZE_CANARY);

    for (int i = 0; i < BASE_SIZE; i++) {
        Stk->data[i] = POISON;
    } 

    recalcHash (Stk);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
}

stack_t stackPush(stack_t* Stk, type num) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    if (Stk->size >= Stk->capacity) {
        Stk = stackRealloc (Stk, CAPACITY_MULTIPLIER * Stk->capacity);
    }
    Stk->data[Stk->size] = num;
    Stk->size++;
    for (unsigned int i = Stk->size; i < Stk->capacity; i++) {
        Stk->data[i] = POISON;
    }

    recalcHash (Stk);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
    return *Stk;
}

stack_t* stackRealloc (stack_t* Stk, size_t change) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    Stk->capacity = change;
    unsigned int i =  8 - (uintptr_t)(Stk->data + Stk->capacity) % 8;
    Stk->data = (type*) ((uintptr_t) realloc((type*) ((uintptr_t) Stk->data - SIZE_CANARY),
                      Stk->capacity * sizeof(type) + 2 * SIZE_CANARY + i) + SIZE_CANARY);
    Stk->data[Stk->capacity] = CANARY2;
    if (i != 8) {
        Stk->data[Stk->capacity] = 0;
        *((size_t*) ((char*) (&Stk->data[Stk->capacity]) + i)) = CANARY2;
    }

    recalcHash (Stk);

    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }
    return Stk;
}

type stackPop (stack_t* Stk) {
    if (stackVerify(Stk) != NO_BADS) {
        stackDump(*Stk, __FILE__, __LINE__);
    }

    if (Stk->size * 4 <= Stk->capacity) {  
        Stk = stackRealloc (Stk, Stk->capacity / CAPACITY_MULTIPLIER);
    }
    type res = Stk->data[Stk->size - 1];
    Stk->data[Stk->size - 1] = POISON;
    Stk->size--;

    recalcHash (Stk);

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
    free ((type*) ((uintptr_t) Stk->data - SIZE_CANARY));
    Stk->size = POISON;
    Stk->capacity = POISON;
}

void stackDump (stack_t Stk, const char* file, const size_t line) {
    
    printf("Stack pointer: %p\nData pointer: %p\nSize: %d\nCapacity: %d\nFile: %s\nLine: %u\n"  
           "Left struct canary: %u\nRight struct canary: %u\nLeft array canary: %u\n"
           "Right array canary: %u\nStruct hash: %u\nArray hash: %u\n", &Stk, Stk.data, Stk.size, Stk.capacity, file, line, 
            Stk.parrot1, Stk.parrot2, *((size_t*)((uintptr_t) Stk.data - SIZE_CANARY)),
            *((size_t*)(Stk.data + Stk.capacity)), Stk.h_t, Stk.h_a);
}

void stackPrint(stack_t* Stk) {
    for (unsigned int i = 0; i < Stk->size; i++) {
        printf(SPECIFICATOR"\t", Stk->data[i]);               
    }
    printf("\n");
}

NUM_ERROR stackVerify (stack_t* Stk) {
    if (!Stk) {             
        return BAD_STACK;        
    } else if (!Stk->data) {
        return BAD_DATA;
    } else if (Stk->size > 1 << 10) {
        return BAD_SIZE;
    } else if (Stk->capacity > 1 << 10) {
        return BAD_CAPACITY;
    } else if (Stk->parrot1 != CANARY1) { 
        return P1_FRIED_T;
    } else if (Stk->parrot2 != CANARY2) {
        return P2_FRIED_T;
    } else if (Stk->h_t != murMur((const unsigned char*) (uintptr_t) &Stk->parrot1, 
                      (uintptr_t)&(Stk->h_t) - (uintptr_t)(&Stk->parrot1 + 1))) {
        return BAD_HASH_T;
    } else if (*((size_t*)((uintptr_t) Stk->data - SIZE_CANARY)) != CANARY1) {
        return P1_FRIED_A;
    } else if (*((size_t*)((uintptr_t) Stk->data + Stk->capacity * sizeof(type) )) != CANARY2) {
        return P2_FRIED_A;
    } else if (Stk->h_a != murMur((const unsigned char*) Stk->data, 
               Stk->capacity * sizeof(type))) {
        return BAD_HASH_A;
    } else {
        return NO_BADS;
    }
}

inline unsigned int murMurScramble(unsigned int k) {
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