#include <iostream> // for standard I/O
#include <string>   // for strings
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;


int main()
{
	const string source = "1.mp4";           // the source file name
	const bool askOutputType = true;  // If false it will use the inputs codec type
	VideoCapture inputVideo(source);              // Open input
	if (!inputVideo.isOpened())
	{
		cout << "Could not open the input video: " << source << endl;
		return -1;
	}

	const string NAME = "test.mp4";   // Form the new name with container
	int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form
																	// Transform from int to char via Bitwise operators
	char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };
	Size S = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputVideo;                                        // Open the output
	if (askOutputType)
		outputVideo.open(NAME, ex = -1, inputVideo.get(CAP_PROP_FPS), S, true);
	else
		outputVideo.open(NAME, ex, inputVideo.get(CAP_PROP_FPS), S, true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << NAME << endl;
		return -1;
	}
	cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
		<< " of nr#: " << inputVideo.get(CAP_PROP_FRAME_COUNT) << endl;
	cout << "Input codec type: " << EXT << endl;
	int channel = 2; // Select the channel to save

	Mat src, res;
	vector<Mat> spl;
	for (;;) //Show the image captured in the window and repeat
	{
		inputVideo >> src;              // read
		if (src.empty()) break;         // check if at end
		Mat src_roi = src(Rect(0, 0, src.cols / 2, src.rows));
		Mat src_roi_grey = src_roi.clone();
		//cvtColor(src_roi, src_roi_grey, COLOR_BGR2GRAY);
		split(src_roi_grey, spl);                // process - extract only the correct channel
		for (int i = 0; i < 3; ++i)
			if (i != channel)
				spl[i] = Mat::zeros(src_roi_grey.size(), spl[0].type());
		merge(spl, res);
		cvtColor(src_roi, src_roi_grey, COLOR_BGR2GRAY);
		res.copyTo(src_roi,src_roi_grey);
		//outputVideo.write(res); //save or
		//pyrDown(src, src, Size(src.cols / 2, src.rows / 2));
		outputVideo << src;
	}
	cout << "Finished writing" << endl;
	return 0;
}