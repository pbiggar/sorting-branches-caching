CC = gcc
CFLAGS = -Wall -O3 -funroll-loops -finline-functions -D_USE_ROLLED_LOOPS
#CFLAGS = -Wall -g -D_USE_SOFTWARE_PREDICTOR -D_USE_ROLLED_LOOPS
#CFLAGS = -Wall -O3 -funroll-loops -finline-functions -D_USE_SOFTWARE_PREDICTOR -D_USE_ROLLED_LOOPS


ON2_SORTS = on2_insertsort on2_selectsort on2_bubblesort on2_shakersort shellsort old_shellsort on2_shakersort2 on2_bubblesort2
BASE_SORTS = base_heapsort base_quicksort base_radixsort base_mergesort knuth_base_mergesort knuth_other_base_mergesort base_quicksort1 base_quicksort5 base_quicksort7 base_quicksort9
CACHE_SORTS = cache_heapsort cache4_heapsort tiled_mergesort multi_mergesort cache_quicksort multi_quicksort double_tiled_mergesort double_multi_mergesort cache_radixsort
BRANCH_SORTS = multi_quicksort_seq predicated_quicksort

SORTS = $(BASE_SORTS) $(BRANCH_SORTS) $(CACHE_SORTS) $(ON2_SORTS) do_nothing
MODULES = predictor.o utils.o visual_sorts.o $(addsuffix .o, $(SORTS))

XS = $(basename $(MODULES)) fastsort

fastsort: $(MODULES) utils.h main.o
	$(CC) $(CFLAGS) $(MODULES) main.o -o fastsort

sample: sample.o
	$(CC) $(CFLAGS) $(MODULES) sample.o -g -o sample

asm: inline_asm.c
	$(CC) inline_asm.c -g -o asm

%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@



clean:
	rm -f $(MODULES) $(SORTS) fastsort asm

#perfex:
#	$(CC) $(CFLAGS) 
