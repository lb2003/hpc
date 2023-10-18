课程名：高性能计算应用实践

姓名：梁彬

学号：220210611

学期：2023秋

----

## lab9 SLIC算法的OpenMP优化

----

### 实验环境：

系统: Ubuntu22.04

CPU: 12th Gen Intel(R) Core(TM) i7-12700H 14核 20线程

--- 

### 算法大意

SLIC超像素算法是利用图像像素的RGB属性转化为高精度的LAB后，对图像进行分块。称分块的中心为聚点，那么通过梯度计算出每个块迭代多次后的聚点后，以聚点像素代表块的像素，对图像进行模糊化的算法。

### 核心代码具体实现

编译指令

> g++ -I /usr/local/include/opencv4 -L /usr/lib/ -g -o openmp  openmp.cpp -fopenmp -l opencv_highgui -l opencv_core -l opencv_imgcodecs -l opencv_imgproc

利用OpenCV库函数实现RGB to LAB的转化

```cpp
cvtColor(image, imageLAB, cv::COLOR_BGR2Lab);
```

利用SLIC算法，计算聚点

```cpp
int clustering(const Mat &imageLAB, Mat &DisMask, Mat &labelMask, 
    vector<center> &centers, int len, int m) {
    if (imageLAB.empty()) {
        cout << "clustering :the input image is empty!\n";
        return -1;
    } 

    double *disPtr = NULL;
    double *labelPtr = NULL;
    const uchar *imgPtr = NULL;

    double dis = 0, disc = 0, diss = 0;
    int cx, cy, cL, cA, cB, clabel;
    int x, y, L, A, B;

    for (int ck = 0; ck < centers.size(); ck++) {
        cx = centers[ck].x;
        cy = centers[ck].y;
        cL = centers[ck].L;
        cA = centers[ck].A;
        cB = centers[ck].B;
        clabel = centers[ck].label;
        for (int i = cy - len; i < cy + len; i++) {
            if (i < 0 || i >= imageLAB.rows) continue;

            imgPtr = imageLAB.ptr<uchar>(i);
            disPtr = DisMask.ptr<double>(i);
            labelPtr = labelMask.ptr<double> (i);

            for (int j = cx - len; j < cx + len; j++) {
                if (j < 0 || j >= imageLAB.cols) continue;
                L = *(imgPtr + j * 3);
                A = *(imgPtr + j * 3 + 1);
                B = *(imgPtr + j * 3 + 2);
                // 三个关键参数
                disc = sqrt(pow(L - cL, 2) + pow(A - cA, 2) + pow(B - cB, 2));
                diss = sqrt(pow(j - cx, 2) + pow(i - cy, 2));
                dis = sqrt(pow(disc, 2) + m * pow(diss, 2));

                if (dis < *(disPtr + j)) {
                    *(disPtr + j) = dis;
                    *(labelPtr + j) = clabel;
                }
            } 
        }
    }
    return 0;
}
```

更新中心

```cpp
int UpdateCenter(Mat &imageLAB, Mat &labelMask, vector<center> &centers, int len) {
    double *labelPtr = NULL;
    const uchar *imgPtr = NULL;
    int cx, cy;

    for (int ck = 0; ck < centers.size(); ck++) {
        double sumx = 0, sumy = 0, sumL = 0, sumA = 0, sumB = 0, sumNum = 0;
        cx = centers[ck].x;
        cy = centers[ck].y;
        for (int i = cy - len; i < cy + len; i++) {
            if (i < 0 || i >= imageLAB.rows) continue;
            imgPtr = imageLAB.ptr<uchar>(i);
            labelPtr = labelMask.ptr<double>(i);
            for (int j = cx - len; j < cx + len; j++) {
                if (j < 0 || j >= imageLAB.cols) continue;

                if (*(labelPtr + j) == centers[ck].label) {
                    sumL += *(imgPtr + j * 3);
                    sumA += *(imgPtr + j * 3 + 1);
                    sumB += *(imgPtr + j * 3 + 2);
                    sumx += j;
                    sumy += i;
                    sumNum += 1;
                }//end if
            }
        }

        if (sumNum == 0) sumNum = 1e-8;
        centers[ck].x = sumx / sumNum;
        centers[ck].y = sumy / sumNum;
        centers[ck].L = sumL / sumNum;
        centers[ck].A = sumA / sumNum;
        centers[ck].B = sumB / sumNum;
    } return 0;
}

int showResult1(Mat &image, Mat &labelMask, vector<center> &centers, int len) {
    Mat dst = image.clone();
    cvtColor(dst, dst, COLOR_BGR2Lab);
    double *labelPtr = NULL;
    uchar *imgPtr = NULL;//image type: 8UC3

    int cx, cy;
    double sumx = 0, sumy = 0, sumL = 0, sumA = 0, sumB = 0, sumNum = 0.00000001;
    for (int ck = 0; ck < centers.size(); ++ck) {
        cx = centers[ck].x;
        cy = centers[ck].y;

        for (int i = cy - len; i < cy + len; i++) {
            if (i < 0 | i >= image.rows) continue;
            //pointer point to the ith row
            imgPtr = dst.ptr<uchar>(i);
            labelPtr = labelMask.ptr<double>(i);
            for (int j = cx - len; j < cx + len; j++) {
                if (j < 0 | j >= image.cols) continue;

                if (*(labelPtr + j) == centers[ck].label) {
                    *(imgPtr + j * 3) = centers[ck].L;
                    *(imgPtr + j * 3 + 1) = centers[ck].A;
                    *(imgPtr + j * 3 + 2) = centers[ck].B;
                }//end if
            }
        }
    }//end for

    cvtColor(dst, dst, cv::COLOR_Lab2BGR);
    namedWindow("showSLIC", 0);
    imshow("showSLIC", dst);
    waitKey(1);

    return 0;
}
```

10次迭代

```cpp
//update cluster 10 times 
    for (int time = 0; time < 10; time++) {
        //clustering
        disMask = Mat(height, width, CV_64FC1, Scalar(MAXDIS));
        clustering(imageLAB, disMask, labelMask, centers, len, m);
        //update
        UpdateCenter(imageLAB, labelMask, centers, len);
        //fituneCenter(imageLAB, sobelGradient, centers);
    }
```

### 代码分析以及OpenMP优化应用

不难分析得到算法瓶颈在十次迭代。

那么考虑将优化应用在此处。

Clustering函数优化

```cpp
for (int ck = 0; ck < centers.size(); ck++) {
        cx = centers[ck].x;
        cy = centers[ck].y;
        cL = centers[ck].L;
        cA = centers[ck].A;
        cB = centers[ck].B;
        clabel = centers[ck].label;
           #pragma omp parallel for num_threads(14)
        for (int i = cy - len; i < cy + len; i++) if (i >= 0 && i < imageLAB.rows) {
        double *disPtr = NULL;
            double *labelPtr = NULL;
            const uchar *imgPtr = NULL;
            int x, y, L, A, B;
            imgPtr = imageLAB.ptr<uchar>(i);
            disPtr = DisMask.ptr<double>(i);
            labelPtr = labelMask.ptr<double> (i);
            double dis = 0, disc = 0, diss = 0;
            for (int j = cx - len; j < cx + len; j++) if (j >= 0 && j < imageLAB.cols) {
                //if (j < 0 || j >= imageLAB.cols) continue;
                L = *(imgPtr + j * 3);
        A = *(imgPtr + j * 3 + 1);
        B = *(imgPtr + j * 3 + 2);

        disc = sqrt(pow(L - cL, 2) + pow(A - cA, 2) + pow(B - cB, 2));
        diss = sqrt(pow(j - cx, 2) + pow(i - cy, 2));
        dis = sqrt(pow(disc, 2) + m * pow(diss, 2));

                if (dis < *(disPtr + j)) {
                    *(disPtr + j) = dis;
                    *(labelPtr + j) = clabel;
                }
            } 
        }
    }
```

updateCenter函数

```cpp
 #pragma omp parallel for num_threads(14)
    for (int ck = 0; ck < centers.size(); ck++) {
        double sumx = 0, sumy = 0, sumL = 0, sumA = 0, sumB = 0, sumNum = 0;
        int cx, cy;
        cx = centers[ck].x;
        cy = centers[ck].y;

        for (int i = cy - len; i < cy + len; i++) {
            if (i < 0 || i >= imageLAB.rows) continue;
            double *labelPtr = NULL;
            const uchar *imgPtr = NULL;
            imgPtr = imageLAB.ptr<uchar>(i);
            labelPtr = labelMask.ptr<double>(i);
            //#pragma omp parallel for
            for (int j = cx - len; j < cx + len; j++) {
                if (j < 0 || j >= imageLAB.cols) continue;

                if (*(labelPtr + j) == centers[ck].label) {
                    sumL += *(imgPtr + j * 3);
                    sumA += *(imgPtr + j * 3 + 1);
                    sumB += *(imgPtr + j * 3 + 2);
                    sumx += j;
                    sumy += i;
                    sumNum += 1;
                }//end if
            }
        }

        if (sumNum == 0) sumNum = 1e-8;
        centers[ck].x = sumx / sumNum;
        centers[ck].y = sumy / sumNum;
        centers[ck].L = sumL / sumNum;
        centers[ck].A = sumA / sumNum;
        centers[ck].B = sumB / sumNum;
    }
```

---

### 运行结果及比较

本结果将分块可视化即最终效果都呈现了出来。

如图：

![](https://github.com/lb2003/hpc/blob/main/photo/pic_1.png)

![](https://github.com/lb2003/hpc/blob/main/photo/pic_2.png)

![](https://github.com/lb2003/hpc/blob/main/photo/pic_3.png)

优化比较如图：

![](https://github.com/lb2003/hpc/blob/main/photo/pic_1.png)

大致提升约6倍。

### 碰到的问题

1. OpenCV库安装中碰到相关文件无法下载

2. OpenCV库安装路径与教程不一致，编译指令不一致

3. OpenMP嵌入代码困难，出现输出结果乱码，段错误的情况

### 解决过程

1. 通过手动下载并修改相关文件索引路径

2. 自行修改路径，找到头文件目标路径后修改索引

3. 查阅相关资料后得知，OpenMP元素对于Public属性的变量会在多进程中发生冲突，尝试将变量变成Private后得到改善。

### 不足

由于时间紧张，并未优化缓存以及数组方式的多线程。
