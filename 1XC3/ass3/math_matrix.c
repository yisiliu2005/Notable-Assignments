// CODE: include necessary library(s)
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "functions.c"
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Check if the number of arguments is correct
    if (argc < 6 || argc > 7) {
        printf("Usage: %s nrows_A ncols_A nrows_B ncols_B <operation> [print]\n", argv[0]);
        return 1;
    }

    int print = 0;
    if (argc == 7)
    {
        print = 1;
    }

    int N1 = atoi(argv[1]);
    int M1 = atoi(argv[2]);
    int N2 = atoi(argv[3]);
    int M2 = atoi(argv[4]);
    // CODE: Generate random matrices A with size N1*M1 and B with size N2*M2
    
        double **A = (double **)malloc(N1 * sizeof(double *)); 
        double **B = (double **)malloc(N2 * sizeof(double *)); 
        double **resultx = (double **)malloc(N1 * sizeof(double *));
        double **resultadd = (double **)malloc(N1 * sizeof(double *));

        for (int i = 0; i < N1; i++) 
        {
            A[i] = (double *)malloc(M1 * sizeof(double)); 
            resultx[i] = (double *)malloc(M2 * sizeof(double)); 
            resultadd[i] = (double *)malloc(M1 * sizeof(double));   
        }
        for (int i = 0; i < N2; i++) {
            B[i] = (double *)malloc(M2 * sizeof(double));  
        }    

    generateMatrix(A, N1, M1);
    if (print) printMatrix(A, N1, M1);

    generateMatrix(B, N2, M2);
    if (print) printMatrix(B, N2, M2);

    // CODE: call the functions as needed
    if (strcmp(argv[5], "add") == 0)
    {
        addMatrices(N1, M1, A, N2, M2, B, resultadd);
        if (print) printMatrix(resultadd,N1, M1);
    }
    if (strcmp(argv[5], "subtract") == 0)
    {
        subtractMatrices(N1, M1, A, N2, M2, B, resultadd);
        if (print) printMatrix(resultadd, N1, M1);
    }

    if (strcmp(argv[5], "multiply") == 0)
    {
        multiplyMatrices(N1, M1, A, N2, M2, B, resultx);
        if (print) printMatrix(resultx,N1, M2);
    }
    if (strcmp(argv[5], "solve") == 0)
    {
        solveLinearSystem(N1, M1, A, N2, M2, B, resultx);
        if (print) printMatrix(resultx,N1, M2);
    }

    deallocateMatrix(A, N1);
    deallocateMatrix(B, N2);
    deallocateMatrix(resultadd, N1);
    deallocateMatrix(resultx, N1);

    return 0;
}