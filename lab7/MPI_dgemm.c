#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "f77blas.h"
#include "mpi.h"
#include <time.h>
#include <sys/time.h>

#define m 16
#define n 16
#define k 16

void Print(double *A) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%lf ", A[i * m + j]);
        puts("");
    }
}
 
int main(int argc, char **argv) {
    //freopen("a.in", "r", stdin);
    freopen("dg.ou", "w", stdout);

    int id, siz, block;
    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &siz);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Status status;
    
    //int m, n, k; scanf("%d%d%d", &m, &n, &k);
    block = m / siz;
    double *A = (double*) malloc(sizeof(double) * m * n);
    double *B = (double*) malloc(sizeof(double) * n * k);
    double *C = (double*) malloc(sizeof(double) * m * k);
    double *buf_A = (double*) malloc(sizeof(double) * block * n);
    double *tmp = (double*) malloc(sizeof(double) * block * k);

    
    if (id == 0) {
        //派发
        struct timeval start, finish;
    	double duration;
        /*for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                scanf("%lf,", &A[i * n + j]);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < k; ++j)
                scanf("%lf,", &B[i * k + j]);*/
                
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
            	A[i * n + j] = rand() % 512;
    	for (int i = 0; i < n; ++i)
            for (int j = 0; j < k; ++j)
            	B[i * k + j] = rand() % 512;
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < k; ++j)
                C[i * k + j] = 0;

        //Print(A);
        //Print(B);
        //printf("block : %d\nsiz : %d\n", block, siz);
    	gettimeofday(&start, NULL);
        for (int i = 1; i < siz; ++i) {
            MPI_Send(A + (i - 1) * block * n, block * n, MPI_DOUBLE, i, 66, MPI_COMM_WORLD);
        } MPI_Bcast(B, n * k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //接收
        for (int t = 1; t < siz; ++t) {
            MPI_Recv(tmp, block * k, MPI_DOUBLE, t, 33, MPI_COMM_WORLD, &status);
            for (int i = 0; i < block; ++i)
                for (int j = 0; j < k; ++j)
                    C[((t - 1) * block + i) * k + j] = tmp[i * k + j]; 
        } for (int i = (siz - 1) * block; i < m; ++i) {
            for (int j = 0; j < k; ++j) {
                double w = 0;
                for (int p = 0; p < n; ++p)
                    w += A[i * n + p] * B[p * k + j];
                C[i * k + j] = w;
            }
        } 
        
        gettimeofday(&finish, NULL);
    	duration = ((double)(finish.tv_sec-start.tv_sec)*1000000 + (double)(finish.tv_usec-start.tv_usec)) / 1000000;

    	double gflops = 2.0 * m * n * k;
    	gflops = gflops / duration * 1.0e-6 / 1000;
    	printf("%d:\nduration: %lf\nGflops: %lf\n", n, duration, gflops);
        /*for (int i = 0; i < m; ++i) {
            for (int j = 0; j < k; ++j)
                printf("%lf ", C[i * k + j]); 
            puts("");
        }*/
        free(A);
        free(B);
        free(C);
        free(buf_A);
        free(tmp);
    } else {
        MPI_Recv(buf_A, block * n, MPI_DOUBLE, 0, 66, MPI_COMM_WORLD, &status);
        MPI_Bcast(B, n * k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        /*printf("id:%d\n", id);
        for (int i = 0; i < block; ++i)
            for (int p = 0; p < n; ++p) {
                printf("%lf ", buf_A[i * n + p]);
            }
        puts("");*/
        for (int i = 0; i < block; ++i)
            for (int j = 0; j < k; ++j) {
                double w = 0;
                for (int p = 0; p < n; ++p) 
                    w += buf_A[i * n + p] * B[p * k + j];
                tmp[i * k + j] = w;
            }
        MPI_Send(tmp, block * k, MPI_DOUBLE, 0, 33, MPI_COMM_WORLD);
    } MPI_Finalize();

    
    return 0;
}
