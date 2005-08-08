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


#define RANDOM_SIZE (4194304)
//#define RANDOM_SIZE (2097152)
//#define RANDOM_SIZE (534523)
//#define RANDOM_SIZE (262144)
//#define RANDOM_SIZE (32768)
//#define RANDOM_SIZE (16384)
//#define RANDOM_SIZE (8192)
//#define RANDOM_SIZE (4096)
//#define RANDOM_SIZE (128)

#define RUN_VISUAL 0
#define VISUAL_SIZE (128)

#include "aux/predictor.h"

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


#define TEST_MIN 1000
#define TEST_MAX 5000
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


// ****************************************************************************************************************
//                    tests
// ****************************************************************************************************************

/*	printf("Beginning sort tests (%d - %d)\n", TEST_MIN, TEST_MAX);
	test_sort(base_heapsort, "base heapsort");
	test_sort(memory_tuned_heapsort, "memory-tuned heapsort");
	test_sort(memory_tuned4_heapsort, "memory-tuned4 heapsort");

	test_sort(algorithm_n, "algorithm N");
	test_sort(algorithm_s, "algorithm S");
	test_sort(base_mergesort, "base mergesort");
*/	test_sort(tiled_mergesort, "tiled mergesort");
/*	test_sort(multi_mergesort, "multi-mergesort");
	test_sort(double_aligned_tiled_mergesort, "double-aligned tiled mergesort");
	test_sort(double_aligned_multi_mergesort, "double-aligned multi-mergesort");


	test_sort(base_quicksort1, "base quicksort (no median)");
	test_sort(base_quicksort, "base quicksort (median-of-3)");
	test_sort(base_quicksort5, "base quicksort (pseudo-median-of-5)");
	test_sort(base_quicksort7, "base quicksort (pseudo-median-of-7)");
	test_sort(base_quicksort9, "base quicksort (pseudo-median-of-9)");
	test_sort(memory_tuned_quicksort, "memory-tuned quicksort");
	test_sort(multi_quicksort, "multi-quicksort (binary search)");
	test_sort(multi_quicksort_seq, "multi-quicksort (sequential search)");

	test_sort(base_radixsort, "base radixsort");
	test_sort(memory_tuned_radixsort, "memory-tuned radixsort");
	test_sort(aligned_memory_tuned_radixsort, "aligned memory-tuned radixsort");
	
	test_sort(shellsort, "shellsort");
	test_sort(improved_shellsort, "improved shellsort");

	if (TEST_MAX <= 4096)
	{
		test_sort(insertsort, "insertion sort");
		test_sort(selectsort, "selection sort");
		test_sort(bubblesort, "bubblesort");
		test_sort(improved_bubblesort, "improved bubblesort");
		test_sort(shakersort, "shakersort");
		test_sort(improved_shakersort, "improved shakersort");
	}
	else
	{
		printf("No timings on insertsort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on insertsort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on improved_bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
		printf("No timings on improved_bubblesort - TEST_MAX (%d) is too big\n", TEST_MAX);
	}



// ****************************************************************************************************************
//                    timings
// ****************************************************************************************************************
	printf("Beginning sort timings (for N = %d)\n", RANDOM_SIZE);
	time_sort(base_heapsort, "base heapsort");
	time_sort(memory_tuned_heapsort, "memory-tuned heapsort");
	time_sort(memory_tuned4_heapsort, "memory-tuned4 heapsort");

	time_sort(algorithm_n, "algorithm N");
	time_sort(algorithm_s, "algorithm S");
	time_sort(base_mergesort, "base mergesort");
*/	time_sort(tiled_mergesort, "tiled mergesort");
/*	time_sort(multi_mergesort, "multi-mergesort");
	time_sort(double_aligned_tiled_mergesort, "double-aligned tiled mergesort");
	time_sort(double_aligned_multi_mergesort, "double-aligned multi-mergesort");


	time_sort(base_quicksort1, "base quicksort1 (no median");
	time_sort(base_quicksort, "base quicksort (median-of-3)");
	time_sort(base_quicksort5, "base quicksort5 (pseudo-median-of-5)");
	time_sort(base_quicksort7, "base quicksort7 (pseudo-median-of-7)");
	time_sort(base_quicksort9, "base quicksort9 (pseudo-median-of-9)");
	time_sort(memory_tuned_quicksort, "memory-tuned quicksort");
	time_sort(multi_quicksort, "multi-quicksort (binary search)");
	time_sort(multi_quicksort_seq, "multi-quicksort (sequential search)");


	time_sort(base_radixsort, "base radixsort");
	time_sort(memory_tuned_radixsort, "memory-tuned radixsort");
	time_sort(aligned_memory_tuned_radixsort, "aligned memory-tuned radixsort");
	
	time_sort(shellsort, "shellsort");
	time_sort(improved_shellsort, "improved shellsort");

	if (RANDOM_SIZE <= 32768)
	{
		time_sort(insertsort, "insertion sort");
		time_sort(selectsort, "selection");
	}
	else
	{
		printf("No timings on insertsort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on selectsort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
	}

	if (RANDOM_SIZE <= 8192)
	{
		time_sort(bubblesort, "bubblesort");
		time_sort(improved_bubblesort, "improved bubblesort");
		time_sort(shakersort, "shakersort");
		time_sort(improved_shakersort, "improved shakersort");
	}
	else
	{
		printf("No timings on bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on improved_bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
		printf("No timings on improved_bubblesort - RANDOM_SIZE (%d) is too big\n", RANDOM_SIZE);
	}


// ****************************************************************************************************************
//                   predictors 
// ****************************************************************************************************************

	printf("Running predictors\n");
	if (RANDOM_SIZE != (4 * 1024 * 1024)) printf("predictors not being run with correct size\n");

	printf("Not running predictors for do_nothing\n");

	predictor_run(base_heapsort, 6, "base heapsort");
	predictor_run(memory_tuned_heapsort, 11, "memory-tuned heapsort");
	predictor_run(memory_tuned4_heapsort, 7, "memory-tuned4 heapsort");

	predictor_run(algorithm_n, 8, "algorithm N");
	predictor_run(algorithm_s, 10, "algorithm S");
	predictor_run(base_mergesort, 33, "base mergesort");
	predictor_run(tiled_mergesort, 46, "tiled mergesort");
	predictor_run(multi_mergesort, 59, "multi-mergesort");
	predictor_run(double_aligned_tiled_mergesort, 46, "double-aligned tiled mergesort");
	predictor_run(double_aligned_multi_mergesort, 59, "double-aligned multi-mergesort");


	predictor_run(base_quicksort1, 5, "base quicksort (no median)");
	predictor_run(base_quicksort, 7, "base quicksort (median-of-3)");
	predictor_run(base_quicksort5, 12, "base quicksort (pseudo-median-of-5)");
	predictor_run(base_quicksort7, 17, "base quicksort (pseudo-median-of-7)");
	predictor_run(base_quicksort9, 22, "base quicksort (pseudo-median-of-9)");
	predictor_run(memory_tuned_quicksort, 4, "memory-tuned quicksort");
	predictor_run(multi_quicksort, 6, "multi-quicksort (binary search)");
	predictor_run(multi_quicksort_seq, 5, "multi-quicksort (sequential search)");

	printf("Not running predictors for base_radixsort\n");
	printf("Not running predictors for memory_tuned_radixsort\n");
	printf("Not running predictors for aligned_memory_tuned_radixsort\n");

	predictor_run(shellsort, 1, "shellsort");
	predictor_run(improved_shellsort, 2, "improved shellsort");

	if (RANDOM_SIZE <= 8192)
	{
		predictor_run(bubblesort, RANDOM_SIZE, "bubblesort");
		predictor_run(improved_bubblesort, RANDOM_SIZE, "improved bubblesort");
		predictor_run(insertsort, 2, "insertion sort");
	}
	else printf ("Not running predictors for bubblesort, improved_bubblesort, or insertsort as N is %d\n", RANDOM_SIZE);

	printf("Not running predictors for selectsort\n");
	printf("Not running predictors for shakersort\n");
	printf("Not running predictors for improved_shakersort\n");
*/

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
memory_tuned4_heapsort.c
memory_tuned_heapsort.c

algorithm_n.c
algorithm_s.c
base_mergesort.c
tiled_mergesort.c
multi_mergesort.c
double_aligned_multi_mergesort.c
double_aligned_tiled_mergesort.c

base_quicksort1.c
base_quicksort.c
base_quicksort5.c
base_quicksort7.c
base_quicksort9.c
memory_tuned_quicksort.c
multi_quicksort.c
multi_quicksort_seq.c

base_radixsort.c
memory_tuned_radixsort.c
aligned_memory_tuned_radixsort.c

shellsort.c
improved_shellsort.c

insertsort.c
selectsort.c
improved_bubblesort.c
bubblesort.c
improved_shakersort.c
shakersort.c
*/
