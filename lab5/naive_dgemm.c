#include <stdio.h> 
#include <assert.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


void naive_dgemm(double *A, double *B, double *C, int m, int n, int k) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int z = 0; z < k; ++z)
                C[i * k + z] += A[i * n + j] * B[j * k + z];
}

#define N 2050

double A[N * N], B[N * N], C[N * N];

int main() {
    freopen("a.in", "r", stdin);
    freopen("a.out", "w", stdout);
    srand(time(0));
    int m, n, k; scanf("%d%d%d", &m, &n, &k);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            A[i * n + j] = rand() % 25547;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < k; ++j)
            B[i * k + j] = rand() % 25547;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < k; ++j)
            C[i * k + j] = rand() % 25547;
    naive_dgemm(A, B, C, m, n, k);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            printf("%lf ", C[i * k + j]);
        }
        puts("");
    }
    return 0;
}
