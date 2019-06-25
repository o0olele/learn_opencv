#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	int channels = I.channels();

	int nRows = I.rows;
	int nCols = I.cols * channels;

	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar* p;
	for (i = 0; i < nRows; ++i)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = table[p[j]];
		}
	}
	return I;
}

Mat& ScanImageAndReduceIterator(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		MatIterator_<uchar> it, end;
		for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
			*it = table[*it];
		break;
	}
	case 3:
	{
		MatIterator_<Vec3b> it, end;
		for (it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
		{
			(*it)[0] = table[(*it)[0]];
			(*it)[1] = table[(*it)[1]];
			(*it)[2] = table[(*it)[2]];
		}
	}
	}

	return I;
}

Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
				I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
		break;
	}
	case 3:
	{
		Mat_<Vec3b> _I = I;

		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
			{
				_I(i, j)[0] = table[_I(i, j)[0]];
				_I(i, j)[1] = table[_I(i, j)[1]];
				_I(i, j)[2] = table[_I(i, j)[2]];
			}
		I = _I;
		break;
	}
	}

	return I;
}

int main()
{
	int divideWith = 10; 

	uchar table[256];
	for (int i = 0; i < 256; ++i)
		table[i] = (uchar)(divideWith * (i / divideWith));

	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = table[i];

	Mat Mat_inA = imread("test.jpg");
	Mat Mat_inB = imread("test.jpg");
	Mat Mat_inC = imread("test.jpg");
	Mat I = imread("test.jpg");
	Mat J;

	double t = (double)getTickCount();

	J = ScanImageAndReduceC(Mat_inA, table);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "The efficient way£¡ " << endl;
	cout << "Times passed in seconds: " << setiosflags(ios::fixed) << t << endl;

	t = (double)getTickCount();

	J = ScanImageAndReduceIterator(Mat_inB, table);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "The iterator (safe) method£¡ " << endl;
	cout << "Times passed in seconds: " << t << endl;

	t = (double)getTickCount();

	J = ScanImageAndReduceRandomAccess(Mat_inC, table);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "On-the-fly address calculation with reference returning£¡" << endl;
	cout << "Times passed in seconds: " << t << endl;
	

	t = (double)getTickCount();

	LUT(I, lookUpTable, J);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "The Core Function£¡" << endl;
	cout << "Times passed in seconds: " << t << endl;

	waitKey();

	return 0;
}