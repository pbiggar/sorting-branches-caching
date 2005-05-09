#include "visual_sorts.h"
#include "base_sorts.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <malloc.h>
#include <errno.h>

static FILE* visual_log = NULL;

int
open_visual_log(const char* filename, const char* flags)
{
	visual_log = fopen(filename, flags);

	if (visual_log == NULL) 
	{
		printf("Error opening the visual log file: error %d \"%s\"", errno, strerror(errno));
		return 1;
	}

	fprintf(visual_log, "<html><head><title>Visual_sorts</title></head><body text=white bgcolor=black>");

	return 0;
}

void 
close_visual_log()
{
	fputs("</body></html>", visual_log);
	fclose(visual_log);
}

#define start_sort(Sort_name) do { fprintf(visual_log, "<hr><h1>%s Sort - file: \"%s\"</h1>", (Sort_name), __FILE__); } while(0)
#define start_vars() do { fprintf(visual_log, "<p>On line %d:<b>", __LINE__); } while(0)

#define add_var(A) do { fprintf(visual_log, " %s=%d", #A, (A)); } while(0)
#define end_vars() do { fputs("</b></p>", visual_log); } while(0)
#define add_comment(Comment) do { fprintf(visual_log, "<hr><h1>%s</h1>", (Comment)); } while(0)


#define MAX_HEIGHT 128.0
#define MAX 32768
#define	HEIGHT_DIV (MAX_HEIGHT / MAX)

static void
visual_print_array(int a[], int l, int r, int highlight_left, int hightlight_right, int highlight_other)
{
	int i;
	for(i = l; i <= r; i++) // we tile this ourselves cause of the fprintf
	{
		// the space is for a gap
		if (i == highlight_other)
			fprintf(visual_log, "<img src=m.gif width=2 title=%u height=%u> ", a[i], (int)(a[i] * HEIGHT_DIV));
		else if((i >= highlight_left) && (i <= hightlight_right))
			fprintf(visual_log, "<img src=r.gif width=2 title=%u height=%u> ", a[i], (int)(a[i] * HEIGHT_DIV));
		else
			fprintf(visual_log, "<img src=b.gif width=2 title=%u height=%u> ", a[i], (int)(a[i] * HEIGHT_DIV));
	}
				
	fputs("</p>", visual_log);
}

void
visual_base_mergesort(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int)); /* make it twice the size to use the notation */

	int i,j; /* indices for the first array */
	int k; /* indices for second array */
	
	int d; /* d => direction, if (f == 0) keep going */

	int count; /* how many in this list */
	int next_count;

	int track;

	unsigned int* temp_pointer;
	unsigned int temp;
	int min;

	unsigned int* source = a;
	unsigned int* target = aux;

	start_sort("Base heapsort");

	/* pre-sort */
	for(i = 0; i <= N-8; i+=8)
	{
		j = i+1;
		if (less(a[i+2], a[j])) j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i], a[j]);

		j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i+1], a[j]);

		compexch(a[i+2], a[i+3]);

		j = i+5;

		if (less(a[j], a[i+6])) j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+4]);

		j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+5]);

		compexch(a[i+7], a[i+6]);
	}

	/* do a selection sort for the next 4, if they exist */
	temp = i+4;

	add_comment("Initial presort");
	visual_print_array(a, 0, N-1, 0, i, -1);

	if (N < temp) temp = N;
	for(; i < temp-1; i++)
	{
		min = i;
		for(j = i+1; j < temp; j++)
		{
			if(less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}

	/* do a reverse sort for the remaining ones, if they exist */
	for(i = temp; i < N-1; i++)
	{
		min = i;
		for(j = i+1; j < N; j++)
		{
			if(!less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}

	add_comment("Finished presort");
	visual_print_array(a, 0, N-1, temp, N, -1);

	/* now beging merging */
	track = 0;
	count = 4;
	next_count = 8;

	i = 0;
	j = next_count - 1;
	k = 0;
	d = 1;

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
			
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];

			add_comment("i == j");
			visual_print_array(source, 0, N-1, -1, -1, i);

			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				if (next_count >= N) /* this means we're done */
				{
					break;
				}

				/* the next iteration will be oevr bigger lists */
				count = next_count;
				next_count <<= 1;

				temp_pointer = source;
				source = target;
				target = temp_pointer;

				i = 0;
				j = next_count - 1;
				if (j >= N)
				{
					j = N-1;
				}
				track = 0;
				k = 0;
				d = 1;
				continue;
			}

			j = i + next_count - 1;
			if (j >= N)
			{
				j = N-1;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = j;
			}
			else
			{
				d = 1;
				k = i;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i];
			i++;
			k += d;
		}
	}

	
	visual_print_array(target, 0, N-1, -1, -1, i);

	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);
}

void
visual_knuth_base_mergesort(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int)); /* make it twice the size to use the notation */

	int s = 0; /* this picks which area we write to */
	
	int i,j; /* indices for the first array */
	int k, l; /* indices for second array */
	
	int d, f; /* d => direction, if (f == 0) keep going */

	unsigned int temp;

	unsigned int* source;
	unsigned int* target;

N2:	/* Prepare for pass */

	i = 0;
	j = N-1;
	k = 0;
	l = N-1;
	d = 1;
	f = 1;

	if (s == 0)
	{
		source = a;
		target = aux;
	}
	else
	{
		source = aux;
		target = a;
	}
	visual_print_array(source, 0, N-1, -1, -1, i);
	

/*	printf("2,3\n"); */

N3: /* compare Ki, Kj */

	if (source[i] > source[j])
	{
/*		printf("3,8\n"); */
		goto N8;
	}

	if (i == j)
	{
		target[k] = source[i];
/*		printf("3,13\n"); */
		goto N13;
	}

/*N4:  transmit Ri */

/*	printf("3,4\n"); */

	target[k] = source[i];
	k = k + d; /* increment in the correct direction */

/*N5: // Stepdown? */

	i++;
	if (source[i-1] <= source[i])
	{
/*		printf("4,3\n"); */
		goto N3;
	}

/*	printf("4,6\n"); */
N6: 
	target[k] = source[j];
	k = k + d;

/*N7: // stepdown? */

	j--;
	if (source[j+1] <= source[j])
	{
/*		printf("6,6\n"); */
		goto N6;
	}
	else
	{
/*		printf("6,12\n"); */
		goto N12;
	}

N8: /* transmit Rj */

	target[k]  = source[j];
	k = k + d; /* increment in the correct direction */

/*N9: // Stepdown? */

	j--;
	if (source[j+1] <= source[j])
	{
/*		printf("8,3\n"); */
		goto N3;
	}

/*	printf("8,10\n"); */
N10: /* transmit Ri */


	target[k] = source[i];
	k = k + d;

/*N11: // stepdown? */

	i++;
	if (source[i-1] <= source[i])
	{
/*		printf("10,10\n"); */
		goto N10;
	}

/*	printf("10,12\n"); */
N12: /* switch sides (of the flow graph on page 162) */

	f = 0;
	d = -d; /* change the direction */
	temp = k;
	k = l;
	l = temp;

/*	printf("12,3\n"); */

	goto N3;
	

N13: /* switch areas */

	if (f == 0)
	{
		s = 1 - s; /* s = !s */
/*		printf("13,2\n"); */
		goto N2;
	}
	else /* sorting is complete */
	{
/*		printf("s = %d\n", s);
		exit(0); */ 
		if (s == 0)
		{
			memcpy(a, target, N * sizeof(unsigned int)); 
		}
	}

	free(aux);
}


static void
fixDown(unsigned int a[], int k, int N)
{
	int j;
	while(2*k <= N) /* move to next child */
	{
		j = 2*k;
		
		if ((j < N) && less(a[j], a[j+1])) /* see if it has more than 1 child - check which child is larger */
		{
			j++;
		}

		if (!less(a[k], a[j])) /* when the larger child isnt large engough stop */
		{
			break;
		}

		exch(a[k], a[j]); /* move down */
		k = j;
	}
}

void
visual_base_heapsort(unsigned int a[], int l, int r)
{
	int k;
	int N = r-l+1;
	start_sort("Base heapsort");

	for(k=N/2; k >= 1; k--)
	{
		fixDown(&a[-1], k, N);
	}
	add_comment("finished heapify");
	
	visual_print_array(a, 0, N-1, k*2, k*2+1, k);
	add_comment("sortdown");
	while(N>1)
	{
		exch(a[0], a[N-1]);
		fixDown(&a[-1], 1, --N);
		add_comment("reheapified");
		visual_print_array(a, 0, r-l, 0, N-1, 0);
	}
	add_comment("finished");
	visual_print_array(a, 0, r-l, -1, -1, -1);
}

#define LINE_BITS 5
#define LINE_MASK 0x1F
/* 32 byte line */ 

#define BLOCK_BITS 16
#define BLOCK_MASK 0xFFFF
/* 65536 blocks */

#define LIMIT 32
#define LIMIT_BITS 5 

void
visual_tiled_mergesort(unsigned int a[], int N)
{

	/* track keeps an eye on where i started last */
	/* outer track keeps and eye on which cache_sized/2 segemnt i started in last */
	int i,j,track; /* indices for the first array */
	int k; /* indices for second array */
	int min;
	
	int d, outer_d; /* d => direction */

	int count, next_count; /* how many in this list */

	int base, limit, last;

	unsigned int* temp_pointer;
	unsigned int temp;

	unsigned int *aux, *aux_data, *source, *target;


	start_sort("Tiled heapsort");
	aux_data = memalign(ALIGNMENT, 2 * N * sizeof(unsigned int));

	if (N > ((1 * 1024 * 1024) / sizeof(unsigned int))) /* half the cache for each array*/ 
	{
		unsigned int indexA = get_index(a);
		unsigned int indexB = get_index(aux_data);

		unsigned int minusA = (indexA ^ 0xFFFF);
		aux = (unsigned int*)(((unsigned int)aux_data & (~0x1FFFE0)) | (minusA << 5));
		if (minusA < indexB) /* then the new index is less than the old one */
		{
			aux = (unsigned int*)((unsigned int)aux | (1 << 21));
		}
	}
	else aux = aux_data;

	// to avoid valgrind giving us false posistive, we memset all to 0
	memset(aux_data, 0,  2 * N * sizeof(unsigned int));

	base = 0;
	last = 0;
	outer_d = 0;

	if (LIMIT > N) // if it can be done inside the cache
	{
		limit = N;
		base = 0;
		last = 1;

		goto small;
	}

	do
	{
		limit = base + LIMIT;

small: // we skip here if the data set fits in half the cache
		outer_d = 1 - outer_d;
		source = a;
		target = aux;

		if (limit > N) limit = N;

		/* pre-sort */ 
		for(i = base; i <= limit-8; i+=8)
		{
			j = i+1;
			if (less(a[i+2], a[j])) j = i+2;
			if (less(a[i+3], a[j])) j = i+3;
			compexch(a[i], a[j]);

			j = i+2;
			if (less(a[i+3], a[j])) j = i+3;
			compexch(a[i+1], a[j]);

			compexch(a[i+2], a[i+3]);

			j = i+5;

			if (less(a[j], a[i+6])) j = i+6;
			if (less(a[j], a[i+7])) j = i+7;
			compexch(a[j], a[i+4]);

			j = i+6;
			if (less(a[j], a[i+7])) j = i+7;
			compexch(a[j], a[i+5]);

			compexch(a[i+7], a[i+6]);
		} 

		/* do a selection sort for the next 4, if they exist */
		
		temp = i+4;

		if (limit < temp) temp = limit;
		add_comment("Initial presort");
		add_comment("a");
		visual_print_array(a, 0, N-1, base, limit-1, -1);

		for(; i < temp-1; i++)
		{
			min = i;
			for(j = i+1; j < temp; j++)
			{
				if(less(a[j],a[min]))
				{
					min = j;
				}
			}
			exch(a[i], a[min]);
		}

		/* do a reverse sort for the remaining ones, if they exist */
		for(i = temp; i < limit-1; i++)
		{
			min = i;
			for(j = i+1; j < limit; j++)
			{
				if(!less(a[j],a[min]))
				{
					min = j;
				}
			}
			exch(a[i], a[min]);
		}

		add_comment("Finished presort");
		add_comment("a");
		visual_print_array(a, 0, N-1, temp, limit-1, -1);

		count = 4;
		next_count = 8;

final:

		i = base;
		track = i;
		k = i;
		d = 1;
		j = i + next_count - 1;
		if (j >= limit - 1)
		{
			add_comment("here");

			j = limit-1;
			if (!last && outer_d == 0) // if theres only 1 copy, reverse here
			{
				/* its if this is the rightmost set */
				add_comment("rightmost set");
				k = j;
				d = -1;
			}
		}



		add_comment("sorting with vars:");
		start_vars();
		add_var(i);
		add_var(j);
		add_var(k);
		add_var(d);
		add_var(outer_d);
		add_var(track);
		add_var(base);
		add_var(limit);
		add_var(count);
		add_var(next_count);
		end_vars();

		while(1)
		{
			/* iterate through the left list */
			while(1)
			{
				if (last == 1)
				{	
					start_vars();
					add_var(i);
					add_var(source[i]);
					add_var(j);
					add_var(source[j]);
					add_var(k);
					add_var(d);
					add_var(outer_d);
					add_var(track);
					add_var(base);
					add_var(limit);
					add_var(count);
					add_var(next_count);
					end_vars();
				}
				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;
			}

			/* iterate through the right list */
			while(1)
			{
				if (last == 1)
				{	
					start_vars();
					add_var(i);
					add_var(source[i]);
					add_var(j);
					add_var(source[j]);
					add_var(k);
					add_var(d);
					add_var(outer_d);
					add_var(track);
					add_var(base);
					add_var(limit);
					add_var(count);
					add_var(next_count);
					end_vars();
				}
				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;
			}

			/* check if we're in the middle */
			if(i == j)
			{
				target[k] = source[i];
				add_comment("i == j");
				add_comment("source");
				visual_print_array(source, 0, N-1, base, limit-1, i);
				add_comment("target");
				visual_print_array(target, 0, N-1, base, limit-1, i);
				start_vars();
				add_var(i);
				add_var(j);
				add_var(k);
				add_var(d);
				add_var(outer_d);
				add_var(track);
				add_var(base);
				add_var(limit);
				add_var(count);
				add_var(next_count);
				end_vars();

				i = track + next_count;

				/* check whether we have left the building */
				if (i >= limit)
				{
					add_comment("we have left the building");
					if (next_count >= (limit - base)) /* this means we're done */
					{
						add_comment("done");
						break;
					}

					/* the next iteration will be over bigger lists */
					count = next_count;
					next_count <<= 1;

					// this is if the merge is of exactly LIMIT size, we need to make it bitonic
					start_vars();
					add_var(LIMIT);
					add_var(next_count);
					end_vars();

					temp_pointer = source;
					source = target;
					target = temp_pointer;

					i = base;
					j = i + next_count - 1;
					k = i;
					d = 1;
					if (j >= limit-1) // if this sort is smaller than we designed for
					{
						j = limit-1;
						if (!last && outer_d == 0) // go backwards
						{
							add_comment("hit it small");
							d = -1;
							k = j;
						}
					}
					track = i;

					if (next_count == LIMIT) // we want to hit the last copy only (not the rightmost)
					{
						add_comment("hit it big");
						if (outer_d == 0) // go backwards
						{
							d = -1;
							k = j;
						}
					}

					/* the last iteration will be backwards to keep it bitonic */
					continue;
				}

				j = i + next_count - 1;
				if (j >= limit-1)
				{
					j = limit-1;
				}

				track = i;

				/* setup k for the next one */
				if (d == 1) 
				{
					d = -1;
					k = j;
				}
				else
				{
					d = 1;
					k = i;
				}
				start_vars();
				add_var(i);
				add_var(j);
				add_var(k);
				add_var(track);
				add_var(count);
				add_var(next_count);
				end_vars();

			}
			else if (source[i] == source[j])
			{
				target[k] = source[i];
				i++;
				k += d;
			}
		}

		base += LIMIT;
	}
	while((base < N) && (last == 0));

	/* now we set the values properly and merge the final sets */
	if (last == 0)
	{
		add_comment("Finsihed the mini-merge");
		add_comment("target");
		visual_print_array(target, 0, N-1, -1, -1, -1);

		// the rightmost one may be smaller than the rest, and so may not be in the correct target. 
		// In this case we should copy over

		temp_pointer = target;
#if (LIMIT_BITS & 1)
		source = aux;
		target = a;
#else
		source = a;
		target = aux;
#endif

		if (temp_pointer != source)
		{
			add_comment("need copy");
			i = base - LIMIT;
			j = N - i;
			memcpy(&source[i], &temp_pointer[i], j * sizeof(unsigned int));
		}

		count = LIMIT;
		next_count = LIMIT << 1;
		limit = N;
		base = 0;
		last = 1;
		outer_d = 1; // forwards

		goto final;
	}


	add_comment("target");
	visual_print_array(target, 0, N-1, -1, -1, -1);

	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);
}


void
visual_multi_mergesort(unsigned int a[], int N)
{

	/* track keeps an eye on where i started last */
	/* outer track keeps and eye on which cache_sized/2 segemnt i started in last */
	int i,j,track; /* indices for the first array */
	int k; /* indices for second array */
	int min;
	
	int d, outer_d; /* d => direction */

	int count, next_count; /* how many in this list */

	int base, limit;

	unsigned int* temp_pointer;
	unsigned int temp;

	unsigned int *aux, *aux_data, *source, *target;

	int* indices;


	aux_data = memalign(ALIGNMENT, 2 * N * sizeof(unsigned int));

	if (N > 1024*1024/2)
	{
		unsigned int indexA = get_index(a);
		unsigned int indexB = get_index(aux_data);

		unsigned int minusA = (indexA ^ 0xFFFF);
		aux = (unsigned int*)(((unsigned int)aux_data & (~0x1FFFE0)) | (minusA << 5));
		if (minusA < indexB) /* then the new index is less than the old one */
		{
			aux = (unsigned int*)((unsigned int)aux | (1 << 21));
		}
	}
	else aux = aux_data;

	base = 0;
	outer_d = 0;

	if (LIMIT > N) /* if it can be done inside the cache */
	{
		limit = N;
		base = 0;
		outer_d = 1;

		goto small;
	}

	do
	{
		limit = base + LIMIT;
		outer_d = 1 - outer_d;

small: /* we skip here if the data set fits in half the cache */
		source = a;
		target = aux;

		if (limit > N) limit = N;

		/* pre-sort */ 
		for(i = base; i <= limit-8; i+=8)
		{
			j = i+1;
			if (less(a[i+2], a[j])) j = i+2;
			if (less(a[i+3], a[j])) j = i+3;
			compexch(a[i], a[j]);

			j = i+2;
			if (less(a[i+3], a[j])) j = i+3;
			compexch(a[i+1], a[j]);

			compexch(a[i+2], a[i+3]);

			j = i+5;

			if (less(a[j], a[i+6])) j = i+6;
			if (less(a[j], a[i+7])) j = i+7;
			compexch(a[j], a[i+4]);

			j = i+6;
			if (less(a[j], a[i+7])) j = i+7;
			compexch(a[j], a[i+5]);

			compexch(a[i+7], a[i+6]);
		} 

		/* do a selection sort for the next 4, if they exist */
		
		temp = i+4;

		if (limit < temp) temp = limit;
		for(; i < temp-1; i++)
		{
			min = i;
			for(j = i+1; j < temp; j++)
			{
				if(less(a[j],a[min]))
				{
					min = j;
				}
			}
			exch(a[i], a[min]);
		}

		/* do a reverse sort for the remaining ones, if they exist */
		for(i = temp; i < limit-1; i++)
		{
			min = i;
			for(j = i+1; j < limit; j++)
			{
				if(!less(a[j],a[min]))
				{
					min = j;
				}
			}
			exch(a[i], a[min]);
		}

		count = 4;
		next_count = 8;

		i = base;
		track = i;
		k = i;
		d = 1;
		j = i + next_count - 1;
		if (j >= limit - 1)
		{
			j = limit-1;
			if (outer_d == 0) /* if theres only 1 copy, reverse here */
			{
				/* its if this is the rightmost set */
				k = j;
				d = -1;
			}
		}

		while(1)
		{
			/* iterate through the left list */
			while(1)
			{
				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
					break;
				target[k] = source[i];
				k += d;
				i++;
			}

			/* iterate through the right list */
			while(1)
			{
				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
					break;
				target[k] = source[j];
				k += d;
				j--;
			}

			/* check if we're in the middle */
			if(i == j)
			{
				target[k] = source[i];
				i = track + next_count;

				/* check whether we have left the building */
				if (i >= limit)
				{
					if (next_count >= (limit - base)) /* this means we're done */
					{
						break;
					}

					/* the next iteration will be over bigger lists */
					count = next_count;
					next_count <<= 1;

					temp_pointer = source;
					source = target;
					target = temp_pointer;

					i = base;
					j = i + next_count - 1;
					k = i;
					d = 1;
					if (j >= limit-1) /* if this sort is smaller than we designed for */
					{
						j = limit-1;
						if (outer_d == 0) /* go backwards */
						{
							d = -1;
							k = j;
						}
					}
					track = i;

					if (next_count == LIMIT) /* we want to hit the last copy only (not the rightmost) */
					{
						if (outer_d == 0) /* go backwards */
						{
							d = -1;
							k = j;
						}
					}

					/* the last iteration will be backwards to keep it bitonic */
					continue;
				}

				j = i + next_count - 1;
				if (j >= limit-1)
				{
					j = limit-1;
				}

				track = i;

				/* setup k for the next one */
				if (d == 1) 
				{
					d = -1;
					k = j;
				}
				else
				{
					d = 1;
					k = i;
				}
			}
			else if (source[i] == source[j])
			{
				target[k] = source[i];
				i++;
				k += d;
			}
		}

		base += LIMIT;
	}
	while((base < N));

#define K (((N-1) >> (LIMIT_BITS))+1)
	add_comment("before k-way sort");
	visual_print_array(target, 0, N-1, -1, -1, -1);

	// now we do the k-way mergesort
	if (LIMIT <= N) // else its already sorted
	{
		temp_pointer = source;
		source = target;
		target = temp_pointer;

		// prepare the indices for the merge
		indices = malloc(K * sizeof(unsigned int));
		for(i = 0; i < K; i++)
		{
			if (i & 1) indices[i] = ((i+1) << LIMIT_BITS) - 1;
			else indices[i] = i << LIMIT_BITS;
		}

		indices[K-1] = N-1;

		// do the merge
		i = 0;
		while(i < N)
		{
			// find the smallest
			int index = 0;
			for(j = 1; j < K; j++) // TODO we can index off the end, so fix it
			{
				if (source[indices[j]] < source[indices[index]]) 
					index = j;
			}

			target[i] = source[indices[index]]; // put it in place
			add_comment("source");
			visual_print_array(source, 0, N-1, -1, -1, indices[index]);
			add_comment("target");
			visual_print_array(target, 0, N-1, -1, -1, i);
			start_vars();
			add_var(K);
			add_var(i);
			add_var(j);
			add_var(index);
			add_var(indices[index]);
			end_vars();

			if (index & 1) indices[index]--;
			else indices[index]++;

			i++;
		}
		free(indices);
	}


	/* copy back if its in the wrong array */
	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);
}
