#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int num = 4;
	double *vec_x = (double*) malloc(num * sizeof(double));
	//size_t size = num * num * sizeof(double);
	double *mat_A = (double*) calloc(num * num, sizeof(double));
	// initialize vec_x and mat_A
	for(int i = 0; i < num; ++i){
		vec_x[i] = i;
	}
	/*for(int i = 0; i < num*num; ++i){
		mat_A[i] = i;
		//printf("mat_A[%d] = %lf\n", i, mat_A[i]);
	}*/
	mat_A[0] = 1.0; mat_A[2] = 2.0;
	mat_A[4] = 3.0; mat_A[5] = 4.0;
	mat_A[10]= 5.0; mat_A[11]= 6.0;
	mat_A[12]= 7.0; mat_A[15]= 8.0;
	/*for(int i = 0; i < num; ++i){
		for(int j = 0; j < num; ++j){
			printf("%lf\t", mat_A[i * num + j]);
		}
		printf("\n");
	}*/

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
