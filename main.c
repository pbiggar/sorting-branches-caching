/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Sat Oct 11 2003
    copyright            : (C) 2003 by Paul Biggar
    email                : biggarp@tcd.ie
 ***************************************************************************/

#include <stdio.h> // fopen, fgets, fclose
#include <errno.h> // errno
#include <assert.h> // assert
#include <string.h> // strerror
#include <stdlib.h> // atoi, malloc, free
#include <sys/types.h> // time
#include <sys/time.h> // time

#include "on2_sorts.h"
#include "base_sorts.h"
#include "branch_sorts.h"
#include "cache_sorts.h"
#include "visual_sorts.h"
#include "utils.h"

//#define RANDOM_SIZE (4194304)
//#define RANDOM_SIZE (534523)
#define RANDOM_SIZE (262144)
//#define RANDOM_SIZE (16384)
//#define RANDOM_SIZE (4096)
//#define RANDOM_SIZE (128)

#define RUN_VISUAL 1
#define VISUAL_SIZE (128)

unsigned int sorted_array[RANDOM_SIZE];
unsigned int random_array[RANDOM_SIZE];
unsigned int random_array2[RANDOM_SIZE];

FILE* visual_log = NULL;

static struct timeval t1;
static struct timeval t2;


static inline long
get_milliseconds(struct timeval *tp1, struct timeval *tp2)
{
	return ( (tp2->tv_sec - tp1->tv_sec) * 1000000) + (tp2->tv_usec - tp1->tv_usec);
}

static void inline 
start_timer()
{
	if (gettimeofday(&t1, NULL) != 0)  printf("Bad result: \"%s\"", strerror(errno));
}

static void inline
stop_timer()
{
	if (gettimeofday(&t2, NULL) != 0) printf("Bad result: \"%s\"", strerror(errno));
}

static void inline
print_timer(const char* description)
{
	printf("%s: %ld ticks\n", description, get_milliseconds(&t1, &t2));
}

static void inline
test_array(const char* description)
{
	if (memcmp(sorted_array, random_array2, RANDOM_SIZE * sizeof(unsigned int)) == 0)
		printf("%s is sorted\n\n", description);
	else
	{
		printf("Not sorted:\n");
		print_int_array(random_array2, RANDOM_SIZE, description, -1, -1, -1);
		printf("\n\nWhat it should be:\n");
		print_int_array(sorted_array, RANDOM_SIZE, "Sorted array", -1, -1, -1);
	}
}

static void inline
time_sort(void sort(unsigned int*, int), const char* description)
{
	memcpy(random_array2, random_array, RANDOM_SIZE * sizeof(int));
	start_timer();
	sort(random_array2, RANDOM_SIZE);
	stop_timer();
	print_timer(description);
	test_array(description);
}

static void inline
test_sort(void sort(unsigned int*, int), const char* description)
{
	int i;
	for(i = 8; i < 256; i++)
	{
		int* a = malloc(i * sizeof(unsigned int));
		int* b = malloc(i * sizeof(unsigned int));
		fill_random_array(a, i, i);
		memcpy(b, a, i * sizeof(unsigned int));
		qsort((void*)a, i, sizeof(unsigned int), compare_ints);
		sort(b, i);
		if (memcmp(a, b, i * sizeof(unsigned int)) != 0)
		{
			printf("%s is not sorted (size == %d):\n", description, i);
			print_int_array(b, i, description, -1, -1, -1);
			printf("\n\nWhat it should be:\n");
			print_int_array(a, i, "Sorted array", -1, -1, -1);
			exit(-1);
		}
		free(a);
		free(b);
	}
	printf("Testing of %s complete\n", description);
}

int
main(int argc, char** args)
{
	int i;
	int seeds[5] = { 34, 5, 24, 456, 38576 };

	//int *test;
	int visual = 0;
	visual = !open_visual_log("visual_sort.html", "w");

	// make printf print upon a newline
  //  setvbuf(stdout, NULL, _IOLBF, 0);

	printf("N = %d\n\n", RANDOM_SIZE);


	test_sort(shellsortGo, "Gonnets shellsort");
	for(i = 0; i < 5; i++)
	{
		printf("Beginning sort tests (seed = %d)\n", seeds[i]);

		fill_random_array(random_array, RANDOM_SIZE, seeds[i]);
		// get a sorted array for comparison
		memcpy(sorted_array, random_array, sizeof(unsigned int) * RANDOM_SIZE);
		qsort((void*)sorted_array, RANDOM_SIZE, sizeof(unsigned int), compare_ints);

		time_sort(base_quicksort, "Base Quicksort");
		time_sort(cache_quicksort, "Cache Quicksort");
		time_sort(shellsortGo, "Gonnets shellsort");

		time_sort(base_mergesort, "Base Mergesort");
		time_sort(tiled_mergesort, "Tiled Mergesort");
		time_sort(double_tiled_mergesort, "Double Tiled Mergesort");
		time_sort(double_multi_mergesort, "Double Multi Mergesort");

		time_sort(base_radixsort, "Base Radixsort");

		time_sort(cache_heapsort, "Cache Heapsort");


	}

/*	test_sort(on2_insertsort, "O(N squared) Insertion");
	test_sort(on2_selectsort, "O(N squared) Selection");
	test_sort(on2_bubblesort, "O(N squared) Bubble");
	test_sort(on2_shakersort, "O(N squared) Shaker");

	test_sort(base_quicksort, "Base Quicksort");
	test_sort(cache_quicksort, "Cache Quicksort");
	test_sort(multi_quicksort, "Multi Quicksort");
	test_sort(multi_quicksort_seq, "Sequential Multi Quicksort");

	test_sort(base_mergesort, "Base Mergesort");
	test_sort(tiled_mergesort, "Tiled Mergesort");
	test_sort(multi_mergesort, "Multi Mergesort");
	test_sort(knuth_base_mergesort, "Knuth Base Mergesort");
	test_sort(knuth_other_base_mergesort, "Knuth Other Base Mergesort");
	test_sort(double_tiled_mergesort, "Double Tiled Mergesort");
	test_sort(double_multi_mergesort, "Double Multi Mergesort");

	test_sort(base_radixsort, "Base Radixsort");

	test_sort(base_heapsort, "Base Heapsort");
	test_sort(cache4_heapsort, "Cache Heapsort, 4-heap");
	test_sort(cache_heapsort, "Cache Heapsort");
	printf("Finished sort tests\n");



	start_timer();
	test = malloc(RANDOM_SIZE * sizeof(unsigned int));
	stop_timer();
	print_timer("Time to malloc N");

	start_timer();
	fill_random_array(random_array, RANDOM_SIZE, seed);
	stop_timer();
	print_timer("Time to fill N");

	start_timer();
	memcpy(test, random_array, sizeof(unsigned int) * RANDOM_SIZE);
	stop_timer();
	print_timer("Time to memcpy array of size N");

	start_timer();
	free(test);
	stop_timer();
	print_timer("Time to free N");
	printf("\n");


#if RANDOM_SIZE <= 32768
	time_sort(on2_insertsort, "O(N squared) Insertion");
	time_sort(on2_selectsort, "O(N squared) Selection");
#endif
#if RANDOM_SIZE <= 8192
	time_sort(on2_bubblesort, "O(N squared) Bubble");
	time_sort(on2_shakersort, "O(N squared) Shaker");
#endif
	time_sort(base_quicksort, "Base Quicksort");
	time_sort(cache_quicksort, "Cache Quicksort");
	time_sort(multi_quicksort, "Multi Quicksort");
	time_sort(multi_quicksort_seq, "Sequential Multi Quicksort");

	time_sort(base_mergesort, "Base Mergesort");
	time_sort(knuth_base_mergesort, "Knuth Base Mergesort");
	time_sort(knuth_other_base_mergesort, "Knuth Other Base Mergesort");
	time_sort(tiled_mergesort, "Tiled Mergesort");
	time_sort(multi_mergesort, "Multi Mergesort");
	time_sort(double_tiled_mergesort, "Double Tiled Mergesort");
	time_sort(double_multi_mergesort, "Double Multi Mergesort");

	time_sort(base_radixsort, "Base Radixsort");

	time_sort(base_heapsort, "Base Heapsort");
	time_sort(cache_heapsort, "Cache Heapsort");
	time_sort(cache4_heapsort, "Cache Heapsort, 4-heap");

	if (visual)
	{
		if (RUN_VISUAL)
		{
			printf("Starting visual\n");

			memcpy(random_array2, random_array, (VISUAL_SIZE) * sizeof(unsigned int));
			visual_tiled_mergesort(random_array2, VISUAL_SIZE);
			qsort(random_array, VISUAL_SIZE, sizeof(unsigned int), compare_ints);
			if (memcmp(random_array2, random_array, VISUAL_SIZE * sizeof(unsigned int)) != 0)
			{
				printf("%s is not sorted (size == %d):\n", "visual", VISUAL_SIZE);
				print_int_array(random_array2, VISUAL_SIZE, "visual", -1, -1, -1);
				printf("\n\nWhat it should be:\n");
				print_int_array(sorted_array, VISUAL_SIZE, "Sorted array", -1, -1, -1);
			}
		}
		close_visual_log();
	}
	else printf("The file did not open, and visual routines will not run\n");
*/
	return 0;
}
