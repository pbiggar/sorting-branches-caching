#ifndef _UTILS_H_
#define _UTILS_H_

#include "predictor.h"

#ifndef NULL
#define NULL 0
#endif

#define HEAP_SIZE 8

#define ALIGNMENT (HEAP_SIZE * sizeof(unsigned int))

#define LINE_BITS 5
#define LINE_MASK 0x1F
/* 32 byte line */ 

#define BLOCK_BITS 16
#define BLOCK_MASK 0xFFFF
/* 65536 blocks */

#define BLOCK_AND_LINE_BITS (BLOCK_BITS + LINE_BITS)
#define BLOCK_AND_LINE_MASK ((BLOCK_MASK << LINE_BITS) | (LINE_MASK))

/* LIMIT isnt the same as 2^(LINE_BITS+BLOCK_BITS)  */
/* it is: sizeof cache / sizeof(unsigned long) / 2) */
#define IN_PLACE_LIMIT 524288
#define IN_PLACE_LIMIT_BITS (19)

#define OUT_OF_PLACE_LIMIT 262144
#define OUT_OF_PLACE_LIMIT_BITS (18)

static inline unsigned int get_index(unsigned int* address)
{
	unsigned int a = (unsigned int)(address);
	return (a >> LINE_BITS) & BLOCK_MASK;
}

static inline unsigned int get_offset(unsigned int* address)
{
	unsigned int a = (unsigned int)(address);
	return a & LINE_MASK;
}


#define key(A) (A)
#define less(A,B) (key(A) < key(B))
#define exch(A, B) do { unsigned int t = (A); (A) = (B); (B) = t; } while(0)
#define compexch(A, B) do { if (less((B), (A))) exch((A), (B)); } while(0)

#define OUT(A) do { printf(#A ": 0x%x (%d)\n", A, A); } while(0)


int check_sorted(unsigned int a[], int l, int r);

void print_array(void* array, unsigned int count, unsigned int size, const char* description);
void print_int_array(unsigned int array[], int N, const char* description, int left, int right, int pivot);
void print_diff_array(unsigned int array[], unsigned int b[], int N, const char* description, int left, int right, int pivot);

void fill_random_array(unsigned int a[], int size, int seed);

int compare_ints(const void* a, const void* b);

#endif
