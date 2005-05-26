#include <stdio.h> // printf, putchar
#include <stdlib.h> // atoi, malloc, free
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#define CONSOLE_WIDTH 10

//returns 0 is sorted
int check_sorted(unsigned int a[], int l, int r)
{
	int i;
	for(i = l+1; i <= r; i++)
	{
		if (a[i-1] > a[i])
		{
			printf("NOT SORTED: a[%d] > a[%d] (%d > %d)\n", (i-1), i, a[i-1], a[i]);
			return -1;
		}
	}
	return 0;
}

#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

void 
setcolor(int attr, int fg, int bg)
{
	printf("%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

void
highlight_text()
{
	setcolor(BRIGHT, RED, BLACK);
}

void
highlight_text2()
{
	setcolor(BRIGHT, CYAN, BLACK);
}

void
reset_text()
{
	setcolor(RESET, WHITE, BLACK);
}


// prints an array of size count, with each member sized size, prefix by description, and highlighting the bits between left and right(inclusive)
void
print_int_array(unsigned int a[], int N, const char* description, int left, int right, int pivot)
{
	int i;
	printf("%s:\n\t", description);
	highlight_text2();
	for(i = 0; i < CONSOLE_WIDTH; i++) // print the row numbers
	{
		printf("%8d  ", i);
	}
	printf("\n\n0\t");
	reset_text();

	for(i = 0; i < N; i++)
	{
//		if (i == pivot) highlight_text2();
//		else if ((i >= left) && (i <= right)) highlight_text();

		printf("%8u", (a[i]));
		
//		if ((i == pivot) || ((i >= left) && (i <= right)))  reset_text();
		
		printf(", ");

		if ((i+1) % CONSOLE_WIDTH == 0 && (i > 1))
		{
//			highlight_text2();
			printf("\n%d\t", i+1);
//			reset_text();
		}
	}
	printf("\n");

}
void
print_array(void* array, unsigned int count, unsigned int size, const char* description)
{
	unsigned int i, j;
	char* a = (char*)array;
	printf("%s:\n", description);
	for(i = 0; i < count; i++)
	{
		if (size == sizeof(char) || size == sizeof(int))
		{
			printf("%c, ", ((int*)a)[i]);
		}
		else
		{
			printf("0x");
			for(j = 0; j < size; j++)
			{
				printf("%2x", a[i*size+j]);
			}
			putchar(' ');
		}

		if (i % CONSOLE_WIDTH == 0 && i != 0) printf("\n");
	}
	printf("\n");
}

void
fill_random_array(unsigned int a[], int size, int seed)
{
	int count = 0;
	int random_file = open("paper/BIG", O_RDONLY);
	lseek(random_file, seed, SEEK_SET);
	if (random_file)
	{
		count = read(random_file, a, sizeof(unsigned int) * size);
	}
	if (count != size * sizeof(unsigned int))
	{
		printf("array not filled\n");
		exit(-1);
	}
	close(random_file);
}


int
compare_ints(const void* a, const void* b)
{
	unsigned int t_a = *((unsigned int*)(a));
	unsigned int t_b = *((unsigned int*)(b));

	if (t_a == t_b) return 0;
	if (t_a < t_b) return -1;
	return 1;
}
