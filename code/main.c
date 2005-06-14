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
#include <sys/time.h> // time
#include <unistd.h> // time
#include <fcntl.h> // time

#include "sorts/on2_sorts.h"
#include "sorts/base_sorts.h"
#include "sorts/branch_sorts.h"
#include "sorts/cache_sorts.h"
#include "aux/visual_sorts.h"
#include "aux/utils.h"

#include "aux/predictor.h"

//#define RANDOM_SIZE (4194304)
//#define RANDOM_SIZE (534523)
//#define RANDOM_SIZE (262144)
//#define RANDOM_SIZE (32768)
#define RANDOM_SIZE (16384)
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
	int random_file = open("../data/BIG", O_RDONLY);

	char filename[200];

	FILE* output_file;
	int i;
	if (random_file == -1) goto error_predictor_run;

	sprintf(filename, "../data/counters/%s", description);
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


#define TEST_MIN 1024
#define TEST_MAX 1025
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
	int visual = 0;
	if (RUN_VISUAL)
	{
		visual = !open_visual_log("../visual_sort.html", "w");
	}

	// make printf print upon a newline
	setvbuf(stdout, NULL, _IOLBF, 0);

	fill_random_array(random_array, RANDOM_SIZE, 0);
	// get a sorted array for comparison
	memcpy(sorted_array, random_array, sizeof(unsigned int) * RANDOM_SIZE);
	qsort((void*)sorted_array, RANDOM_SIZE, sizeof(unsigned int), compare_ints);


/* ****************************************************************************************************************
 *                    tests
 * ****************************************************************************************************************/
	printf("Beginning sort tests (%d - %d)\n", TEST_MIN, TEST_MAX);
	test_sort(base_heapsort, "Base Heapsort");
	test_sort(cache_heapsort, "Cache Heapsort");
	test_sort(cache4_heapsort, "Cache4 Heapsort");

	test_sort(knuth_base_mergesort, "Algorithm N");
	test_sort(knuth_other_base_mergesort, "Algorithm S");
	test_sort(base_mergesort, "Base Mergesort");
	test_sort(tiled_mergesort, "Tiled Mergesort");
	test_sort(multi_mergesort, "Multi Mergesort");
	test_sort(double_tiled_mergesort, "Double Tiled Mergesort");
	test_sort(double_multi_mergesort, "Double Multi Mergesort");


	test_sort(base_quicksort1, "Base Quicksort1");
	test_sort(base_quicksort, "Base Quicksort");
	test_sort(base_quicksort5, "Base Quicksort5");
	test_sort(base_quicksort7, "Base Quicksort7");
	test_sort(base_quicksort9, "Base Quicksort9");
	test_sort(cache_quicksort, "Cache Quicksort");
	test_sort(multi_quicksort, "Multi Quicksort");
	test_sort(multi_quicksort_seq, "Sequential Multi Quicksort");


	test_sort(base_radixsort, "Base Radixsort");
	test_sort(base_radixsort, "Cache Radixsort");
	
	test_sort(old_shellsort, "Old Shellsort");
	test_sort(shellsort, "Shellsort");

	if (TEST_MAX <= 4096)
	{
		test_sort(on2_insertsort, "O(N squared) Insertion");
		test_sort(on2_selectsort, "O(N squared) Selection");
	}
	else
	{
		printf("No timings on on2_insertsort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on on2_insertsort - TEST_MAX (%d) is too big\n", TEST_MAX);
	}
	if (TEST_MAX <= 4096)
	{
		test_sort(on2_bubblesort, "O(N squared) Bubble");
		test_sort(on2_bubblesort2, "O(N squared) Bubble2");
		test_sort(on2_shakersort, "O(N squared) Shaker");
		test_sort(on2_shakersort2, "O(N squared) Shaker2");
	}
	else
	{
		printf("No timings on on2_bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on on2_bubblesort2 - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on on2_bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on on2_bubblesort2 - TEST_MAX (%d) is too big\n", TEST_MAX);
	}



// ****************************************************************************************************************
//                    timings
// ****************************************************************************************************************
	printf("Beginning sort timings (for N = %d)\n", RANDOM_SIZE);
	time_sort(base_heapsort, "Base Heapsort");
	time_sort(cache_heapsort, "Cache Heapsort");
	time_sort(cache4_heapsort, "Cache4 Heapsort");

	time_sort(knuth_base_mergesort, "Algorithm N");
	time_sort(knuth_other_base_mergesort, "Algorithm S");
	time_sort(base_mergesort, "Base Mergesort");
	time_sort(tiled_mergesort, "Tiled Mergesort");
	time_sort(multi_mergesort, "Multi Mergesort");
	time_sort(double_tiled_mergesort, "Double Tiled Mergesort");
	time_sort(double_multi_mergesort, "Double Multi Mergesort");


	time_sort(base_quicksort1, "Base Quicksort1");
	time_sort(base_quicksort, "Base Quicksort");
	time_sort(base_quicksort5, "Base Quicksort5");
	time_sort(base_quicksort7, "Base Quicksort7");
	time_sort(base_quicksort9, "Base Quicksort9");
	time_sort(cache_quicksort, "Cache Quicksort");
	time_sort(multi_quicksort, "Multi Quicksort");
	time_sort(multi_quicksort_seq, "Sequential Multi Quicksort");


	time_sort(base_radixsort, "Base Radixsort");
	time_sort(base_radixsort, "Cache Radixsort");
	
	time_sort(old_shellsort, "Old Shellsort");
	time_sort(shellsort, "Shellsort");

	if (RANDOM_SIZE <= 32768)
	{
		time_sort(on2_insertsort, "O(N squared) Insertion");
		time_sort(on2_selectsort, "O(N squared) Selection");
	}
	else
	{
		printf("No timings on on2_insertsort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on on2_insertsort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
	}
	if (RANDOM_SIZE <= 8192)
	{
		time_sort(on2_bubblesort, "O(N squared) Bubble");
		time_sort(on2_bubblesort2, "O(N squared) Bubble2");
		time_sort(on2_shakersort, "O(N squared) Shaker");
		time_sort(on2_shakersort2, "O(N squared) Shaker2");
	}
	else
	{
		printf("No timings on on2_bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on on2_bubblesort2 - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on on2_bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on on2_bubblesort2 - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
	}


// ****************************************************************************************************************
//                   predictors 
// ****************************************************************************************************************

	printf("Running predictors\n");
	if (RANDOM_SIZE != (4 * 1024 * 1024)) printf("predictors not being run with correct size\n");

	printf("Not running predictors for do_nothing\n");

	predictor_run(base_heapsort, 6, "Base Heapsort");
	predictor_run(cache_heapsort, 11, "Cache Heapsort");
	predictor_run(cache4_heapsort, 7, "Cache4 Heapsort");

	predictor_run(knuth_base_mergesort, 8, "Algorithm N");
	predictor_run(knuth_other_base_mergesort, 10, "Algorithm S");
	predictor_run(base_mergesort, 33, "Base Mergesort");
	predictor_run(tiled_mergesort, 33, "Tiled Mergesort");
	predictor_run(multi_mergesort, 46, "Multi Mergesort");
	predictor_run(double_tiled_mergesort, 46, "Double Tiled Mergesort");
	predictor_run(double_multi_mergesort, 59, "Double Multi Mergesort");


	predictor_run(base_quicksort1, 5, "Base Quicksort1");
	predictor_run(base_quicksort, 7, "Base Quicksort");
	predictor_run(base_quicksort5, 12, "Base Quicksort5");
	predictor_run(base_quicksort7, 17, "Base Quicksort7");
	predictor_run(base_quicksort9, 22, "Base Quicksort9");
	predictor_run(cache_quicksort, 4, "Cache Quicksort");
	predictor_run(multi_quicksort, 6, "Multi Quicksort");
	predictor_run(multi_quicksort_seq, 5, "Sequential Multi Quicksort");

	printf("Not running predictors for base_radixsort\n");
	printf("Not running predictors for cache_radixsort\n");

	predictor_run(old_shellsort, 1, "Old Shellsort");
	predictor_run(shellsort, 2, "Shellsort");

	printf("Not running predictors for on2_insertsort\n");
	printf("Not running predictors for on2_selectsort\n");
	printf("Not running predictors for on2_bubblesort\n");
	printf("Not running predictors for on2_bubblesort2\n");
	printf("Not running predictors for on2_shakersort\n");
	printf("Not running predictors for on2_shakersort2\n");


// ****************************************************************************************************************
//                  visual 
// ****************************************************************************************************************
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

/* list of sorts
do_nothing.c

base_heapsort.c
cache4_heapsort.c
cache_heapsort.c

knuth_base_mergesort.c
knuth_other_base_mergesort.c
base_mergesort.c
multi_mergesort.c
double_multi_mergesort.c
double_tiled_mergesort.c
tiled_mergesort.c

base_quicksort1.c
base_quicksort.c
base_quicksort5.c
base_quicksort7.c
base_quicksort9.c
cache_quicksort.c
multi_quicksort.c
multi_quicksort_seq.c

base_radixsort.c
cache_radixsort.c

old_shellsort.c
shellsort.c

on2_insertsort.c
on2_selectsort.c
on2_bubblesort2.c
on2_bubblesort.c
on2_shakersort2.c
on2_shakersort.c
*/
