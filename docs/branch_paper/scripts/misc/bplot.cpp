#include <iostream>
#include <cstdlib>
#include <ctime>

void swap(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
	return;
}

void print(int* a, int n)
{
	using namespace std;
	for(int i = 0; i < n; i++)
	{
		cout << a[i] << " ";
	}
	cout << endl;
	return;
}

int state = 0;

void update_bimodal(bool taken)
{
	if(taken)
  	{
    		state++;
		if(state > 3) state = 3;
    	}
	else
	{
		state--;
		if(state < 0) state = 0;
	}
}

bool correct_predict(bool taken)
{
	if(taken && state >= 2 || !taken && state < 2)
	{
		return true;
	}
	return false;
}

int main()
{
	using namespace std;
	const int NUM_ITERS = 500;
	const int n = 8192;

	srand(time(0));

	int *a = new int[n];
	unsigned long long *total_correct = new unsigned long long[n];
	for(int i = 0; i < n; i++)
	{
		total_correct[i] = 0;
	}
	for(int iter = 0; iter < NUM_ITERS; iter++)
	{
		for(int z = 0; z < n; z++)
		{
			a[z] = z;
		}
		for(int z = 0; z < n; z++)
		{
			swap(a[z], a[z + rand() % (n - z)]);
		}
		for(int i = 0; i < n - 1; i++)
		{
			for(int j = 0; j < n - i - 1; j++)
			{
				if(a[j] > a[j + 1])
				{
					if(correct_predict(true)) total_correct[i]++;
                                        update_bimodal(true);
					swap(a[j], a[j + 1]);						
				}
                                else
				{
					if(correct_predict(false)) total_correct[i]++;
					update_bimodal(false);
					
				}
			}
		}
	}

	for(int i = 0; i < n - 1; i++)
	{
		cout << i + 1 << " " << n - i - 1 << " " << total_correct[i] / (double) NUM_ITERS << endl;
	}


	delete [] a;
	delete [] total_correct;

	return 0;
}


