课程名：高性能计算应用实践

姓名：梁彬

学号：220210611

学期：2023秋

----

## lab5,6,7

----

### 实验环境：

系统: Ubuntu22.04

CPU: 12th Gen Intel(R) Core(TM) i7-12700H 14核 28线程

--- 

### 实验方案

1. pthread采用分块法，根据cpu的28个进程将矩阵分成28块，每块占一个线程运算。

2. OpenMP方法也是运用分块法优化，for循环处采用多线程运算。

3. MPI方法采用主进程传递数据，分进程处理、回传数据。

4. naive法未做过多调整。

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

MPI法

```cpp
    int id, siz, block;
    MPI_Init(0, 0);
    MPI_Comm_size(MPI_COMM_WORLD, &siz);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Status status;


    if (id == 0) {
        //派发
        struct timeval start, finish;
        double duration;

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
        free(A);
        free(B);
        free(C);
        free(buf_A);
        free(tmp);
    } else {
        MPI_Recv(buf_A, block * n, MPI_DOUBLE, 0, 66, MPI_COMM_WORLD, &status);
        MPI_Bcast(B, n * k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int i = 0; i < block; ++i)
            for (int j = 0; j < k; ++j) {
                double w = 0;
                for (int p = 0; p < n; ++p) 
                    w += buf_A[i * n + p] * B[p * k + j];
                tmp[i * k + j] = w;
            }
        MPI_Send(tmp, block * k, MPI_DOUBLE, 0, 33, MPI_COMM_WORLD);
    } MPI_Finalize();
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

```
naive法

2048:
duration: 18.381240
Gflops: 0.934641

1024:
duration: 2.250964
Gflops: 0.954028

512:
duration: 0.283640
Gflops: 0.946395

256:
duration: 0.036726
Gflops: 0.913642

128:
duration: 0.006588
Gflops: 0.636658

64:
duration: 0.001601
Gflops: 0.327475

16:
duration: 0.000027
Gflops: 0.303407

pthread

2048:
duration: 1.967628
Gflops: 8.731259

1024:
duration: 0.232615
Gflops: 9.231922

512:
duration: 0.032020
Gflops: 8.383368

256:
duration: 0.007890
Gflops: 4.252780

128:
duration: 0.001578
Gflops: 2.657987

64:
duration: 0.001483
Gflops: 0.353532

16:
duration: 0.001448
Gflops: 0.005657

OpenMP
2048:
duration: 2.369842
Gflops: 7.249373

1024:
duration: 0.256667
Gflops: 8.366809

512:
duration: 0.039032
Gflops: 6.877317

256:
duration: 0.007734
Gflops: 4.338561

128:
duration: 0.003301
Gflops: 1.270616

64:
duration: 0.001284
Gflops: 0.408324

16:
duration: 0.001186
Gflops: 0.006907

MPI

2048:
duration: 12.910291
Gflops: 1.330711

1024:
duration: 0.824530
Gflops: 2.604494

512:
duration: 0.078124
Gflops: 3.436018

256:
duration: 0.009299
Gflops: 3.608391

128:
duration: 0.003077
Gflops: 1.363115

64:
duration: 0.000888
Gflops: 0.590414

16:
duration: 0.000230
Gflops: 0.035617
```

### 数据分析

![](https://github.com/lb2003/hpc/blob/main/photo/data.png)

![](https://github.com/lb2003/hpc/blob/main/photo/gflops.png)

在28个线程下，对程序的优化大致约为 6 - 8 倍，提速明显。

MPI单机效率实际上并不理想，但有利于多机器交互。

### 碰到的问题

1. pthread_create函数传参时遇到 int 类型报错

2. 在OpenMP实现中，会出现pragma指令放置在哪提速的问题

3. MPI实现过程中遇到数据越界的情况

### 解决过程

1. 将int类型改为long时不会出现该问题。

2. 首先放置在分块函数外，对每个分块函数调用，后来发现不行；改成对分块内for循环多进程后运行速度显著改善。

3. 找到了数据越界的位置，并进行了修改，成功通过所给数据。
