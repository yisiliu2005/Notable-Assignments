// CODE: include necessary library(s)
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

void deallocateMatrix(double **matrix , int rows)
{
    for (int i = 0; i < rows; i++) 
    {
        free(matrix[i]); 
    }
    free(matrix);
}

// Function to generate a random matrix
void generateMatrix(double **matrix, int rows, int cols) 
{
    // CODE: Generate random numbers between -10 and 10

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = (rand()%20000000)/(double)1000000 - 10;
        }
    }
}

// Function to print a matrix
void printMatrix(double **matrix, int rows, int cols) {
    // CODE: to print the matrix 

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%lf ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function to add two matrices
void addMatrices(int N1, int M1, double **A, int N2, int M2, double **B, double **result)
{   // CODE: check for the condition 
    if (N1 != N2 || M1 != M2)
    {
        printf("Matrix sizes incompatible for addition. Result matrix will be random numbers or a system default.\n");
        return;
    }
    
    for (int i = 0; i < N1; i++)
    {
        for (int j = 0; j < M1; j++)
        {
            result[i][j] = A[i][j] + B[i][j];
        }
    }
}

void subtractMatrices(int N1, int M1, double **A, int N2, int M2, double **B, double **result)
{   // CODE: check for the condition 
    if (N1 != N2 || M1 != M2)
    {
        printf("Matrix sizes incompatible for subtraction. Result matrix will be random numbers or a system default.\n");
        return;
    }
    
    for (int i = 0; i < N1; i++)
    {
        for (int j = 0; j < M1; j++)
        {
            result[i][j] = A[i][j] - B[i][j];
        }
    }
}

void multiplyMatrices(int N1, int M1, double **A, int N2, int M2, double **B, double **result)
{
    if (M1 !=  N2)
    {
        printf("Matrix sizes incompatible for multiplication. Result matrix will be random numbers or a system default.\n");
        return;
    }
    
    for (int i = 0; i < N1; i++)
    {
        for (int j = 0; j < M2; j++)
        {
            double sum = 0;
            for (int k = 0; k < M1; k++)
            {
                sum += (A[i][k] * B[k][j]);
            }
            result[i][j] = sum;
        }
    }    

}

//creating forward-declared helper functions for row operations seperately to keep organized:
void interchange(int rows, int cols, double **matrix, int idx1, int idx2);

void multiply(int rows, int cols, double **matrix, int idx, double constant);

void addrow(int rows, int cols, double **matrix, int idx1, int idx2, double constant);

void solveLinearSystem(int N1, int M1, double **A, int N2, int M2, double **B, double **x)
{
    if (N1 != N2 || M2 != 1 || M1 != N1) //if A isn't square then there is no unique solution
    {
        printf("Matrix sizes incompatible for obtaining a solution for a linear system. Result matrix will be random numbers or a system default.\n");
        return;
    }

    double **aug = (double **)malloc(N1 * sizeof(double *)); 
    for (int i = 0; i < N1; i++) 
    {
        aug[i] = (double *)malloc((M1+1) * sizeof(double)); 
    }
    //double aug[N1][M1+1]; //augmenting A with B
    for (int i = 0; i < N1; i++)
    {
        for (int j = 0; j < M1; j++)
        {
            aug[i][j] = A[i][j];
        }
    }    
    for (int i = 0; i < N2; i++)
    {
        aug[i][M1] = B[i][0];//actual index of the last extra row is M1 + 1 - 1
    }
     
    for (int i = 0; i < N1; i++)//essentially iterating down the diagonal of A since it's square to perform Gaussian Elimination on the augmented matrix where B tags along
    {    
        multiply(N1, (M1+1), aug, i, (1/aug[i][i]));
        for (int j = i+1; j < M1; j++)
        {
            addrow(N1, (M1+1), aug, i, j, (-aug[j][i]));
        }
        
    }
    
    for (int i = (N1 - 1); i >= 0; i--)//now for the backwards phase
    {
        for (int j = i-1; j >= 0; j--)
        {
            addrow(N1, (M1+1), aug, i, j, (-aug[j][i]));
        }   
    }
    
    for (int i = 0; i < N2; i++)//the last column that tagged along is now the answer
    {
        x[i][0] = aug[i][M1];
    }  

    deallocateMatrix(aug,N1);   
}

void interchange(int rows, int cols, double **matrix, int idx1, int idx2)
{
    double spare[cols];

    for (int j = 0; j < cols; j++)
    {
        spare[j] = matrix[idx2][j];
    }

    for (int j = 0; j < cols; j++)
    {
        matrix[idx2][j] = matrix[idx1][j];
    }
    
    for (int j = 0; j < cols; j++)
    {
        matrix[idx2][j] = spare[j];
    }
}

void multiply(int rows, int cols, double **matrix, int idx, double constant)
{
    for (int j = 0; j < cols; j++)
    {
        matrix[idx][j] = (matrix[idx][j])*constant;
    }
}

void addrow(int rows, int cols, double **matrix, int idx1, int idx2, double constant)
{
    for (int j = 0; j < cols; j++)
    {
        matrix[idx2][j] += (matrix[idx1][j])*constant;
    }
}