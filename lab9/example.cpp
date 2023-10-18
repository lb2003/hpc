#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <sys/time.h>
#include "f77blas.h"
#include <time.h>
#include <omp.h>
#include <pthread.h> 

using namespace std;
using namespace cv;

struct center {
    int x; 
    int y; 
    int L; 
    int A;
    int B;
    int label;
};

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
} int showResult2(const Mat &image, Mat &labelMask, vector<center> &centers, int len)
{
	cv::Mat dst = image.clone();
	//cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);
	double *labelPtr = NULL;//labelMask type: 32FC1
	double *labelPtr_nextRow = NULL;//labelMask type: 32FC1
	uchar *imgPtr = NULL;//image type: 8UC3

	for (int i = 0; i < labelMask.rows - 1; i++)
	{
		labelPtr = labelMask.ptr<double>(i);
		imgPtr = dst.ptr<uchar>(i);
		for (int j = 0; j < labelMask.cols - 1; j++) {
			//if left pixel's label is different from the right's 
			if (*(labelPtr + j) != *(labelPtr + j + 1)) {
				*(imgPtr + 3 * j) = 0;
				*(imgPtr + 3 * j + 1) = 0;
				*(imgPtr + 3 * j + 2) = 0;
			}

			//if the upper pixel's label is different from the bottom's 
			labelPtr_nextRow = labelMask.ptr<double>(i + 1);
			if (*(labelPtr_nextRow + j) != *(labelPtr + j)) {
				*(imgPtr + 3 * j) = 0;
				*(imgPtr + 3 * j + 1) = 0;
				*(imgPtr + 3 * j + 2) = 0;
			}
		}
	}

	//show center
	for (int ck = 0; ck < centers.size(); ck++) {
		imgPtr = dst.ptr<uchar>(centers[ck].y);
		*(imgPtr + centers[ck].x * 3) = 100;
		*(imgPtr + centers[ck].x * 3 + 1) = 100;
		*(imgPtr + centers[ck].x * 3 + 1) = 10;
	}

	namedWindow("showSLIC2", 0);
	imshow("showSLIC2", dst);
	waitKey(1);
	return 0;
} 

int init(Mat &imageLAB, vector<center> &centers, int len) {
    if (imageLAB.empty()) {
		std::cout << "In itilizeCenters:     image is empty!\n";
		return -1;
	}

	uchar *ptr = NULL;
	center cent;
	int num = 0;
	for (int i = 0; i < imageLAB.rows; i += len) {
		cent.y = i + len / 2;
		if (cent.y >= imageLAB.rows) continue;
		ptr = imageLAB.ptr<uchar>(cent.y);
		for (int j = 0; j < imageLAB.cols; j += len) {
			cent.x = j + len / 2;
			if ((cent.x >= imageLAB.cols)) continue;
			cent.L = *(ptr + cent.x * 3);
			cent.A = *(ptr + cent.x * 3 + 1);
			cent.B = *(ptr + cent.x * 3 + 2);
			cent.label = ++num;
			centers.push_back(cent);
		}
	}
	return 0;
}

int fituneCenter(Mat &imageLAB, Mat &sobelGradient, vector<center> &centers) {
	if (sobelGradient.empty()) return -1;

	center cent;
	double *sobPtr = sobelGradient.ptr<double>(0);
	uchar *imgPtr = imageLAB.ptr<uchar>(0);
	int w = sobelGradient.cols;
	for (int ck = 0; ck < centers.size(); ck++) {
		cent = centers[ck];
		if (cent.x - 1 < 0 || cent.x + 1 >= sobelGradient.cols || cent.y - 1 < 0 || cent.y + 1 >= sobelGradient.rows) {
			continue;
		}//end if
		double minGradient = 9999999;
		int tempx = 0, tempy = 0;
		for (int m = -1; m < 2; m++) {
			sobPtr = sobelGradient.ptr<double>(cent.y + m);
			for (int n = -1; n < 2; n++) {
				double gradient = pow(*(sobPtr + (cent.x + n) * 3), 2)
					+ pow(*(sobPtr + (cent.x + n) * 3 + 1), 2)
					+ pow(*(sobPtr + (cent.x + n) * 3 + 2), 2);
				if (gradient < minGradient) {
					minGradient = gradient;
					tempy = m;//row
					tempx = n;//column
				}//end if
			}
		}
		cent.x += tempx;
		cent.y += tempy;
		imgPtr = imageLAB.ptr<uchar>(cent.y);
		centers[ck].x = cent.x;
		centers[ck].y = cent.y;
		centers[ck].L = *(imgPtr + cent.x * 3);
		centers[ck].A = *(imgPtr + cent.x * 3 + 1);
		centers[ck].B = *(imgPtr + cent.x * 3 + 2);

	}//end for
	return 0;
}

int SLIC(Mat &image, Mat &resultLabel,vector<center> &centers, int len, int m) {
    if (image.empty()) {
		std::cout << "in SLIC the input image is empty!\n";
		return -1;

	}

	int MAXDIS = 999999;
	int height, width;
	height = image.rows;
	width = image.cols;

	//convert color
	Mat imageLAB;
	cvtColor(image, imageLAB, cv::COLOR_BGR2Lab);

	//get sobel gradient map
	Mat sobelImagex, sobelImagey, sobelGradient;
	Sobel(imageLAB, sobelImagex, CV_64F, 0, 1, 3);
	Sobel(imageLAB, sobelImagey, CV_64F, 1, 0, 3);
	addWeighted(sobelImagex, 0.5, sobelImagey, 0.5, 0, sobelGradient);//sobel output image type is CV_64F

	//initiate
	//std::vector<center> centers;
	//disMask save the distance of the pixels to center;
	Mat disMask;
	//labelMask save the label of the pixels
	Mat labelMask = Mat::zeros(Size(width, height), CV_64FC1);

	//initialize centers,  get centers
	init(imageLAB, centers, len);
	//if the center locates in the edges, fitune it's location
	fituneCenter(imageLAB, sobelGradient, centers);

	//update cluster 10 times 
	for (int time = 0; time < 10; time++) {
		//clustering
		disMask = Mat(height, width, CV_64FC1, Scalar(MAXDIS));
		clustering(imageLAB, disMask, labelMask, centers, len, m);
		//update
		UpdateCenter(imageLAB, labelMask, centers, len);
		//fituneCenter(imageLAB, sobelGradient, centers);
	}

	resultLabel = labelMask;

	return 0;
}

int SLIC_Demo() {
	struct timeval start, finish;
    	double duration;
    	gettimeofday(&start, NULL);
	string imagePath = "./pic.jpg";
	Mat image = imread(imagePath);
	Mat labelMask;//save every pixel's label
	Mat dst;//save the shortest distance to the nearest centers
	vector<center> centers;//clustering centers

	int len = 25;//the scale of the superpixel ,len*len
	int m = 10;//a parameter witch adjust the weights of spacial distance and the color space distance
	SLIC(image, labelMask, centers, len, m);

	namedWindow("image", 1);
	imshow("image", image);
	showResult1(image, labelMask, centers, len);
	showResult2(image, labelMask, centers, len);
	gettimeofday(&finish, NULL);
	duration = ((double)(finish.tv_sec-start.tv_sec)*1000000 + (double)(finish.tv_usec-start.tv_usec)) / 1000000;
	int rows = image.rows, cols = image.cols;
        double gflops = 320.0 * rows * cols;
        gflops = gflops / duration * 1.0e-6 / 1000;
        printf("rows:%d\ncols:%d:\nduration: %lf\nGflops: %lf\n", rows, cols, duration, gflops);
	waitKey(0);
	return 0;
}

int main() {
    SLIC_Demo();
    return 0;
}
