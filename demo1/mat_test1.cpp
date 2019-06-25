#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat A, C;                          // creates just the header parts
	A = imread("feng.png", IMREAD_COLOR); // here we'll know the method used (allocate matrix)

	Mat B(A);                                 // Use the copy constructor

	C = A;                                    // Assignment operator

	Mat F = A.clone();
	Mat G;
	A.copyTo(G);

	cout << "A ptr: " << hex << (void *)A.data << endl;
	cout << "B ptr: " << hex << (void *)B.data << endl;
	cout << "C ptr: " << hex << (void *)C.data << endl;
	cout << "F ptr: " << hex << (void *)F.data << endl;
	cout << "G ptr: " << hex << (void *)G.data << endl;

	cout << endl;

	waitKey();

	return 0;
}

