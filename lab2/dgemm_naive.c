#include <stdio.h>

#define M 2048
#define N 2048
#define K 2048


void dgemm(int m, int n, int k, int beta,
          double A[][N], double B[][M], double C[][K]){
    for(int i=0; i< m;i ++){    //C[i] 
        for(int j=0; j< n; j++){  //C[i][j]
            C[i][j] = beta*C[i][j];
            for(int p=0; p< k; p++){  
                C[i][j] += A[i][p]*B[p][j]; 
             }
        }
    }
}

void printf_matrix(int row, int col, double matrix[N][M] ){
  for(int i=0; i<row; i++){
    for(int j=0; j<col;j++){
        printf("%lf ", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n\n");
}
double A[M][N], B[M][N], C[M][N];



void main()
{
	freopen("a.in", "r", stdin);
	freopen("a.ou", "w", stdout);
	
	int n, m, k; scanf("%d%d%d", &m, &k, &n);
	//printf("%d %d %d\n", n,  m, k); 
	
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < k; ++j) {
			scanf("%lf", &A[i][j]);
			//printf("%lf\n", A[i][j]);
		}

	for (int i = 0; i < k; ++i)
		for (int j = 0; j < n; ++j)
			scanf("%lf", &B[i][j]);

    //C=A*B + beta*C
    dgemm(m,n,k,2, A,B,C);

    //printf_matrix(m,k,A);
    //printf_matrix(k,n,B);
    printf_matrix(m,n,C);

}


