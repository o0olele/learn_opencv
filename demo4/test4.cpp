#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void iomat()
{
	Mat img_jpg, img_png;

	img_jpg = imread("test1.jpg", IMREAD_GRAYSCALE);
	img_png = imread("test2.png", IMREAD_GRAYSCALE);

	imshow("JPG", img_jpg);
	imshow("PNG", img_png);

	imwrite("test3.png", img_jpg);
}

void Accessing_pixel_intensity()
{
	Mat A(3, 3, CV_8UC3, Scalar(123, 10, 0));

	cout << "A:\n" << A << endl;
	A.at<uchar>(1, 1) = 200;
	cout << "A:\n" << A << endl;

	A.at<Vec3b>(2, 1)[0] = 200;
	A.at<Vec3b>(2, 1)[2] = 200;

	cout << "A:\n" << A << endl;
}

void primitive_operations()
{
	Mat img, grey;

	img = imread("test1.jpg");
	imshow("operations_before", img);

	Rect r(img.cols / 2, 0, img.cols / 2, img.rows);
	Mat smallImg = img(r);

	Rect rC(0, 0, img.cols / 2, img.rows);
	Mat smallImgC = img(rC);

	Mat src = smallImgC.clone();
	cvtColor(src, grey, COLOR_BGR2GRAY);

	src.copyTo(smallImg, grey);

	imshow("operations", img);
}

void visual_image()
{
	Mat img = imread("test1.jpg");
	Mat grey;
	cvtColor(img, grey, COLOR_BGR2GRAY);
	Mat sobelx;
	Sobel(grey, sobelx, CV_32F, 1, 0);
	double minVal, maxVal;
	minMaxLoc(sobelx, &minVal, &maxVal); //find minimum and maximum intensities
	Mat draw;
	sobelx.convertTo(draw, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
	namedWindow("image", WINDOW_FREERATIO);
	imshow("image", draw);
}

void other()
{
	Mat img = imread("pikachu.png");
	Vec3b intensity = img.at<Vec3b>(0, 0);
	cout << intensity << endl;

	Mat grey;
	cvtColor(img, grey, COLOR_BGR2GRAY);
	cout << hex << grey.at<uchar>(0, 0) << endl;

	imshow("png", img);
	imshow("png_gray", grey);
}

int main()
{
	Mat img, src, grey;

	img = imread("test1.jpg");
	src = imread("pikachu.png");
	imshow("operations_before", img);

	Rect r(0, 0, src.cols, src.rows);
	Mat smallImg = img(r);

	cvtColor(src, grey, COLOR_BGR2GRAY);

	src.copyTo(smallImg, grey);

	imshow("operations", img);

	waitKey();

	return 0;
}
