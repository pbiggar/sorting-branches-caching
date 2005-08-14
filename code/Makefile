CC = gcc
#CFLAGS = -Wall -O3 -funroll-loops -finline-functions -D_USE_ROLLED_LOOPS
#CFLAGS = -Wall -g -D_USE_SOFTWARE_PREDICTOR -D_USE_ROLLED_LOOPS
#CFLAGS = -Wall -g -O3
#CFLAGS = -Wall -g
CFLAGS = -Wall -O3 -funroll-loops -finline-functions -D_USE_SOFTWARE_PREDICTOR -D_USE_ROLLED_LOOPS


ON2_SORTS = insertsort selectsort bubblesort shakersort improved_shakersort improved_bubblesort
BASE_SORTS = base_heapsort base_quicksort base_radixsort base_mergesort algorithm_n algorithm_s base_quicksort1 base_quicksort5 base_quicksort7 base_quicksort9 shellsort improved_shellsort
CACHE_SORTS = memory_tuned_heapsort memory_tuned4_heapsort tiled_mergesort multi_mergesort memory_tuned_quicksort multi_quicksort double_aligned_tiled_mergesort double_aligned_multi_mergesort memory_tuned_radixsort aligned_memory_tuned_radixsort
BRANCH_SORTS = multi_quicksort_seq predicated_quicksort


SORTS = $(BASE_SORTS) $(BRANCH_SORTS) $(CACHE_SORTS) $(ON2_SORTS) do_nothing
AUX = predictor utils visual_sorts
MODULES =  main.o $(addprefix sorts/, $(addsuffix .o, $(SORTS))) $(addprefix aux/, $(addsuffix .o, $(AUX))) $(addsuffix .o, $(MAINS))

fastsort: $(MODULES)
	$(CC) $(CFLAGS) $(MODULES) -o fastsort

asm: inline_asm.c
	$(CC) inline_asm.c -g -o asm

%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -f $(MODULES) $(SORTS) fastsort asm
