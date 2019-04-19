#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

bool shutdown = false;
const int fps = 10;
int recordingTime = 10;			// in seconds
int replayCount = 1;			// # of replays during playback

string bgName = "Background Window";
string frameDisplayName = "Display";
string frameName_0 = "Front";
string frameName_1 = "Side";

string filename_0 = "output_front.avi";
string filename_1 = "output_side.avi";

int main();

void streamcapture(Mat, Mat, Mat);	// streams video and saves into an .avi
void playback(Mat, Mat);		// plays back saved .avi
void clearDisplay(Mat);			// clears the display
int main()
{
	// create background
	Mat bg(512, 512, CV_8UC3, Scalar(0));
	namedWindow(bgName, WINDOW_NORMAL);
	setWindowProperty(bgName, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); 
	putText(bg, "Newton JUMP!", Point(10, 50), FONT_HERSHEY_COMPLEX, 2, Scalar(255, 255, 255), 2, LINE_AA, false);		// Draw Text Title
	imshow(bgName, bg);

	// create windows and position them
	Mat frame_display(256, 512, CV_8UC3, Scalar(0));
	namedWindow(frameDisplayName, WINDOW_NORMAL);
	resizeWindow(frameDisplayName, 300, 100);
	moveWindow(frameDisplayName, 515, 615);
	imshow(frameDisplayName, frame_display);

	Mat frame_0;
	namedWindow(frameName_0);
	moveWindow(frameName_0, 0, 100);

	Mat frame_1;
	namedWindow(frameName_1);
	moveWindow(frameName_1, 700, 100);

	do
	{
		clearDisplay(frame_display);
		streamcapture(frame_0, frame_1, frame_display);
		playback(frame_0, frame_1);
	} while (shutdown == false);

	return 0;
}

// STREAM CAPTURE
void streamcapture(Mat frame_0, Mat frame_1, Mat frame_display)
{
	bool recording = false;
	bool streaming = true;
	bool countdown = false;
	int frameCount = 0;
	int delayCount = 0;
	int countdownTime = 3;
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

		// display countdown timer
		if (countdown == true)
		{
			if (delayCount > 0 && delayCount < fps * 2)					// Get Ready
			{
				putText(frame_display, "Get Ready!", Point(70, 130), FONT_HERSHEY_COMPLEX, 2, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 2 && delayCount < fps * 4)		// 3
			{
				countdownTime = 3;
				frame_display = Scalar(0);
				putText(frame_display, to_string(countdownTime), Point(220, 160), FONT_HERSHEY_COMPLEX, 5, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 4 && delayCount < fps * 6)		// 2
			{
				countdownTime = 2;
				frame_display = Scalar(0);
				putText(frame_display, to_string(countdownTime), Point(220, 160), FONT_HERSHEY_COMPLEX, 5, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 6 && delayCount < fps * 8)		// 1
			{
				countdownTime = 1;
				frame_display = Scalar(0);
				putText(frame_display, to_string(countdownTime), Point(220, 160), FONT_HERSHEY_COMPLEX, 5, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 8 && delayCount < fps * 10)		// JUMP!
			{
				// start recording, draw start text
				recording = true;
				countdownTime = 0;
				frame_display = Scalar(0);
				putText(frame_display, "JUMP!", Point(25, 170), FONT_HERSHEY_COMPLEX, 5, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount >= fps * 12)
			{
				frame_display = Scalar(0);
				putText(frame_display, "600N", Point(20, 170), FONT_HERSHEY_COMPLEX, 5, Scalar(0, 0, 255), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			
				// cleanup, reset flags
				delayCount = -1;
				countdownTime = 3;
				countdown = false;
			}
	
			delayCount++;
		}
		
		// draw recording indicator
		if (recording == true)
		{
			circle(frame_0, Point(20, 20), 3, Scalar(0, 0, 255), 5, LINE_AA, 0);
			circle(frame_1, Point(20, 20), 3, Scalar(0, 0, 255), 5, LINE_AA, 0);
		}

		// display frame
		imshow(frameName_0, frame_0);
		imshow(frameName_1, frame_1);

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
		int c = waitKey(40);											// wait 40 milliseconds
		if (27 == char(c)) { shutdown = true; break; }					// exit loop if user pressed "esc" key
		if (recording == false && (114 == char(c) || 82 == char(c)))	// wait for keypress to record
		{
			countdown = true;
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
	if (shutdown == true) { return; }
	
	int playbackLoop = 0;
	int playbackCount = 0;
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

			playbackLoop++;
			if (playbackLoop > replayCount)
			{
				playbackLoop = 0;
				playbackCount = -1;
				cout << " RESTARTING MAIN PROGRAM" << endl;
			}
			cout << " PLAYBACK LOOPED" << endl;
		}

		// push image to frame
		file_0 >> frame_0;
		file_1 >> frame_1;

		// draw playback indicator
		circle(frame_0, Point(20, 20), 4, Scalar(0, 255, 0), 5, LINE_AA, 0);
		circle(frame_1, Point(20, 20), 4, Scalar(0, 255, 0), 5, LINE_AA, 0);

		// display frame
		imshow(frameName_0, frame_0);
		imshow(frameName_1, frame_1);

		// slow motion, after second playback
		if (playbackLoop >= 1) { waitKey(80); }

		// wait for keypress to exit
		int c = waitKey(40);								// wait 40 milliseconds
		if (27 == char(c)) { shutdown = true; break; }		// exit loop if user pressed "esc" key
		if (114 == char(c) || 82 == char(c))				// wait for keypress to record
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

void clearDisplay(Mat frame_display)
{
	frame_display = Scalar(0);
	imshow(frameDisplayName, frame_display);
}