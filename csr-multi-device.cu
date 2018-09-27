#include <stdio.h>
#include "mmio.h"
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
	int num_row, num_col, nnz;
	int *ind_row_coo, *ind_col_coo;
	double* vals;
	read_mm(argv[1], &num_row, &num_col, &nnz, ind_row_coo, ind_col_coo, vals);

	return 0;
}
