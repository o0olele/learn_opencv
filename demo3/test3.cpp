#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void Sharpen(const Mat& myImage, Mat& Result);

int main()
{
	const char* filename = "lena.jpg";

	Mat src, dst0, dst1;

	src = imread(filename, IMREAD_COLOR);

	if (src.empty())
	{
		cerr << "Can't open image [" << filename << "]" << endl;
		return -1;
	}

	namedWindow("Input", WINDOW_AUTOSIZE);
	namedWindow("Output", WINDOW_AUTOSIZE);

	imshow("Input", src);
	double t = (double)getTickCount();

	Sharpen(src, dst0);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Hand written function time passed in seconds: " << t << endl;

	imshow("Sharpen Result", dst0);
	
	getchar();

	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
		-1, 5, -1,
		0, -1, 0);

	t = (double)getTickCount();

	filter2D(src, dst1, src.depth(), kernel);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Built-in filter2D time passed in seconds:     " << t << endl;

	imshow("Filter2D Result", dst1);

	cout << "depth" << src.depth() << endl;
	waitKey();
	return 0;
}
void Sharpen(const Mat& myImage, Mat& Result)
{
	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images

	const int nChannels = myImage.channels();
	Result.create(myImage.size(), myImage.type());

	for (int j = 1; j < myImage.rows - 1; ++j)
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);//前一行
		const uchar* current = myImage.ptr<uchar>(j);//当前行
		const uchar* next = myImage.ptr<uchar>(j + 1);//下一行

		uchar* output = Result.ptr<uchar>(j);

		for (int i = nChannels; i < nChannels*(myImage.cols - 1); ++i)
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);
		}
	}

	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
}