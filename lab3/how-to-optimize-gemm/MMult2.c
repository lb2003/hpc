/* Create macros so that the matrices are stored in column-major order */
#include <cblas.h>
#define A(i,j) a[ (j)*lda + (i) ]
#define B(i,j) b[ (j)*ldb + (i) ]
#define C(i,j) c[ (j)*ldc + (i) ]

/* Routine for computing C = A * B + C */

void MY_MMult( int m, int n, int k, double *a, int lda, 
                                    double *b, int ldb,
                                    double *c, int ldc )
{

  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans, m, n, k, 1, a, m, b, n, 1, c, m);
}
