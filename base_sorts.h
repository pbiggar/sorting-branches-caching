#ifndef _BASE_SORTS_H_
#define _BASE_SORTS_H_

#include "utils.h"

void base_heapsort(unsigned int a[], int N);
void base_quicksort(unsigned int a[], int N);
void base_radixsort(unsigned int a[], int N);
void base_mergesort(unsigned int a[], int N);

void knuth_base_mergesort(unsigned int a[], int N);
void knuth_other_base_mergesort(unsigned int a[], int N);

void do_nothing(unsigned int a[], int N);
void base_quicksort1(unsigned int a[], int N);
void base_quicksort(unsigned int a[], int N);
void base_quicksort5(unsigned int a[], int N);
void base_quicksort7(unsigned int a[], int N);
void base_quicksort9(unsigned int a[], int N);

#endif
