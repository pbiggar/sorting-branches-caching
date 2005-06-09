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
#include <unistd.h>
#include <fcntl.h>


#include "on2_sorts.h"
#include "base_sorts.h"
#include "branch_sorts.h"
#include "cache_sorts.h"
#include "visual_sorts.h"
#include "utils.h"

//#define _USE_SOFTWARE_PREDICTOR
#include "predictor.h"

//#define RANDOM_SIZE (4194304)
//#define RANDOM_SIZE (534523)
#define RANDOM_SIZE (262144)
//#define RANDOM_SIZE (32768)
//#define RANDOM_SIZE (16384)
//#define RANDOM_SIZE (4096)
//#define RANDOM_SIZE (128)

#define RUN_VISUAL 0
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
		print_diff_array(random_array2, sorted_array, RANDOM_SIZE, description, -1, -1, -1);
		printf("\n\nWhat it should be:\n");
		print_diff_array(sorted_array, random_array2, RANDOM_SIZE, "Sorted array", -1, -1, -1);
	}
}

#define RUN_COUNT 10
// we have enough data for 10 unique runs

#ifdef _USE_SOFTWARE_PREDICTOR
static void inline
predictor_run(void sort(unsigned int*, int), int counter_count, const char* description)
{
	unsigned int* temp_array = malloc(RANDOM_SIZE * sizeof(unsigned int));
	int random_file = open("paper/BIG", O_RDONLY);

	char filename[200];

	FILE* output_file;
	int i;
	if (random_file == -1) goto error_predictor_run;

	sprintf(filename, "paper/counters/%s", description);
	output_file = fopen(filename, "w");
	if (output_file == NULL) goto error_predictor_run;

	for(i = 0; i < counter_count; i++)
		init_predictor(&global_predictor[i]);

	for(i = 0; i < RUN_COUNT; i++)
	{
		int count = read(random_file, temp_array, RANDOM_SIZE * sizeof(unsigned int));
		int total = count;
		printf("Starting predictor run %d of '%s'\n", i, description);
		while(total < RANDOM_SIZE * sizeof(unsigned int))
		{
			if (count == -1) // error
			{
				fprintf(stderr, "problem reading\n");
				fprintf(output_file, "problem reading\n");
				exit(-1);
			}
			else if (count == 0)
			{
				printf("going back to the start\n");
				lseek(random_file, 0, SEEK_SET); // seek to the start of the file
				count = read(random_file, &((char*)temp_array)[total], (RANDOM_SIZE * sizeof(unsigned int)) - total);
				total += count;
			}
			else
			{
				printf("read some more (%d just read, %d total read, %d more to read)\n", count, total, (RANDOM_SIZE * sizeof(unsigned int)) - total);
				count = read(random_file, &((char*)temp_array)[total], (RANDOM_SIZE * sizeof(unsigned int)) - total);
				total += count;
			}
		}
		sort(temp_array, RANDOM_SIZE);
	}
	for(i = 0; i < counter_count; i++)
	{
		if
		(
			(global_predictor[i].taken_count[0] > 0) || (global_predictor[i].not_taken_count[0] > 0)
		 ||	(global_predictor[i].taken_count[1] > 0) || (global_predictor[i].not_taken_count[1] > 0)
		 ||	(global_predictor[i].taken_count[2] > 0) || (global_predictor[i].not_taken_count[2] > 0)
		 ||	(global_predictor[i].taken_count[3] > 0) || (global_predictor[i].not_taken_count[3] > 0)
		)
		{
			dump_predictor_info(stdout, &global_predictor[i], description);
			dump_predictor_info(output_file, &global_predictor[i], description);
		}
	}

	fclose(output_file);
	close(random_file);
error_predictor_run:
	printf("Error, with a value of %d and a string of '%s'\n", errno, strerror(errno));

	free(temp_array);
}
#else
static void inline
predictor_run(void sort(unsigned int*, int), int counter_count, const char* description)
{
	printf("Can't do a predictor run on %s, as support isnt compiled in\n", description);
}
#endif

static void inline
time_sort(void sort(unsigned int*, int), const char* description)
{
	memcpy(random_array2, random_array, RANDOM_SIZE * sizeof(unsigned int));
	start_timer();
	sort(random_array2, RANDOM_SIZE);
	stop_timer();
	print_timer(description);
	test_array(description);
}


#define TEST_MIN 8
#define TEST_MAX 10000
static void inline
test_sort(void sort(unsigned int*, int), const char* description)
{
	int i;
	printf("Beginning tests on %s\n", description);
	for(i = TEST_MIN; i < TEST_MAX; i++)
	{
		unsigned int* a = malloc(i * sizeof(unsigned int));
		unsigned int* b = malloc(i * sizeof(unsigned int));
//		memset(a, 0, i * sizeof(unsigned int));
//		memset(b, 0, i * sizeof(unsigned int));
		fill_random_array(a, i, i);
		memcpy(b, a, i * sizeof(unsigned int));
		qsort((void*)a, i, sizeof(unsigned int), compare_ints);
		sort(b, i);
		if (memcmp(a, b, i * sizeof(unsigned int)) != 0)
		{
			printf("%s is not sorted (size == %d):\n", description, i);
			print_diff_array(b, a, i, description, -1, -1, -1);
			printf("\n\nWhat it should be:\n");
			print_diff_array(a, b, i, "Sorted array", -1, -1, -1);
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
	if (RUN_VISUAL)
	{
		int visual = 0;
		visual = !open_visual_log("visual_sort.html", "w");
	}

	// make printf print upon a newline
	setvbuf(stdout, NULL, _IOLBF, 0);

	printf("N = %d\n\n", RANDOM_SIZE);

	fill_random_array(random_array, RANDOM_SIZE, 0);
	// get a sorted array for comparison
	memcpy(sorted_array, random_array, sizeof(unsigned int) * RANDOM_SIZE);
	qsort((void*)sorted_array, RANDOM_SIZE, sizeof(unsigned int), compare_ints);

	printf("Beginning sort tests\n");

	test_sort(double_tiled_mergesort, "Double Tiled Mergesort");
	test_sort(double_multi_mergesort, "Double Multi Mergesort");
//	predictor_run(knuth_base_mergesort, 8, "Algorithm N");
//	predictor_run(knuth_other_base_mergesort, 10, "Algorithm S");
//	predictor_run(base_heapsort, 6, "Base Heapsort");
//	predictor_run(base_quicksort, 4, "Base Quicksort");
//	predictor_run(base_quicksort9, 4, "Base Quicksort9");
//	predictor_run(cache_heapsort, 11, "Cache Heapsort");
//	predictor_run(cache_quicksort, 4, "Cache Quicksort");
//	predictor_run(cache4_heapsort, 7, "Cache4 Heapsort");
//	predictor_run(multi_quicksort, 6, "Multi Quicksort");
//	predictor_run(old_shellsort, 1, "Old Shellsort");
//	predictor_run(multi_quicksort_seq, 5, "Sequential Multi Quicksort");
//	predictor_run(shellsort, 2, "Shellsort");
//	predictor_run(tiled_mergesort, 33, "Tiled Mergesort");

//	predictor_run(multi_mergesort, 46, "Multi Mergesort");
//	predictor_run(double_tiled_mergesort, 46, "Double Tiled Mergesort");
//	predictor_run(double_multi_mergesort, 59, "Double Multi Mergesort");
//	predictor_run(base_mergesort, 33, "Base Mergesort");

//	predictor_run(base_quicksort5, 4, "Base Quicksort5");
//	predictor_run(base_quicksort1, 5, "Base Quicksort1");
//	predictor_run(base_quicksort7, 4, "Base Quicksort7");

//	time_sort(base_quicksort, "Base Quicksort");
//	time_sort(base_quicksort9, "Base Quicksort9");
//	time_sort(cache_quicksort, "Cache Quicksort");
//	time_sort(multi_quicksort, "Multi Quicksort");
//	time_sort(multi_quicksort_seq, "Sequential Multi Quicksort");
//	time_sort(predicated_quicksort, "Predicated Quicksort");

//	time_sort(base_mergesort, "Base Mergesort");
//	time_sort(knuth_base_mergesort, "Knuth Base Mergesort");
//	time_sort(knuth_other_base_mergesort, "Knuth Other Base Mergesort");
//	time_sort(tiled_mergesort, "Tiled Mergesort");
//	time_sort(multi_mergesort, "Multi Mergesort"); 
//	time_sort(double_tiled_mergesort, "Double Tiled Mergesort");
//	time_sort(double_multi_mergesort, "Double Multi Mergesort");
/*
	time_sort(base_radixsort, "Base Radixsort");
	time_sort(cache_radixsort, "Cache radixsort");

	time_sort(base_heapsort, "Base Heapsort");
	time_sort(cache_heapsort, "Cache Heapsort");
	time_sort(cache4_heapsort, "Cache Heapsort, 4-heap");
#if RANDOM_SIZE <= 32768
	time_sort(on2_insertsort, "O(N squared) Insertion");
	time_sort(on2_selectsort, "O(N squared) Selection");
#endif
#if RANDOM_SIZE <= 8192
	time_sort(on2_bubblesort, "O(N squared) Bubble");
	time_sort(on2_shakersort, "O(N squared) Shaker");
#endif
*/

	if (RUN_VISUAL)
	{
		if (visual)
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

	return 0;
}
