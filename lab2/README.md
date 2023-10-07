课程名：高性能计算应用实践

姓名：梁彬

学号：220210611

学期：2023秋

----

## lab5、lab6

----

### 实验环境：

系统: Ubuntu22.04

CPU: 12th Gen Intel(R) Core(TM) i7-12700H 14核 28线程

--- 

### 实验方案

1. pthread采用分块法，根据cpu的28个进程将矩阵分成28块，每块占一个线程运算。

2. OpenMP方法也是运用分块法优化，for循环处采用多线程运算。

3. naive法未做过多调整。

### 关键代码：

基于pthread实现多线程dgemm

```cpp
\\dgemm.c
void trans() {\\转秩后可以利用连续内存的优化
    double *tmp = (double*) malloc(sizeof(double) * n * k);
    for (int i = 0; i < k; ++i)
        for (int j = 0; j < n; ++j)
            tmp[i * n + j] = B[j * k + i];
    free(B); 
    B = tmp;
}

void *pthread_mult(void *id) {\\分块优化
    long tid = (long) id;
    int l = tid * block;
    int r = (tid + 1) * block - 1;
    r = min(r, m - 1);
    for (int i = l; i <= r; ++i)
        for (int j = 0; j < k; ++j) {
            double sum = 0;
            for (int z = 0; z < n; ++z)
                sum += A[i * n + z] * B[j * n + z];
            C[i * k + j] = sum;
        }
}

int tnum = 28;
block = (m + tnum - 1) / tnum;
pthread_t *thread_handles = (pthread_t*) malloc(tnum * sizeof(pthread_t));
for (int i = 0; i < tnum; ++i)
    pthread_create(&thread_handles[i], NULL, pthread_mult, (void *) i);
for (int i = 0; i < tnum; ++i)
    pthread_join(thread_handles[i], NULL);
```

基于OpenMP多线程优化

```cpp
void mult(int tid) {\\分块优化+多线程
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
```

naive法

```cpp
void naive_dgemm(double *A, double *B, double *C, int m, int n, int k) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int z = 0; z < k; ++z)
                C[i * k + z] += A[i * n + j] * B[j * k + z];
}
```

---

### 运行结果及比较

pthread

n = m = k = 2048

![](https://github.com/lb2003/hpc/tree/main/photo/p2048.png)

n = m = k = 1024

![](https://github.com/lb2003/hpc/tree/main/photo/p1024.png)

n = m = k = 256

![](https://github.com/lb2003/hpc/tree/main/photo/p256.png)

n = m = k = 64

![](https://github.com/lb2003/hpc/tree/main/photo/p64.png)

n = m = k = 16

![](https://github.com/lb2003/hpc/tree/main/photo/p16.png)

OpenMp

n = m = k = 2048

![](https://github.com/lb2003/hpc/tree/main/photo/o2048.png)

n = m = k = 1024

![](https://github.com/lb2003/hpc/tree/main/photo/o1024.png)

n = m = k = 256

![](https://github.com/lb2003/hpc/tree/main/photo/o256.png)

n = m = k = 64

![](https://github.com/lb2003/hpc/tree/main/photo/o64.png)

n = m = k = 16

![](https://github.com/lb2003/hpc/tree/main/photo/o16.png)

naive

n = m = k = 2048

![](https://github.com/lb2003/hpc/tree/main/photo/n2048.png)

n = m = k = 1024

![](https://github.com/lb2003/hpc/tree/main/photo/n1024.png)

n = m = k = 512

![](https://github.com/lb2003/hpc/tree/main/photo/n512.png)

n = m = k = 256

![](https://github.com/lb2003/hpc/tree/main/photo/n256.png)

n = m = k = 64

![](https://github.com/lb2003/hpc/tree/main/photo/n64.png)

n = m = k = 16

![](https://github.com/lb2003/hpc/tree/main/photo/n16.png)
