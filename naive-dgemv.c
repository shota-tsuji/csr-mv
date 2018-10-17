#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int num = 4;
	double *vec_x = (double*) malloc(num * sizeof(double));
	size_t size = num * num * sizeof(double);
	double *mat_A = (double*) malloc(size);
	// initialize vec_x and mat_A
	for(int i = 0; i < num; ++i){
		vec_x[i] = i;
	}
	for(int i = 0; i < num*num; ++i){
		mat_A[i] = i;
		//printf("mat_A[%d] = %lf\n", i, mat_A[i]);
	}
	double *vec_y = (double*) calloc(num, sizeof(double));
	// calculate dgemv
	for(int i = 0; i < num; ++i){
		for(int j = 0; j < num; ++j){
			vec_y[i] += mat_A[i * num + j] * vec_x[j];
		}
	}
	// print the result vector
	for(int i = 0; i < num; ++i){
		printf("result[%d] = %lf\n", i, vec_y[i]);
	}
	return 0;
}
