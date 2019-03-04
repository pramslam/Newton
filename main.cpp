#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

bool recording = false;
const int fps = 10;
int frameCount = 0;
int recordingTime = 10;			// in seconds

void display(Mat, Mat);
void write(Mat, Mat, VideoCapture, VideoCapture);
void release(VideoWriter, VideoWriter, VideoCapture, VideoCapture);

int main()
{
	// initialize and allocate memory to load the video stream from camera
	VideoCapture cam_0(0);
	VideoCapture cam_1(1);

	// check that the camera is open
	if (!cam_0.isOpened()) { return -1; }
	if (!cam_1.isOpened()) { return -1; }

	// store resolution of the video
	double frameWidth_0 = cam_0.get(CAP_PROP_FRAME_WIDTH);
	double frameHeight_0 = cam_0.get(CAP_PROP_FRAME_HEIGHT);
	double frameWidth_1 = cam_1.get(CAP_PROP_FRAME_WIDTH);
	double frameHeight_1 = cam_1.get(CAP_PROP_FRAME_HEIGHT);

	// create video writer
	VideoWriter output_0("output_0.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_0, frameHeight_0));
	VideoWriter output_1("output_1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_1, frameHeight_1));

	while (true)
	{
		// wait for keypress to exit
		int c = waitKey(40);			// wait 40 milliseconds
		if (27 == char(c)) { break; }	// exit loop if user pressed "esc" key
		if (recording == false && (114 == char(c) || 82 == char(c)))	// wait for keypress to record
		{
			recording = true;
			cout << " START RECORDING";
		}

		// grab and retrieve each frame of the video sequentially
		Mat frame_0;
		cam_0 >> frame_0;
		Mat frame_1;
		cam_1 >> frame_1;

		// display frame
		display(frame_0, frame_1);

		if (recording == true)
		{
			// write video frame
			output_0.write(frame_0);
			output_1.write(frame_1);

			// out of time
			if (frameCount >= (fps * recordingTime)) { break; }
			frameCount++;
		}
	}

	// cleanup
	release(output_0, output_1, cam_0, cam_1);

	return 1;
}

// display the frame
void display(Mat frame_0, Mat frame_1)
{
	imshow("Cam 0", frame_0);
	imshow("Cam 1", frame_1);
}

// release video capture and writer
void release(VideoWriter output_0, VideoWriter output_1, VideoCapture cam_0, VideoCapture cam_1)
{
	output_0.release();
	cam_0.release();
	output_1.release();
	cam_1.release();
	cout << " OUTPUT & CAMS RELEASED";
}