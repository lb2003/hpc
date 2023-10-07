#include <stdio.h> 
#include <assert.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double *A, *B, *C;
int n, m, k;

void trans() {
    double *tmp = (double*) malloc(sizeof(double) * n * k);
    for (int i = 0; i < k; ++i)
        for (int j = 0; j < n; ++j)
            tmp[i * n + j] = B[j * k + i];
    free(B); 
    B = tmp;
}

int block;

int min(int x, int y) {
    return x > y ? y : x;
}

void mult(int tid) {
    int l = tid * block;
    int r = (tid + 1) * block - 1;
    r = min(r, m - 1);
    #pragma omp parallel for
    for (int i = l; i <= r; ++i)
        for (int j = 0; j < k; ++j) {
            double sum = 0;
            for (int z = 0; z < n; ++z)
                sum += A[i * n + z] * B[j * n + z];
            C[i * k + j] = sum;
        }
}

void solve() {
    srand(time(0));
    scanf("%d%d%d", &m, &n, &k);
    A = (double*) malloc(sizeof(double) * m * n);
    B = (double*) malloc(sizeof(double) * n * k);
    C = (double*) malloc(sizeof(double) * m * k);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            A[i * n + j] = rand() % 512;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < k; ++j)
            B[i * k + j] = rand() % 512;
    trans();
    int tnum = 28;
    block = (m + tnum - 1) / tnum;

    for (int i = 0; i < tnum; ++i)
        mult(i);
    
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            printf("%lf ", C[i * k + j]);
        } puts("");
    }
}

int main() {
    freopen("a.in", "r", stdin);
    freopen("a.ou", "w", stdout);
    solve();
    return 0;
}
