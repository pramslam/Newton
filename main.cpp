#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

const int fps = 30;

int main()
{
	// initialize and allocate memory to load the video stream from camera
	VideoCapture cam_0(0);
	VideoCapture cam_1(1);

	if (!cam_0.isOpened()) { return -1; }

	if (!cam_1.isOpened()) { return -1; }

	while (true)
	{
		// grab and retrieve each frame of the video sequentially
		Mat frame_0;
		cam_0 >> frame_0;
		Mat frame_1;
		cam_1 >> frame_1;

		imshow("Cam 0", frame_0);
		imshow("Cam 1", frame_1);

		// wait for keypress to exit
		int c = waitKey(40);			// wait 40 milliseconds
		if (27 == char(c)) { break; }	// exit loop if user pressed "esc" key
	}

	return 1;
}