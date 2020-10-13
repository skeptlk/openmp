#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 
#include <time.h>
#include <omp.h>
#include <limits.h> 

void merge(int arr[], int beg, int mid, int end, int maxele) 
{ 
	int i = beg; 
	int j = mid + 1; 
	int k = beg; 
	while (i <= mid && j <= end) { 
		if (arr[i] % maxele <= arr[j] % maxele) { 
			arr[k] = arr[k] + (arr[i] % maxele) * maxele; 
			k++; 
			i++; 
		} 
		else { 
			arr[k] = arr[k] + (arr[j] % maxele) * maxele; 
			k++; 
			j++; 
		} 
	} 
	while (i <= mid) { 
		arr[k] = arr[k] + (arr[i] % maxele) * maxele; 
		k++; 
		i++; 
	} 
	while (j <= end) { 
		arr[k] = arr[k] + (arr[j] % maxele) * maxele; 
		k++; 
		j++; 
	} 

	// Obtaining actual values 
	for (int i = beg; i <= end; i++) 
		arr[i] = arr[i] / maxele; 
} 

void merge_sort_rec(int arr[], int beg, int end, int maxele) 
{ 
	if (beg < end) { 
		int mid = (beg + end) / 2; 
		merge_sort_rec(arr, beg, mid, maxele); 
		merge_sort_rec(arr, mid + 1, end, maxele); 
		merge(arr, beg, mid, end, maxele); 
	} 
} 

int max_element (int arr[], int n) {
	int max = INT_MIN;
	for (int i = 0; i < n; i++)
		if (arr[i] > max) max = arr[i];
	return max;
}

void merge_sort(int arr[], int n, int maxele) 
{ 
	merge_sort_rec(arr, 0, n-1, maxele); 
} 

void fill_rand(int *arr, int n)
{
	srand(time(0));
	for (int i = 0; i < n; i++)
		arr[i] = rand() % 1000 - 300;
}

bool test_sorted(int *arr, int n)
{
	for (int i = 1; i < n; i++)
		if (arr[i] < arr[i - 1]) return false;
	return true;
}

int main() 
{ 
	int n = 33; 
	int arr[n]; 

	fill_rand(arr, n);
	int maxele = max_element(arr, n) + 1; 
	int threads_n = 0; // remember number of threads in parallel block

	int sumbef = 0, sumaft = 0;
	for (int i = 0; i < n; sumbef += arr[i], i++);

	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");

	#pragma omp parallel num_threads(3) shared(arr)
	{
		const int i = omp_get_thread_num();
		const int thr_n = omp_get_num_threads();
		const int share = n / thr_n;
		const int left  = i * share;
		// if this is a last chunk, take all that remains
		const int right = (i == thr_n - 1) ? n - 1: (i + 1) * share - 1;
		// printf("thread #%d: indices %d to %d\n", i, left, right);

		#pragma omp single
			threads_n = thr_n;

		merge_sort(arr + left, right - left + 1, maxele);
	}

	const int share = (n / threads_n);
	int mid, right;
	for (int i = 1; i < threads_n; i++) {	
		mid = i * share - 1;
		// if this is a last chunk, take all that remains
		right = (i == threads_n - 1) ? n - 1: (i + 1) * share - 1;
		
		// printf("merging arr[] left=%d, mid=%d, right=%d\n", 0, mid, right);
		merge(arr, 0, mid, right, maxele);
	}

	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
	


	sumaft = 0;
	for (int i = 0; i < n; sumaft += arr[i], i++);
	
	if (test_sorted(arr, n))
		printf("Test passed!\n");
	else
		printf("[!!!] Test failed!\n");

	if (sumbef == sumaft)
		printf("Sum not changed!\n");
	else
		printf("[!!!] Sum before = %d, after = %d!\n", sumbef, sumaft);

	

	return 0;
} 
