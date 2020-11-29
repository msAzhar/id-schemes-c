#include <stdio.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <stdbool.h>
#include "sha3.c"

#define _SUCCESS_ 0
#define _FAILED_ 1
#define N  677
#define Q  1024

struct SecretKey
{
    int x_h[N];
    int x_t[N];
};

struct  PublicKey
{
   int a_h[N];
   int a_t[N];
   int y[N]; 
};

struct Com_c1
{
    int y[N];
};

struct Com_c2
{
    int r_h[N];
    int r_t[N];
};

struct Com_c3
{
    int rx_h[N];
    int rx_t[N];
};

struct Commitments
{
    struct Com_c1 com_c1;
    struct Com_c2 com_c2;
    struct Com_c3 com_c3;

};

struct Params
{
    int resp_param1[N]; // 
    int resp_param2[N]; // 
    int resp_param3[N]; // 
    int resp_param4[N]; // 
    
};

typedef struct Commitments Coms;
typedef struct Params Resp_params;

typedef struct SecretKey SKparams;
typedef struct PublicKey PKparams;

int addVectors(int * result, int* v1, int* v2, int size);
int subVectors(int * result, int* v1, int* v2, int size);
int multVectors(int * result, int* v1, int* v2, int size);
int modVector(int* v, int len, int q);

int addMod(int a, int b, int q);
int subMod(int a, int b, int q);

void generateMatrixModQ(int rows, int cols, int matrix_A[][cols], int q);

int generateVector(int* v, int size, int  q);
void printVector(int *v, int size);

int vectorMultiplyMatrix(int *result, int *v, int rows, int cols, int m[][cols]);

int assignVectorValues(int *dest, int *source, int size);
bool areEqual(int *v1, int *v2, int size);

int keygen(PKparams *pk_ptr, SKparams *sk_ptr);
int p_coms(int r_h[N], int r_t[N], int rx_h[N], int rx_t[N], Coms *coms_ptr, SKparams *sk_ptr, PKparams *pk_ptr);
int v_challenge();
void p_params(int ch, Resp_params *params_ptr, int r_h[N], int r_t[N], int rx_h[N], int rx_t[N], SKparams *sk_ptr);
int v_check(int ch, Coms *coms_ptr, Resp_params *params_ptr, PKparams *pk_ptr);
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

int multVectors(int * result, int* v1, int* v2, int size)
{
    int i;
    for (i = 0; i < size; i++){
        result[i] = v1[i] * v2[i];
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
int keygen(PKparams *pk_ptr, SKparams *sk_ptr){
    int a_h[N] = {0};
    int a_t[N] = {0};

	generateVector(a_h, N, Q);
	//printVector(a_h, N);

    generateVector(a_t, N, Q);
    //printVector(a_t, N);

    int x_h[N] = {0};
    int x_t[N] = {0};

	size_t i;
    for(int i = 0; i < N/2; i++){
        x_h[i] = 1;
    }

    for(int i = N/2; i < N; i++){
        x_h[i] = 0;
    }
    //TODO: ass shuffle function

    x_t[0] = 1;

    for(int i = 1; i < N; i++){
        x_h[i] = 0;
    }

    int y[N] = {0};
    int ax_h[N] = {0};
    int ax_t[N] = {0};

    multVectors(ax_h, a_h, x_h, N);
    multVectors(ax_t, a_t, x_t, N);
    addVectors(y, ax_h, ax_t, N);

    assignVectorValues(pk_ptr->y, y, N); 

    //modVector(pk_y, N, Q);
    //printVector(pk_y, N);

    assignVectorValues(sk_ptr->x_h, x_h, N);
    assignVectorValues(sk_ptr->x_t, x_t, N); 
    
	return 0;
}

//TODO: prover_commitments
int p_coms(int r_h[N], int r_t[N], int rx_h[N], int rx_t[N], Coms *coms_ptr, SKparams *sk_ptr, PKparams *pk_ptr){
	generateVector(r_h, N, Q);
    generateVector(r_t, N, Q);
	//printVector(r_h, N);

    assignVectorValues(coms_ptr->com_c1.y, pk_ptr->y, N);

	assignVectorValues(coms_ptr->com_c2.r_h, r_h, N);
    assignVectorValues(coms_ptr->com_c2.r_t, r_t, N);

    addVectors(rx_h, r_h, sk_ptr->x_h, N);
    addVectors(rx_t, r_t, sk_ptr->x_t, N);

    assignVectorValues(coms_ptr->com_c3.rx_h, rx_h, N);
    assignVectorValues(coms_ptr->com_c3.rx_t, rx_t, N);

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
void p_params(int ch, Resp_params *params_ptr, int r_h[N], int r_t[N], int rx_h[N], int rx_t[N], SKparams *sk_ptr){

    if(ch == 0){
        assignVectorValues(params_ptr->resp_param1, sk_ptr->x_h, N);
        assignVectorValues(params_ptr->resp_param2, sk_ptr->x_t, N);
        assignVectorValues(params_ptr->resp_param3, r_h, N);
        assignVectorValues(params_ptr->resp_param4, r_t, N);
    }else if(ch == 1){
        assignVectorValues(params_ptr->resp_param3, rx_h, N);
        assignVectorValues(params_ptr->resp_param4, rx_t, N);
    }else if(ch == 2){
        assignVectorValues(params_ptr->resp_param3, r_h, N);
        assignVectorValues(params_ptr->resp_param4, r_t, N);
    }else{
        printf("Error: invalid challenge!\n");
    }
}

int v_check(int ch, Coms *coms_ptr, Resp_params *params_ptr, PKparams *pk_ptr){
	int result = 0;

	if(ch == 0){
        int rx_h[N] = {0};
        int rx_t[N] = {0};
        addVectors(rx_h, params_ptr->resp_param3, params_ptr->resp_param1, N);
        addVectors(rx_t, params_ptr->resp_param4, params_ptr->resp_param2, N);
        if(areEqual(coms_ptr->com_c2.r_h, params_ptr->resp_param3, N) && areEqual(coms_ptr->com_c2.r_t, params_ptr->resp_param4, N)
            && areEqual(coms_ptr->com_c3.rx_h, rx_h, N) && areEqual(coms_ptr->com_c3.rx_t, rx_t, N))
        	result = 1;
    }else if(ch == 1){
        if(areEqual(coms_ptr->com_c1.y, pk_ptr->y, N) && areEqual(coms_ptr->com_c3.rx_h, params_ptr->resp_param3, N)
            && areEqual(coms_ptr->com_c3.rx_t, params_ptr->resp_param4, N))
        	result = 1;
    }else if(ch == 2){
        if(areEqual(coms_ptr->com_c1.y, pk_ptr->y, N) && areEqual(coms_ptr->com_c2.r_h, params_ptr->resp_param3, N)
            && areEqual(coms_ptr->com_c2.r_t, params_ptr->resp_param4, N))
        	result = 1;
    }else{
        result = 0;
    }
    return result;
}

int main(void){
	//srand((unsigned int)time(NULL));
	SKparams sk_params;
    SKparams *sk_ptr = &sk_params;

    PKparams pk_params;
    PKparams *pk_ptr = &pk_params;

    Coms coms;
    Coms *coms_ptr = &coms;

    Resp_params params;
    Resp_params *params_ptr = &params;

    int r_h[N] = {0};
    int r_t[N] = {0};

    int rx_h[N] = {0};
    int rx_t[N] = {0};


    printf("--- Xagawa's ID Scheme ---\n");
    printf("start Key Generation...\n");
	keygen(pk_ptr, sk_ptr);
    printf("finish Key Generation.\n");
    printf("Prover (Compute commitments) ...\n");
    p_coms(r_h, r_t, rx_h, rx_t, coms_ptr, sk_ptr, pk_ptr);
    printf("Verifier (Generate challenge) ...\n");
	int ch = v_challenge();
	printf("Challenge: %d\n", ch);
    printf("Prover (Send some parameters) ...\n");
	p_params(ch, params_ptr, r_h, r_t, rx_h, rx_t, sk_ptr);

	int result;
    printf("Verifier (Check the truthfulness) ...\n");
	result = v_check(ch, coms_ptr, params_ptr, pk_ptr);
	//printf("Result:%d\n", result);

	if(result == 1)
		printf("Success!\n");
	else
		printf("Failed!\n");	
}



