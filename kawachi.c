#include <stdio.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <stdbool.h>
#include "sha3.c"

#define _SUCCESS_ 0
#define _FAILED_ 1
#define N  512//3 // y-rows
#define M  2048//4 // x-cols
#define Q  257//3

int addVectors(int * result, int* v1, int* v2, int size);
int subVectors(int * result, int* v1, int* v2, int size);
int modVector(int* v, int len, int q);

int addMod(int a, int b, int q);
int subMod(int a, int b, int q);
int subVectorsMod(int result[N], int* v1, int* v2, int size, int q);
int addVectorsMod(int result[M], int* v1, int* v2, int size, int q);

void generateMatrixModQ(int rows, int cols, int matrix_A[][cols], int q);

int generateVector(int* v, int size, int  q);
void printVector(int *v, int size);

int vectorMultiplyMatrix(int *result, int *v, int rows, int cols, int m[][cols]);

int assignVectorValues(int *dest, int *source, int size);
bool areEqual(int *v1, int *v2, int size);

int keygen(int matrix_A[][M], int q, int sk_x[M], int pk_y[N]);
int p_coms(int c1[N], int c2[M], int c3[M], int rand_r[M], int matrix_A[][M], int sk_x[M]);
int v_challenge();
void p_params(int param1[M], int param2[M], int ch, int sk_x[M], int rand_r[M]);
int v_check(int c1[N], int c2[M], int c3[M], int ch, int param1[M], int param2[M], int matrix_A[][M], int sk_x[M], int pk_y[N], int rand_r[M]);
void printMatrix(int rows, int cols, int matrix_A[][cols]);


int modVector(int* v, int len, int q) {
    size_t i;
    for (i = 0; i < len; i++) {
        v[i] %= q;
        if (v[i] < 0) {
            v[i] += q;
        }
    }
    return 0;
}

int addVectors(int * result, int* v1, int* v2, int size)
{
    int i;
    for (i = 0; i < size; i++){
        result[i] = v1[i] + v2[i];
    }
    return 0;
}

int subVectors(int * result, int* v1, int* v2, int size)
{
    int i;
    for (i = 0; i < size; i++){
        result[i] = v1[i] - v2[i];
    }
    return 0;
}

int subMod(int a, int b, int q)
{
    if (a>=b)
        return a - b;
    else
        return q - b + a;
}

int addMod(int a, int b, int q)
{
	if ( a==0 ) return b;
	if ( b==0 ) return a;
  	
  	b = q - b;
  	if ( a>=b )
    	return a - b;
  	else
    	return q - b + a;
}

int addVectorsMod(int result[M], int* v1, int* v2, int size, int q)
{
    int i;
    for (i = 0; i < size; i++){
        result[i] = addMod(v1[i],v2[i],q); //(v1[i]%q + v2[i]%q + q)%q;
    }
    return 0;
}

int subVectorsMod(int result[N], int* v1, int* v2, int size, int q)
{
    //int* result = malloc(size * sizeof (int));
    int i;
    for (i = 0; i < size; i++){
        result[i] = subMod(v1[i],v2[i],q); //(v1[i]%q + v2[i]%q + q)%q;
    }
    return 0;
}

void generateMatrixModQ(int rows, int cols, int matrix_A[][cols], int  q) 
{
    size_t i, j;
    for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
        matrix_A[i][j] = rand()%q;
}

void printMatrix(int rows, int cols, int matrix_A[][cols]) 
{
	size_t i, j;
    for (i = 0; i < rows; i++){
      for (j = 0; j < cols; j++){
        printf("%3i ", matrix_A[i][j]);
    }
    printf("\n");
	}
}

void printVector(int *v, int size)
{
    size_t i;
    for (i = 0; i < size; i++){
        printf("%d ", v[i]);
	}
}

int generateVector(int* v, int size, int  q) 
{
    size_t i;
    for (i = 0; i < size; i++){
        v[i] = rand()%q;
    }
    return 0;
}

int vectorMultiplyMatrix(int *result, int *v, int rows, int cols, int m[][cols])
{
    size_t i, j;
    int total;
    for (i = 0; i < rows; i++){
        total=0;
        for (j = 0; j < cols; j++){
            total += m[i][j]*v[j];
        }
    //printf("%d ", total);
    result[i] = total;
    }
    return 0;
}

int assignVectorValues(int *dest, int *source, int size){
	int i;
	for(i=0; i<size; i++)
    {
        dest[i] = source[i];
    }
    return 0;
}

bool areEqual(int *v1, int *v2, int size){
	for (int i = 0; i < size; ++i)
	{
		if(v1[i] != v2[i]){
			return false;
		}
	}
	return true;
}

//TODO: comment ler yaz!!
int keygen(int matrix_A[][M], int q, int sk_x[M], int pk_y[N]){
	generateMatrixModQ(N, M, matrix_A, q);
	//printMatrix(N, M, matrix_A);

	generateVector(sk_x, M, q);
	//printVector(sk_x, M);

	vectorMultiplyMatrix(pk_y, sk_x, N, M, matrix_A);
    //printVector(pk_y, N);

    modVector(pk_y, N, Q);
    //printVector(pk_y, N);


	return 0;
}

//TODO: prover_commitments
int p_coms(int c1[N], int c2[M], int c3[M], int rand_r[M], int matrix_A[][M], int sk_x[M]){
	//printf("vector r:\n");
	generateVector(rand_r, M, Q);
	//printVector(rand_r, M);

	//printf("CC1:\n"); //TODO: printtteki yazilari duzenle!!
	vectorMultiplyMatrix(c1, rand_r, N, M, matrix_A);
	modVector(c1, N, Q);
	//printVector(c1, N);

	//printf("c2:\n");
	assignVectorValues(c2, rand_r, M);
	//printVector(c2, M);

	//printf("c3:\n");
	addVectors(c3, sk_x, rand_r, M);
	modVector(c3, M, Q);
	//printVector(c3, M);

	return 0;
}

int v_challenge() 
{
	int ch;
    srand(time(NULL));
    ch = rand()%3;
    //printf("%d\n", ch);
    return ch;
}

//TODO: M leri degistir!!
void p_params(int param1[M], int param2[M], int ch, int sk_x[M], int rand_r[M]){

	int sum_xr[M] = {0};
	addVectors(sum_xr, sk_x, rand_r, M);
	modVector(sum_xr, M, Q);
	//printVector(sum_xr, M);

    if(ch == 0){
        assignVectorValues(param1, sk_x, M);
        assignVectorValues(param2, rand_r, M);
    }else if(ch == 1){
        //assignVectorValues(param1, {0});
        assignVectorValues(param2, sum_xr, M);
    }else if(ch == 2){
        //assignVectorValues(param1, {0});
        assignVectorValues(param2, rand_r, M);
    }else{
        printf("Error: invalid challenge!\n");
    }
}

int v_check(int c1[N], int c2[M], int c3[M], int ch, int param1[M], int param2[M], int matrix_A[][M], int sk_x[M], int pk_y[N], int rand_r[M]){
	int result = 0;
	int sum_xr[M] = {0};
	addVectors(sum_xr, sk_x, rand_r, M);
	modVector(sum_xr, M, Q);

	int comp1[N] = {0};
	int comp1_res[N] = {0};
	vectorMultiplyMatrix(comp1, sum_xr, N, M, matrix_A);
	modVector(comp1, N, Q);
	//printVector(comp1, N);
	subVectorsMod(comp1_res, comp1, pk_y, N, Q);
	//subVectors(comp1_res, comp1, pk_y, N);
	//printVector(comp1, N);

	int comp2[N] = {0};
	vectorMultiplyMatrix(comp2, rand_r, N, M, matrix_A);
	modVector(comp2, N, Q);
	//printVector(comp2, N);

	if(ch == 0){
        if(areEqual(c2, param2, M) && areEqual(c3, sum_xr, M))
        	result = 1;
    }else if(ch == 1){
        if(areEqual(c1, comp1_res, N) && areEqual(c3, param2, M))
        	result = 1;
    }else if(ch == 2){
        if(areEqual(c1, comp2, N) && areEqual(c2, param2, M))
        	result = 1;
    }else{
        result = 0;
    }
    return result;
}

int main(void){
	//srand((unsigned int)time(NULL));
	int matrix_A[N][M] = {0};
	int sk_x[M] = {0};
	int pk_y[N] = {0};

	int c1[N] = {0};
	int c2[M] = {0};
	int c3[M] = {0};

	int rand_r[M] = {0};
	int ch;

	int param1[M] = {0};
	int param2[M] = {0};

    printf("--- Kawachi's ID Scheme ---\n");
    printf("start Key Generation...\n");
	keygen(matrix_A, Q, sk_x, pk_y);
    printf("finish Key Generation.\n");
    printf("Prover (Compute commitments) ...\n");
	p_coms(c1, c2, c3, rand_r, matrix_A, sk_x);
    printf("Verifier (Generate challenge) ...\n");
	ch = v_challenge();
	printf("Challenge: %d\n", ch);
    printf("Prover (Send some parameters) ...\n");
	p_params(param1, param2, ch, sk_x, rand_r);

	//printf("Param1:\n");
	//printVector(param1, M);

	//printf("Param2:\n");
	//printVector(param2, M);

	int result;
    printf("Verifier (Check the truthfulness) ...\n");
	result = v_check(c1, c2, c3, ch, param1, param2, matrix_A, sk_x, pk_y, rand_r);
	//printf("Result:%d\n", result);

	if(result == 1)
		printf("Success!\n");
	else
		printf("Failed!\n");	
}



