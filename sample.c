#include "on2_sorts.h"

#include <math.h>


int
main()
{
	/*
	unsigned int a[] = {
		(int)'a',
		(int)'s',
		(int)'o',
		(int)'r',
		(int)'t',
		(int)'i',
		(int)'n',
		(int)'g',
		(int)'e',
		(int)'x',
		(int)'a',
		(int)'m',
		(int)'p',
		(int)'l',
		(int)'e' };

	on2_bubblesort2(a, 15);

	return 0;

	*/
/*
	double acc = 0, x;
	int i, k;
	double j;

//#define MAX 262144
#define MAX 32768
	
	for(i = 1; i < MAX+1; i++)
	{
		x = (double)(i)/(MAX+1);

		for(k = 1; k < 20; k++)
		{
			j = (double)(k);
//			printf("x = %f\n", x);
//			printf("1-x = %f\n", 1-x);
//			printf("j = %f\n", j);
//			printf("pow((1-x), j-1) = %f\n", pow(1-x, j-1));

			acc += (pow(1-x, j-1) * x * j);


//			printf("acc; %d, %d: %f\n", i,k , acc);
		}
//		printf("result %d: %f\n", i , acc);
	}
	acc /= MAX+1;
	printf("result: %f\n", acc);
	*/
	int count[4][256+1];
	printf("%d\n",    sizeof(count));  
}
