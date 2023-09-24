课程名：高性能计算应用实践

姓名：梁彬

学号：220210611

学期：2023秋

----

### lab3

2. 
   
   在 O0 下比较

![](https://github.com/lb2003/hpc/blob/main/lab3/O0.jpg)

    在 O1 下比较

![](https://github.com/lb2003/hpc/blob/main/lab3/O1.jpg)

    在 O2 下比较

![](https://github.com/lb2003/hpc/blob/main/lab3/O2.jpg)

    在 O3 下比较

![](https://github.com/lb2003/hpc/blob/main/lab3/O3.jpg)

3.

 cblas与naive的比较

![](https://github.com/lb2003/hpc/blob/main/lab3/cblas.png)

4.

    碰到的问题如下:

1. 对于cblas的编译应置于makefile文件何处产生疑问（已解决）

2. 对于plotAll文件输出图像的大小需要调整（已解决）


    解决过程如下：
1. 在.x文件处添加-lopenblas
2. 在plotAll.py文件中修改


