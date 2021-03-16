#include <stdio.h>
#include <stdlib.h>
#define INF 99999

// inicia uma matriz vazia
int **init_matrix(int m, int n);

// inicia uma matriz aleatória
int **rand_matrix(int n);

// imprime a matriz em um arquivo
void fprint_matrix(char *filename, int **matrix, int matrix_size);