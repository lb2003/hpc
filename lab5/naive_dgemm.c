#include <stdio.h> 
#include <assert.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "f77blas.h"


void naive_dgemm(double *A, double *B, double *C, int m, int n, int k) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int z = 0; z < k; ++z)
                C[i * k + z] += A[i * n + j] * B[j * k + z];
}

#define N 4100

double A[N * N], B[N * N], C[N * N];

int main() {
    freopen("a.in", "r", stdin);
    freopen("a.ou", "w", stdout);
    srand(time(0));
    int m, n, k; scanf("%d%d%d", &m, &n, &k);
    struct timeval start, finish;
    double duration;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            A[i * n + j] = rand() % 25547;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < k; ++j)
            B[i * k + j] = rand() % 25547;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < k; ++j)
            C[i * k + j] = rand() % 25547;
    gettimeofday(&start, NULL);
    naive_dgemm(A, B, C, m, n, k);
    gettimeofday(&finish, NULL);
    duration = ((double)(finish.tv_sec-start.tv_sec)*1000000 + (double)(finish.tv_usec-start.tv_usec)) / 1000000;

    double gflops = 2.0 * m * n * k;
    gflops = gflops / duration * 1.0e-6 / 1000;
    printf("%d:\nduration: %lf\nGflops: %lf\n", n, duration, gflops);
    /*for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            printf("%lf ", C[i * k + j]);
        }
        puts("");
    }*/
    return 0;
}
