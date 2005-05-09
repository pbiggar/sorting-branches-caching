#define Item unsigned int
#include "utils.h"

void shellsort(Item a[], int N)
{
	int l = 0;
	int r = N-1;
	int i, h; 
	for (h = r-l+1; h > 0; h = ((h > 1) && (h < 5)) ? 1 : 5*h/11)
	{
		for (i = l+h; i <= r; i++)
		{
			int j = i; 
			Item v = a[i]; 
			while (j >= l+h && less(v, a[j-h]))
			{ 
				a[j] = a[j-h]; 
				j -= h; 
			}
			a[j] = v; 
		}
	}
}
