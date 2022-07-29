/*
*				In His Exalted Name
*	Title:	Pseudo-Timsort Sequential Code
*	Author: Ahmad Siavashi, Email: siavashi@aut.ac.ir
*	Date:	24/11/2015
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#define MAX(A, B) ((A)>(B))?(A):(B)
#define MIN(A, B) ((A)<(B))?(A):(B)

#define segment_size 4



void printArray(int *array, int size);
void fillArray(int *array, int size);
void merge(int *a, int n, int m);
void mergeSort(int *a, int n);
void insertionSort(int *a, int n);
void p_timsort(int *a, int n);
void s_timsort(int *a, int n);
void t_timsort(int *a, int n);

int n_threads;


int main(int argc, char *argv[]){
	double t_start, t_end, t_elapsed,
	sum,speedup,
	avg_s_timsort,
	avg_merge_sort,
	avg_p_timsort,
	avg_t_timsort;
	
	int *array = NULL;
	int size = 0;
	if (argc < 2){
		printf("[-] Invalid No. of arguments.\n");
		printf("[-] Try -> <size_of_array> <#threads>\n");
		printf(">>> ");
		scanf("%d %d", &size, &n_threads);
	}
	else{
		size = atoi(argv[1]);
		n_threads = atoi(argv[2]);
	}
	posix_memalign((void**)&array,64,sizeof(int)* size);
	
	sum = 0;
	for (int i=1 ; i<= 1 ; i++)
	{
	fillArray(array, size);
	t_start = omp_get_wtime();
	mergeSort(array, size);
	t_end = omp_get_wtime();
	t_elapsed = t_end - t_start;
	printf("\nmerge sort  -  run(%d) [time : %f]",i,t_elapsed);
	sum = sum+t_elapsed;
	}
	avg_merge_sort = sum;
	printf("\n[Average merge sort time : %f ] --- enter 'd' to print data... ",avg_merge_sort);
	if (getchar() == 'd')
	{
		printArray(array, size);
	}
	
	sum = 0;
	for (int i=1 ; i<= 1 ; i++)
	{
	fillArray(array, size);
	t_start = omp_get_wtime();
	s_timsort(array, size);
	t_end = omp_get_wtime();
	t_elapsed = t_end - t_start;
	printf("\nserial tim sort  -  run(%d) [time : %f]",i,t_elapsed);
	sum = sum+t_elapsed;
	}
	avg_s_timsort = sum;
	speedup = avg_merge_sort/avg_s_timsort ;
	printf("\n[Average serial tim sort time : %f -- Speedup(vs merge sort): %f] --- enter 'd' to print data... ",avg_s_timsort,speedup);
	if (getchar() == 'd')
	{
		printArray(array, size);
	}


	sum = 0;
	for (int i=1 ; i<= 1 ; i++)
	{
	fillArray(array, size);
	t_start = omp_get_wtime();
	p_timsort(array, size);
	t_end = omp_get_wtime();
	t_elapsed = t_end - t_start;
	printf("\nparllel tim sort  -  run(%d) [time : %f]",i,t_elapsed);
	sum = sum+t_elapsed;
	}
	avg_p_timsort = sum;
	speedup = avg_s_timsort/avg_p_timsort;
	printf("\n[Average parallel tim sort time : %f -- Speedup(vs serial tim sort): %f] --- enter 'd' to print data... ",avg_merge_sort,speedup);
	if (getchar() == 'd')
	{
		printArray(array, size);
	}


	
	sum = 0;
	for (int i=1 ; i<= 1 ; i++)
	{
	fillArray(array, size);
	t_start = omp_get_wtime();
	omp_set_num_threads(n_threads);
#pragma omp parallel 
{
#pragma omp single nowait
{
	t_timsort(array, size);
}
}
	t_end = omp_get_wtime();
	t_elapsed = t_end - t_start;
	printf("\nparallel task based tim sort  -  run(%d) [time : %f]",i,t_elapsed);
	sum = sum+t_elapsed;
	}
	avg_t_timsort = sum;
	speedup = avg_s_timsort/avg_t_timsort;
	printf("\n[Average parallel task based tim sort time : %f -- Speedup(vs serial tim sort): %f] --- enter 'd' to print data... ",avg_t_timsort,speedup);
	if (getchar() == 'd')
	{
		printArray(array, size);
	}


		



	free(array);
	system("PAUSE");
	return EXIT_SUCCESS;
}

void fillArray(int *array, int size){
	srand(time(NULL));
	while (size-->0){
		*array++ = rand() % 100;
	}
}

void printArray(int *array, int size){
	while (size-->0){
		printf("%d, ", *array++);
	}
	printf("\n");
}

void insertionSort(int *a, int n){
	int i, j, temp;
	for (i = 1; i < n; i++) {
		temp = a[i];
		for (j = i; j > 0 && temp < a[j - 1]; j--) {
			a[j] = a[j - 1];
		}
		a[j] = temp;
	}
}

void merge(int *a, int n, int m){
	int i, j, k;
	int *temp;
	posix_memalign((void**)&temp,64,n * sizeof (int));
	for (i = 0, j = m, k = 0; k < n; k++) {
		temp[k] = j == n ? a[i++]
			: i == m ? a[j++]
			: a[j] < a[i] ? a[j++]
			: a[i++];
	}
	for (i = 0; i < n; i++) {
		a[i] = temp[i];
	}
	free(temp);
}

void mergeSort(int *a, int n){
	int m;
	if (n < 2)
		return;
	m = n / 2;
	mergeSort(a, m);
	mergeSort(a + m, n - m);
	merge(a, n, m);
}


void p_timsort(int *a, int n)
{
	int n_segments = n / segment_size;
	omp_set_num_threads(n_threads);
#pragma omp parallel for schedule(static) shared(n_segments)
	for (int i = 0; i < n_segments; i++)
	{
		insertionSort((a + i*segment_size), segment_size);
	}
	//at this point we have number of array segments all sorted internally
	// then we should merge this parts to build completely sorted array
	int l = segment_size;
	while (l < n)
	{
		n_segments = n / (2 * l);

#pragma omp parallel for schedule(static) shared(l,n_segments)
		for (int i = 0; i < n_segments; i++)
		{
			merge(a + i * 2 * l, 2 * l, l);
		}
		l = l * 2;
	}
}


void s_timsort(int *a, int n)
{
	int m;
	if (n < 16)
	{
		insertionSort(a, n);
		return;
	}
	m = n / 2;
	mergeSort(a, m);
	mergeSort(a + m, n - m);
	merge(a, n, m);
}



void t_timsort(int *a, int n)
{
	int m;
	if (n < segment_size)
	{
#pragma omp task firstprivate(a,n)
		insertionSort(a, n);
#pragma omp taskwait
		return;
	}
	m = n / 2;
#pragma omp task firstprivate(a,m)
	mergeSort(a, m);
#pragma omp task firstprivate(a,m)
	mergeSort(a + m, n - m);
#pragma omp taskwait
	merge(a, n, m);
}
