#include <stdio.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <stdbool.h>
#include "sha3.c"

#define _SUCCESS_ 0
#define _FAILED_ 1
#define N  3 // y-rows
#define M  4 // x-cols
#define Q  3

struct Com_c1
{
    int r1[N];
};

struct Com_c2
{
    int Aus[N];
    int r2[N];
};

struct Com_c3
{
    int Aub[N];
    int r3[N];
};

struct Commitments
{
    struct Com_c1 com_c1;
    struct Com_c2 com_c2;
    struct Com_c3 com_c3;

};

struct Params
{
    int resp_param1[M]; // 
    int resp_param2[N]; // 
    int resp_param3[M]; // 
    int resp_param4[N]; // 
    
};

typedef struct Commitments Coms;
typedef struct Params Resp_params;


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

int keygen(int matrix_A[][M], int q, int sk_s[M], int pk_b[N], int errors[N]);
int p_coms(Coms *coms_ptr, int r1[N], int r2[N], int r3[N], int u[M], int us[M], int aus[N], int matrix_A[][M], int q, int sk_s[M], int pk_b[N], int errors[N]);
int v_challenge();
void p_params(Resp_params *params_ptr, int ch, int r1[N], int r2[N], int r3[N], int u[M], int us[M], int aus[N], int errors[N]);
int v_check(Coms *coms_ptr, Resp_params *params_ptr, int ch, int matrix_A[][M], int q, int pk_b[N]);
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
    //int* result = malloc(size * sizeof (int));;;
    int i;
    for (i = 0; i < size; i++){
        result[i] = v1[i] + v2[i];
    }
    return 0;
}

int subVectors(int * result, int* v1, int* v2, int size)
{
    //int* result = malloc(size * sizeof (int));;;
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
    //int* result = malloc(size * sizeof (int));
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

int generateErrors(int* v, int  size) // specific to Silva's scheme
{
    size_t i;
    for (i = 0; i < size/2; i++){
        v[i] = 1;
    }

    for (i = size/2; i < size; i++){
        v[i] = 0;
    }
    //TODO: shuffle result
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
	for(i=0; i < size; i++)
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
int keygen(int matrix_A[][M], int q, int sk_s[M], int pk_b[N], int errors[N]){
	generateMatrixModQ(N, M, matrix_A, q);
	//printMatrix(N, M, matrix_A);

	generateVector(sk_s, M, q);
	//printVector(sk_s, M);

	generateErrors(errors, N); //yarisi 1 yarisi 0
	//printVector(errors, N);
	//TODO: add Hw counter!!

	int temp_y[N] = {0};
	vectorMultiplyMatrix(temp_y, sk_s, N, M, matrix_A);
    //printVector(temp_y, N);

    modVector(temp_y, N, Q);
    //printVector(temp_y, N);

    addVectors(pk_b, temp_y, errors, N);
    modVector(pk_b, N, Q);
    //printVector(pk_b, N);
	return 0;
}

//TODO: prover_commitments
int p_coms(Coms *coms_ptr, int r1[N], int r2[N], int r3[N], int u[M], int us[M], int aus[N], int matrix_A[][M], int q, int sk_s[M], int pk_b[N], int errors[N]){
	//printf("vector r1:\n");
	generateVector(r1, N, Q);
	//printVector(r1, M);

    generateVector(r2, N, Q);
    generateVector(r3, N, Q);
    generateVector(u, M, Q);

    //printf("(u+s):\n");
    addVectors(us, u, sk_s, M);
    //printVector(us, M);


	//printf("A(u+s):\n"); 
	vectorMultiplyMatrix(aus, us, N, M, matrix_A);
	modVector(aus, N, Q);
	//printVector(aus, N);

    //coms icerigini doldurma:
    int au[N] = {0};
    int aub[N] = {0};
	//printf("Au:\n");
    vectorMultiplyMatrix(au, u, N, M, matrix_A);
    addVectors(aub, au, pk_b, N);
    modVector(aub, N, Q);
	
    assignVectorValues(coms_ptr->com_c1.r1, r1, N);

    assignVectorValues(coms_ptr->com_c2.Aus, aus, N);
    assignVectorValues(coms_ptr->com_c2.r2, r2, N);

    assignVectorValues(coms_ptr->com_c3.Aub, aub, N);
    assignVectorValues(coms_ptr->com_c3.r3, r3, N);
    //printVector(coms_ptr->com_c3.Aub, N);

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


void p_params(Resp_params *params_ptr, int ch, int r1[N], int r2[N], int r3[N], int u[M], int us[M], int aus[N], int errors[N]){

    if(ch == 0){
        assignVectorValues(params_ptr->resp_param1, r1, N);
        assignVectorValues(params_ptr->resp_param2, r2, N);
        assignVectorValues(params_ptr->resp_param3, us, M);
    }else if(ch == 1){
        assignVectorValues(params_ptr->resp_param1, r2, N);
        assignVectorValues(params_ptr->resp_param2, r3, N);
        assignVectorValues(params_ptr->resp_param3, aus, N);
        assignVectorValues(params_ptr->resp_param4, errors, N);
    }else if(ch == 2){
        assignVectorValues(params_ptr->resp_param1, r1, N);
        assignVectorValues(params_ptr->resp_param2, r3, N);
        assignVectorValues(params_ptr->resp_param4, u, M);
    }else{
        printf("Error: invalid challenge!\n");
    }
}

int v_check(Coms *coms_ptr, Resp_params *params_ptr, int ch, int matrix_A[][M], int q, int pk_b[N]){
	int result = 0;
	int computed_aus[N] = {0};
    vectorMultiplyMatrix(computed_aus, params_ptr->resp_param3, N, M, matrix_A);
    modVector(computed_aus, N, Q);
    //printVector(computed_aus, N);

	if(ch == 0){
        if(areEqual(coms_ptr->com_c1.r1, params_ptr->resp_param1, N) && areEqual(coms_ptr->com_c2.Aus, computed_aus, N) && areEqual(coms_ptr->com_c2.r2, params_ptr->resp_param2, N))
        	result = 1;
    }else if(ch == 1){
        int computed_aub[N] = {0};
        addVectors(computed_aub, params_ptr->resp_param3, params_ptr->resp_param4, N);
        modVector(computed_aub, N, Q);
        if(areEqual(coms_ptr->com_c2.Aus, params_ptr->resp_param3, N) && areEqual(coms_ptr->com_c2.r2, params_ptr->resp_param1, N) && areEqual(coms_ptr->com_c3.Aub, computed_aub, N) && areEqual(coms_ptr->com_c3.r3, params_ptr->resp_param2, N))
        	result = 1;
    }else if(ch == 2){
        int computed_au[N] = {0};
        int computed_aub[N] = {0};
        vectorMultiplyMatrix(computed_au, params_ptr->resp_param4, N, M, matrix_A);
        addVectors(computed_aub, computed_au, pk_b, N);
        modVector(computed_aub, N, Q);

        if(areEqual(coms_ptr->com_c1.r1, params_ptr->resp_param1, N) && areEqual(coms_ptr->com_c3.Aub, computed_aub, N) && areEqual(coms_ptr->com_c3.r3, params_ptr->resp_param2, N))
        	result = 1;
    }else{
        result = 0;
    }
    return result;
}

int main(void){
	//srand((unsigned int)time(NULL));
	int matrix_A[N][M] = {0};
	int sk_s[M] = {0};
	int pk_b[N] = {0};
	int errors[N] = {0};


	int r1[N] = {0};
	int r2[N] = {0};
	int r3[N] = {0};
	int u[M] = {0};

    int us[M] = {0};
    int aus[N] = {0};

	Coms coms;
    Coms *coms_ptr = &coms;
    int ch;

    Resp_params params;
    Resp_params *params_ptr = &params;

    printf("---Silva's ID Scheme---\n");
    printf("start Key Generation...\n");
	keygen(matrix_A, Q, sk_s, pk_b, errors);
    printf("finish Key Generation.\n");
    printf("Prover (Compute commitments) ...\n");
	p_coms(coms_ptr, r1, r2, r3, u, us, aus, matrix_A, Q, sk_s, pk_b, errors);
    printf("Verifier (Generate challenge) ...\n");
	ch = v_challenge();
	printf("Challenge: %d\n", ch);
    printf("Prover (Send some parameters) ...\n");
	p_params(params_ptr, ch, r1, r2, r3, u, us, aus, errors);

	int result;
    printf("Verifier (Check the truthfulness) ...\n");
	result = v_check(coms_ptr, params_ptr, ch, matrix_A, Q, pk_b);
	//printf("Result:%d\n", result);

	if(result == 1)
		printf("Success!\n");
	else
		printf("Failed!\n");	
}



