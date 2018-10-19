#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	const int num = 4;
	const int MAT_SIZE = num * num;
	/*double mat_A[MAT_SIZE] =
	 {1, 0, 2, 0,
		3, 4, 0, 0,
		0, 0, 5, 6,
		7, 0, 0, 8};*/
	double *mat_A = (double*) calloc(MAT_SIZE, sizeof(double));
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
	double *vec_x = (double*) malloc(num * sizeof(double));
	MPI_Init(&argc, &argv);
	int myrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	int nnz = 0;
	if(myrank == 0){
		// initialize vec_x
		for(int i = 0; i < num; ++i){
			vec_x[i] = i;
		}
		// count non zero elements
		for(int i = 0; i < num*num; ++i){
			if(mat_A[i] != 0.0){
				nnz++;
			}
		}
		//printf("nnz = %d\n", nnz);
	}
	// broadcast the number of nnz
	MPI_Bcast(&nnz, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/*if(myrank == 1){
		printf("myrank == %d, nnz = %d\n", myrank, nnz);
	}*/
	// allocate arrays for val and col_ind
	size_t size = nnz * sizeof(double);
	double *val = (double*) malloc(size);
	size = nnz * sizeof(int);
	int *col_ind = (int*) malloc(size);
	int *row_ptr = (int*) calloc(num + 1, sizeof(int));
	if(myrank == 0){
		// prepare row_ptr
		row_ptr[0] = 0;
		// count non-zero value in i-th row and set it to row_ptr[i+1]
		for(int i = 0; i < num; ++i){
			int cnt_row_nnz = 0;
			for(int j = 0; j < num; ++j){
				if(mat_A[i * num + j] != 0.0){
					cnt_row_nnz++;
				}
			}
			row_ptr[i + 1] = cnt_row_nnz;
		}
		for(int i = 0; i < num; ++i){
			row_ptr[i + 1] += row_ptr[i];
		}
		/*for(int i = 0; i <= num; ++i){
			printf("row_ptr[%d] = %d\n", i, row_ptr[i]);
		}*/
		// prepare val and col_ind
		int idx = 0;
		for(int i = 0; i < num; ++i){
			for(int j = 0; j < num; ++j){
				if(mat_A[i * num + j] != 0.0){
					val[idx] = mat_A[i * num + j];
					col_ind[idx] = j;
					idx++;
				}
			}
		}
		/*for(int i = 0; i < nnz; ++i){
			printf("val[%d] = %lf, col_ind[%d] = %d\n", i, val[i], i, col_ind[i]);
		}*/
	}
	// broadcast arrays for csr
	MPI_Bcast(row_ptr, num + 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(val, nnz, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(col_ind, nnz, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec_x, num, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/*if(myrank == 1){
		for(int i = 0; i < nnz; ++i){
			printf("myrank = %d, val[%d] = %lf, col_ind[%d] = %d\n", myrank, i, val[i], i, col_ind[i]);
		}
	}*/
	// calculate csrmv
	int numproc;
	MPI_Comm_size(MPI_COMM_WORLD, &numproc);
	double *sub_vec_y = (double*) calloc(num / numproc, sizeof(double));
	for(int i = 0; i < (num / numproc); ++i){
		// specify the index for row_ptr
		int idx = myrank * (num / numproc) + i;
		for(int j = row_ptr[idx]; j < row_ptr[idx + 1]; ++j){
			sub_vec_y[i] += val[j] * vec_x[col_ind[j]];
	 	}
		//printf("myrank = %d, sub_vec_y[%d] = %lf\n", myrank, i, sub_vec_y[i]);
	}
	// gather all sub-result-vectors
	double *vec_y;
	if(myrank == 0){
		vec_y = (double*) malloc(num * sizeof(double));
	}
	MPI_Gather(sub_vec_y, num / numproc, MPI_DOUBLE, vec_y, num / numproc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(myrank == 0){
		for(int i = 0; i < num; ++i){
			//printf("myrank = %d, result[%d] = %lf\n", myrank, i, vec_y[i]);
			printf("result[%d] = %lf\n", i, vec_y[i]);
		}
	}
	
	MPI_Finalize();
	free(sub_vec_y);
	free(row_ptr);
	free(col_ind);
	free(val);
	free(vec_x);
	free(mat_A);
	return 0;
}
