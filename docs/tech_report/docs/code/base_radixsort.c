#define THRESHHOLD 10

#define bitsword 32
#define bitsbyte 8
#define bytesword 4
#define Radix (1 << bitsbyte)

#define digit(A, B) (((A) >> (bitsword-((B)+1)*bitsbyte)) & (Radix-1))

Item * radix_aux;

void
radixLSD(Item a[], int l, int r)
{
	/* R is the size of a digit, hence the number of bins */
	int count[Radix+1]; 

	/* this is from lsb. 3 == lsByte, 0 == msByte */
	for(w = bytesword - 1; w >= 0; w--) 
	{
		/* set the counts to 0 */
		for(int j = 0; j < Radix; j++) count[j] = 0; 
		for(int i = l; i <= r; i++) /* count each digit */
			count[digit(a[i], w) + 1]++;
		
		/* make the count cumulatative */
		for(int j = 1; j < Radix; j++) 
			count[j] += count[j-1];

		/* when we look up the array, we now have its position */
		for(int i = l; i <= r; i++) 
			radix_aux[count[digit(a[i], w)]++] = a[i];
		
		for(int i = l; i <= r; i++) /* copy it back */
			a[i] = radix_aux[i];
		
	}
}

void base_radixsort(Item a[], int N)
{
	radix_aux = malloc(N * sizeof(Item ));
	radixLSD(a, 0, N-1);
	free(radix_aux);
}
