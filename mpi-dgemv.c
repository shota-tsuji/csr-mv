#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int num = 4;
	double *vec_x = (double*) malloc(num * sizeof(double));
	// prepare dense matrix
	size_t size = num * num * sizeof(double);	
	double *mat_A = (double*) malloc(size);
	MPI_Init(&argc, &argv);
	int numproc;
	MPI_Comm_size(MPI_COMM_WORLD, &numproc);
	printf("numproc = %d\n", numproc);
	// prepare sub-matrix hold on each processes
	size = (num / numproc) * num * sizeof(double); //(row) * col * bytes
	double *sub_A = (double*) malloc(size);
	// get myrank
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	// initialize vector by root process
	if(myrank == 0){
		for(int i = 0; i < num; ++i){
			vec_x[i] = i;
		}
		for(int i = 0; i < num*num; ++i){
			mat_A[i] = i;
		}
	}
	// send from root and receive on other processes
	MPI_Bcast(vec_x, num, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(mat_A, (num/numproc)*num, MPI_DOUBLE, sub_A, (num/numproc)*num, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(myrank == 1){
		for(int i = 0; i < num; ++i){
			printf("vec_x[%d] = %lf\n", i, vec_x[i]);
		}
	}
	for(int i = 0; i < (num/numproc)*num; ++i){
		printf("rank = %d, sub_A[%d] = %lf\n", myrank, i, sub_A[i]);
	}

	// prepare result sub-vector
	int num_row = num / numproc; // the number of sub-matrix row
	size = num_row * sizeof(double);
	double* sub_vec_y = (double*) calloc(num_row, sizeof(double));
	// calculate dgemv
	for(int i = 0; i < num_row; ++i){
		for(int j = 0; j < num; ++j){
			sub_vec_y[i] += sub_A[i * num + j] * vec_x[j];
		}
	}
	for(int i = 0; i < num_row; ++i){
		printf("rank = %d, result[%d] = %lf\n", myrank, i, sub_vec_y[i]);
	}
	// gather sub-result-vector
	double *vec_y;
	if(myrank == 0){
		vec_y = (double*) malloc(num * sizeof(double));
	}
	MPI_Gather(sub_vec_y, num_row, MPI_DOUBLE, vec_y, num_row, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(myrank == 0){
		for(int i = 0; i < num; ++i){
			printf("result[%d] = %lf\n", i, vec_y[i]);
		}
	}
	MPI_Finalize();
	free(sub_A);
	free(mat_A);
	free(vec_x);
	return 0;
}
