课程名：高性能计算应用实践

姓名：梁彬

学号：220210611

学期：2023秋

----

## lab8

----

### 实验环境：

系统: Ubuntu22.04

CPU: 12th Gen Intel(R) Core(TM) i7-12700H 14核 20线程(实际测试14线程)

----

### CPU理论峰值计算

全核为单精度 2146Gflops

### 软件版本

openblas：0.3.20
openmpi: 4.1.2
gcc: 11.4.0

### 具体参数

```
HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
HPL.out      output file name (if any) 
6            device out (6=stdout,7=stderr,file)
1            # of problems sizes (N)
29236         Ns
1            # of NBs
192           NBs
0            PMAP process mapping (0=Row-,1=Column-major)
1            # of process grids (P x Q)
2            Ps 
7            Qs // 14 cores
32.0         threshold
1            # of panel fact
2            PFACTs (0=left, 1=Crout, 2=Right)
1            # of recursive stopping criterium
4            NBMINs (>= 1)
1            # of panels in recursion
2            NDIVs
1            # of recursive panel fact.
1            RFACTs (0=left, 1=Crout, 2=Right)
1            # of broadcast
1            BCASTs (0=1rg,1=1rM,2=2rg,3=2rM,4=Lng,5=LnM)
1            # of lookahead depth
1            DEPTHs (>=0)
2            SWAP (0=bin-exch,1=long,2=mix)
64           swapping threshold
0            L1 in (0=transposed,1=no-transposed) form
0            U  in (0=transposed,1=no-transposed) form
1            Equilibration (0=no,1=yes)
8            memory alignment in double (> 0)
##### This line (no. 32) is ignored (it serves as a separator). ######
0                               Number of additional problem sizes for PTRANS
1200 10000 30000                values of N
0                               number of additional blocking sizes for PTRANS
40 9 8 13 13 20 16 32 64        values of NB
```
先通过自动调参工具进行调整，发现最多只能14个进程。
之后微调参数作比较得出最优结果。

### 结果

```
T/V                N    NB     P     Q               Time                 Gflops
--------------------------------------------------------------------------------
WR11C2R4       29236   192     2     7              95.22             1.7497e+02
```

### 安装时碰到的问题

1. 有些lib的路径并不完全一致

### 解决过程

1. 通过/usr/lib 搜索找到对应路径并修改。
