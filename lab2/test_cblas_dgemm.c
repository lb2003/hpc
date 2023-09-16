#include <cblas.h>
#include <stdio.h>

#define N 2048
double A[N * N], B[N * N], C[N * N];
void main()
{
  int i=0;
  freopen("a.in", "r", stdin);
  freopen("a.ou", "w", stdout);
  int m, n, k; scanf("%d%d%d", &m, &n, &k);
  
  for (int i = 0; i < m; ++i)
  	for (int j = 0; j < k; ++j)
  		scanf("%lf", &A[i * k + j]);
 	
  for (int i = 0; i < k; ++i)
  	for (int j = 0; j < n; ++j)
  		scanf("%lf", &B[i * n + j]); 
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,m,n,k,1, A, m, B, k,2,C,n);

  for(i=0; i<k * n; i++)
    printf("%lf ", C[i]);
  printf("\n");
}
