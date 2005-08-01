#ifndef _CACHE_SORTS_H_
#define _CACHE_SORTS_H_

#include "../aux/utils.h"


void memory_tuned_heapsort(unsigned int a[], int N); /* aligned 8 heap */
void memory_tuned4_heapsort(unsigned int a[], int N); /* aligned 4 heap */

void tiled_mergesort(unsigned int a[], int N); /* level 2 first, then continue merging */
void multi_mergesort(unsigned int a[], int N); /* level 2, then k-way mere using heapsort */
void double_aligned_tiled_mergesort(unsigned int a[], int N); /* level 1, level 2, then continue merging */
void double_aligned_multi_mergesort(unsigned int a[], int N); /* level 1, level 2, then k-way mere using heapsort */

void memory_tuned_quicksort(unsigned int a[], int N); /* insertion sort at the end */
void multi_quicksort(unsigned int a[], int N); /* sort into correct section first with linked lists */

void memory_tuned_radixsort(unsigned int a[], int N);
void aligned_memory_tuned_radixsort(unsigned int a[], int N);
#endif
