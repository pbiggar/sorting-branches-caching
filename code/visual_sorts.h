#ifndef _VISUAL_SORTS_H_
#define _VISUAL_SORTS_H_

#include <stdio.h>

int open_visual_log(const char* file_name, const char* flags);
void close_visual_log();

void visual_base_mergesort(unsigned int a[], int N);
void visual_tiled_mergesort(unsigned int a[], int N);
void visual_base_heapsort(unsigned int a[], int l, int r);
void visual_multi_mergesort(unsigned int a[], int N);

void visual_knuth_base_mergesort(unsigned int a[], int N);
#endif
