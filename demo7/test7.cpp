#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	const char* filename = "text2.png";

	Mat I = imread(filename, IMREAD_GRAYSCALE);
	if (I.empty()) {
		cout << "Error opening image" << endl;
		return -1;
	}

	Mat Mfloat = Mat_ <float>(I);
	imshow("float", Mfloat);

	Mat padded;                            //expand input image to optimal size
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols); // on the border add zero values
	/*cout << "input image rows:" << I.rows << endl;
	cout << "input image cols:" << I.cols << endl;

	cout << "optimal rows:" << m << endl;
	cout << "optimal cols:" << n << endl;*/

	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));

	imshow("padded", padded);


	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	dft(complexI, complexI);            // this way the result may fit in the source matrix

										// compute the magnitude and switch to logarithmic scale
										// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];
	Mat magC = planes[1];

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	magC += Scalar::all(1);                    // switch to logarithmic scale
	log(magC, magC);

	cout << "after log Mat[0,0]:" << magI.at<Vec2f>(0, 0) << endl;

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	magC = magC(Rect(0, 0, magC.cols & -2, magC.rows & -2));
	/*cout << "magI rows:" << magI.rows << endl;
	cout << "magI cols:" << magI.cols << endl;
	cout << "magI rows & -2:" << (magI.rows & -2) << endl;
	cout << "magI cols & -2:" << (magI.cols & -2) << endl;*/

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(magC, magC, 0, 1, NORM_MINMAX);
	normalize(magI, magI, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a
											  // viewable image form (float between values 0 and 1).
	cout << "after normal Mat[0,0]:" << magI.at<Vec2f>(0, 0) << endl;

	imshow("Input Image", I);    // Show the result
	imshow("spectrum magnitude", magI);
	imshow("spectrum complex", magC);
	cout << magI.depth() << endl;
	waitKey();

	return 0;
}