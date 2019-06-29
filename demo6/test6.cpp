#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

// we're NOT "using namespace std;" here, to avoid collisions between the beta variable and std::beta in c++17
using std::cin;
using std::cout;
using std::endl;
using namespace cv;

int alpha;
int beta;
Mat image;

void on_trackbar(int, void*)
{
	double alpha_t = alpha / 100.0;
	Mat new_image = Mat::zeros(image.size(), image.type());
	/*for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				new_image.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha_t*image.at<Vec3b>(y, x)[c] + beta);
			}
		}
	}*/
	image.convertTo(new_image, -1, alpha_t, beta);

	imshow("Original Image", image);
	imshow("bright&contrast", new_image);
}

void test_bight_contrast()
{
	alpha = 100; /*< Simple contrast control */
	beta = 0;       /*< Simple brightness control */

	namedWindow("bright&contrast");

	createTrackbar("contrast:", "bright&contrast", &alpha, 300, on_trackbar);
	createTrackbar("bright:  ", "bright&contrast", &beta, 100, on_trackbar);

	on_trackbar(alpha, 0);
	on_trackbar(beta, 0);

	while (char(waitKey(1)) != 'q') {}
}

int main()
{
	image = imread("gamma.jpg");
	if (image.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}

	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, 0.4) * 255.0);

	Mat res = image.clone();
	LUT(image, lookUpTable, res);

	imshow("source", image);
	imshow("gamma", res);

	waitKey();

	return 0;
}