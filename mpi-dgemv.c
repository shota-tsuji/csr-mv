#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int num = 4;
	double *vec_x = (double*) malloc(num * sizeof(double));
	MPI_Init(&argc, &argv);
	// get myrank
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	// initialize vector by root process
	if(myrank == 0){
		for(int i = 0; i < num; ++i){
			vec_x[i] = i;
		}
	}
	// send from root and receive on other processes
	//MPI_Scatter(vec_x, num, MPI_DOUBLE, vec_x, num, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec_x, num, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(myrank == 1){
		for(int i = 0; i < num; ++i){
			printf("vec_x[%d] = %lf\n", i, vec_x[i]);
		}
	}
	MPI_Finalize();
	return 0;
}
