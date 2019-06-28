#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;

// we're NOT "using namespace std;" here, to avoid collisions between the beta variable and std::beta in c++17
using std::cin;
using std::cout;
using std::endl;

int main(void)
{
	double alpha = 0.0; double beta; double input;
	

	Mat src1, src2, dst;

	src1 = imread("add1.jpg");
	src2 = imread("add2.jpg");

	if (src1.empty()) { cout << "Error loading src1" << endl; return -1; }
	if (src2.empty()) { cout << "Error loading src2" << endl; return -1; }

	VideoWriter writer("test.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25.0, Size(src1.cols, src1.rows));

	while (alpha < 1)
	{
		beta = (1.0 - alpha);
		addWeighted(src1, alpha, src2, beta, 0.0, dst);

		imshow("Linear Blend", dst);
		waitKey(20);

		alpha += 0.02;
		writer << dst;
	}

	return 0;
}