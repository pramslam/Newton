#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

bool shutdown = false;
const int fps = 10;
int recordingTime = 10;			// in seconds

string filename_0 = "output_0.avi";
string filename_1 = "output_1.avi";

void streamcapture(Mat, Mat);	// streams video and saves into an .avi
void playback(Mat, Mat);		// plays back saved .avi

int main()
{
	// create frame
	Mat frame_0;
	Mat frame_1;
	
	do
	{
		streamcapture(frame_0, frame_1);
		playback(frame_0, frame_1);
	} while (shutdown == false);
}

// STREAM CAPTURE
void streamcapture(Mat frame_0, Mat frame_1)
{
	bool recording = false;
	bool streaming = true;
	int frameCount = 0;
	cout << " START STREAMING" << endl;

	// initialize and allocate memory to load the video stream from camera
	VideoCapture cam_0(0);
	VideoCapture cam_1(1);

	// check that the camera is open
	if (!cam_0.isOpened()) { cout << " ERROR - CAM 1 NOT OPENED" << endl; }
	if (!cam_1.isOpened()) { cout << " ERROR - CAM 2 NOT OPENED" << endl; }

	// store resolution of the video
	double frameWidth_0 = cam_0.get(CAP_PROP_FRAME_WIDTH);
	double frameHeight_0 = cam_0.get(CAP_PROP_FRAME_HEIGHT);
	double frameWidth_1 = cam_1.get(CAP_PROP_FRAME_WIDTH);
	double frameHeight_1 = cam_1.get(CAP_PROP_FRAME_HEIGHT);

	// create video writer
	VideoWriter output_0(filename_0, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_0, frameHeight_0));
	VideoWriter output_1(filename_1, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_1, frameHeight_1));

	do
	{
		// push image to frame
		cam_0 >> frame_0;
		cam_1 >> frame_1;

		// display frame
		imshow("Cam 0", frame_0);
		imshow("Cam 1", frame_1);

		// RECORD
		if (recording == true)
		{
			// write video frame
			output_0.write(frame_0);
			output_1.write(frame_1);

			// out of time
			if (frameCount >= (fps * recordingTime))
			{
				recording = false;
				streaming = false;
				cout << " STOP RECORDING" << endl;
			}
			frameCount++;
		}

		// wait for keypress to exit
		int c = waitKey(40);							// wait 40 milliseconds
		if (27 == char(c)) { break; }					// exit loop if user pressed "esc" key
		if (recording == false && (114 == char(c) || 82 == char(c)))	// wait for keypress to record
		{
			recording = true;
			cout << " START RECORDING" << endl;
		}
	} while (streaming == true);

	// clean up
	output_0.release();
	output_1.release();
	cout << " OUTPUT RELEASED" << endl;
	cam_0.release();
	cam_1.release();
	cout << " CAM RELEASED" << endl;
}

// PLAYBACK
void playback(Mat frame_0, Mat frame_1)
{
	double playbackCount = 0;
	cout << " PLAYBACK START" << endl;

	// initialize and allocate memory to load the video stream from camera
	VideoCapture file_0(filename_0);
	VideoCapture file_1(filename_1);

	// check that the camera is open
	if (!file_0.isOpened()) { cout << " ERROR - FILE 1 NOT OPENED" << endl; }
	if (!file_1.isOpened()) { cout << " ERROR - FILE 2 NOT OPENED" << endl; }

	while (playbackCount >= 0)
	{
		playbackCount++;

		// loop if at end
		if (playbackCount >= file_0.get(CAP_PROP_FRAME_COUNT))
		{
			playbackCount = 0;
			file_0.set(CAP_PROP_POS_AVI_RATIO, 0);
			file_1.set(CAP_PROP_POS_AVI_RATIO, 0);
			cout << " PLAYBACK LOOPED" << endl;
		}

		// push image to frame
		file_0 >> frame_0;
		file_1 >> frame_1;

		// display frame
		imshow("Cam 0", frame_0);
		imshow("Cam 1", frame_1);

		// wait for keypress to exit
		int c = waitKey(40);					// wait 40 milliseconds
		if (27 == char(c)) { break; }			// exit loop if user pressed "esc" key
		if (114 == char(c) || 82 == char(c))	// wait for keypress to record
		{
			playbackCount = -1;
			cout << " RESTARTING..." << endl;
		}
	}

	// cleanup
	file_0.release();
	file_1.release();
	cout << " FILES RELEASED" << endl;
}