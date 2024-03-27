#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Lab3IO.h"
#include "timer.h"

int main(int argc, char *argv[]){
    assert((argc == 2));
    int num_threads = atoi(argv[1]);
    double **A;
    int mat_size;
    Lab3LoadInput(&A, &mat_size);
    double pivot_global;
    int pivot_idx_global;
    double start, end;
    GET_TIME(start);
    #pragma omp parallel num_threads(num_threads)
    {
        int j;
        for(j = 0; j < mat_size; j++){
            // pivoting
            #pragma omp single
            {
                pivot_global = A[j][j];
                pivot_idx_global = j;
            }
            double pivot_local = pivot_global;
            int pivot_idx_local = pivot_idx_global;
            int i;
            #pragma omp for
            for(i = j + 1; i < mat_size; i++){
                if(abs(A[i][j]) > abs(pivot_local)){
                    pivot_local = A[i][j];
                    pivot_idx_local = i;
                }
            }
            
            if(abs(pivot_local) > abs(pivot_global)){
                #pragma omp critical
                if(abs(pivot_local) > abs(pivot_global)){
                    pivot_global = pivot_local;
                    pivot_idx_global = pivot_idx_local;
                }
            }

            // necessary
            #pragma omp barrier

            #pragma omp single
            {
                // move the pivot row
                double *temp = A[j];
                A[j] = A[pivot_idx_global];
                A[pivot_idx_global] = temp;
            }

            // subtract ratio*pivot_row from other rows to make the
            // lower part of the colomn zero
            int k;
            #pragma omp for
            for(i = j + 1; i < mat_size; i++){
                double ratio = A[i][j] / A[j][j];
                for(k = j; k < mat_size + 1; k++){
                    A[i][k] -= ratio * A[j][k];
                }
            }
            // implict barrier here
        }



        // make the upper triangle zero
        for(j = mat_size - 1; j > 0; j--){
            int i;
            #pragma omp for 
            for(i = 0; i < j; i++){
                double ratio = A[i][j] / A[j][j];
                A[i][mat_size] -= ratio * A[j][mat_size];
            }
        }
    }
    GET_TIME(end);

    double *x = malloc(sizeof(double) * mat_size);
    int i;
    for(i = 0; i < mat_size; i++){
        x[i] = A[i][mat_size] / A[i][i];
    }
    Lab3SaveOutput(x, mat_size, end-start);
    
    // clean-up
    for(i = 0; i < mat_size; i++){
        free(A[i]);
    }
    free(A);
    free(x);
    return 0;
}
