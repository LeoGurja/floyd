#include <stdio.h>
#include <stdlib.h>
#define INF 99999

// inicia uma matriz vazia
int **init_matrix(int m, int n);

int **copy_matrix(int **matrix, int n);

int **rand_matrix(int n);

void print_matrix(int **matrix, int n);

void println(int *line, int n);

void fprint_matrix(int **matrix, int m, int n, FILE *output);

void fprintln(int *line, int n, FILE *output);