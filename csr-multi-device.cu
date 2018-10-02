#include <stdio.h>
#include "mmio.h"

// print vector values and device number
__global__ void print_vec_info(const int device_num, double* vec)
{
	int thrd_num = threadIdx.x;
	printf(" device: %d, vec[%d] = %lf\n", device_num, thrd_num, vec[thrd_num]);
}
// get the number of rows, cols and non-zero elements and set indices and values from Matrix Market file 
void read_mm(char* f_name, int* num_row, int* num_col, int* nnz, int* ind_row_coo, int* ind_col_coo, double* vals)
{
	FILE *fptr = fopen(f_name, "r");
	MM_typecode matcode;
	mm_read_banner(fptr, &matcode);
	if(mm_is_complex(matcode) && mm_is_matrix(matcode) && mm_is_sparse(matcode)){
		printf("Matrix Market type: [%s]\n", mm_typecode_to_str(matcode));
		exit(1);
	}
	mm_read_mtx_crd_size(fptr, num_row, num_col, nnz);
	size_t size = (*nnz) * sizeof(int);
	ind_row_coo = (int*)malloc(size);
	ind_col_coo = (int*)malloc(size);
	size = (*nnz) * sizeof(double);
	vals = (double*)malloc(size);
	for(int i = 0; i < (*nnz); ++i){
		fscanf(fptr, "%d %d %lf\n", &ind_row_coo[i], &ind_col_coo[i], &vals[i]);
		ind_row_coo[i]--;
		ind_col_coo[i]--;
	}
	fclose(fptr);
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		fprintf(stderr, "Usage: %s [matrix-market-filename]\n", argv[0]);
		exit(1);
	}

	// process Matrix Market file and set some values
	int num_row, num_col, nnz;
	int *ind_row_coo=NULL, *ind_col_coo=NULL;
	double* vals=NULL;
	read_mm(argv[1], &num_row, &num_col, &nnz, ind_row_coo, ind_col_coo, vals);

	// prepare vector-x and -y for y = Ax
	size_t size = num_col * sizeof(double);
	double* vec_x = (double*)malloc(size);
	for(int i = 0; i < num_col; ++i){
		vec_x[i] = 1.0;
	}
	printf("here\n");
	size = num_row * sizeof(double);
	double* vec_y = (double*)malloc(size);
	for(int i = 0; i < num_row; ++i){
		vec_y[i] = 0.0;
	}

	int num_device;
	cudaGetDeviceCount(&num_device);
	printf("num_device = %d\n", num_device);
	// allocate device memory for vec-x and -y
	//double *vec_x_d=NULL, *vec_y_d=NULL;
	//size = num_col * sizeof(double);
	//cudaMalloc(vec_x_d, size);
	double **vec_x_ds = (double**)malloc(num_device*sizeof(double*));
	double **vec_y_ds = (double**)malloc(num_device*sizeof(double*));
	for(int i = 0; i < num_device; ++i){
		cudaSetDevice(i);
		size_t size_x = num_col * sizeof(double);
		cudaMalloc(&vec_x_ds[i], size_x);
		cudaMemcpy(vec_x_ds[i], vec_x, size_x, cudaMemcpyHostToDevice);
		size_t size_y = num_row * sizeof(double);
		cudaMalloc(&vec_y_ds[i], size_y);
		cudaMemcpy(vec_y_ds[i], vec_y, size_y, cudaMemcpyHostToDevice);
	}

	// on each device print the vector and device number
	dim3 dimGrid(1, 1);
	dim3 dimBlock(num_col, 1);
	for(int i = 0; i < num_device; ++i){
		print_vec_info<<<dimGrid, dimBlock>>>(i, vec_x_ds[i]);
	}


	// free pointers allocated on each device
	for(int i = 0; i < num_device; ++i){
		cudaFree(vec_x_ds[i]);
		cudaFree(vec_y_ds[i]);
	}
	free(vec_y_ds);
	free(vec_x_ds);
	free(vec_y);
	free(vec_x);
	return 0;
}
