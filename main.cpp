// Created by David Lo 2018-2019
// for
// Eugene Science Center "Jump" Exhibit
// Project dates 12/18 - 6/2/19

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

bool shutdown = false;
bool bRecordingIndicator = true;	// show recording indicator
const int fps = 10;
int recordingTime = 10;				// in seconds
int replayCount = 1;				// # of replays during playback

double frameWidth_0;
double frameHeight_0;
double frameWidth_1;
double frameHeight_1;

double frameDisplayWidth;
double frameDisplayHeight;

string bgName = "Background Window";
string frameDisplayName = " ";
string frameName_0 = "1";
string frameName_1 = "2";

string filename_0 = "output_1.avi";
string filename_1 = "output_2.avi";

string text_intro = "frame_display_intro.png";
string text_ready = "frame_display_ready.png";
string text_3 = "frame_display_3.png";
string text_2 = "frame_display_2.png";
string text_1 = "frame_display_1.png";
string text_jump = "frame_display_jump.png";
string text_replay = "frame_display_replay.png";
string text_slowmotionreplay = "frame_display_slowmotionreplay.png";

int main();
void StreamCapture(Mat, Mat, Mat);		// streams video and saves into an .avi
void Playback(Mat, Mat, Mat);			// plays back saved .avi
void ClearDisplay(Mat);					// clears the display
void ClearOutput();						// clears the output

int main()
{
	// create background
	Mat bg(512, 512, CV_8UC3, Scalar(0));
	namedWindow(bgName, WINDOW_NORMAL);
	setWindowProperty(bgName, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); 
	imshow(bgName, bg);

	// create windows and position them
	Mat frame_0;
	namedWindow(frameName_0);
	moveWindow(frameName_0, 0, 200);

	Mat frame_1;
	namedWindow(frameName_1);
	moveWindow(frameName_1, 700, 200);

	Mat frame_display(256, 1024, CV_8UC3, Scalar(0));
	namedWindow(frameDisplayName, WINDOW_NORMAL);
	moveWindow(frameDisplayName, 0, 0);
	resizeWindow(frameDisplayName, 1350, 198);			// Manually sized to block upper window tabs of frame_1 & 2
	imshow(frameDisplayName, frame_display);

	// MAIN LOOP
	do
	{
		ClearDisplay(frame_display);
		StreamCapture(frame_0, frame_1, frame_display);
		Playback(frame_0, frame_1, frame_display);
		ClearOutput();
	} while (shutdown == false);

	return 0;
}

// STREAM CAPTURE
void StreamCapture(Mat frame_0, Mat frame_1, Mat frame_display)
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
	frameWidth_0 = cam_0.get(CAP_PROP_FRAME_WIDTH);
	frameHeight_0 = cam_0.get(CAP_PROP_FRAME_HEIGHT);
	frameWidth_1 = cam_1.get(CAP_PROP_FRAME_WIDTH);
	frameHeight_1 = cam_1.get(CAP_PROP_FRAME_HEIGHT);

	// store display size
	frameDisplayWidth = frame_display.cols;
	frameDisplayHeight = frame_display.rows;

	// create video writer
	VideoWriter output_0(filename_0, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_0, frameHeight_0));
	VideoWriter output_1(filename_1, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth_1, frameHeight_1));

	// Intro text
	frame_display = imread(text_intro);
	//putText(frame_display, "PUSH BUTTON TO START", Point(frameDisplayWidth / 9.5, frameDisplayHeight / 1.6), FONT_HERSHEY_COMPLEX, 2, Scalar(255, 0, 0), 3, LINE_AA, false);
	imshow(frameDisplayName, frame_display);

	do
	{
		// push image to frame
		cam_0 >> frame_0;
		cam_1 >> frame_1;

		// display countdown timer
		if (countdown == true)
		{
			// Offsets margin from edge
			float numOffsetW = 2.2;
			float numOffsetH = 1.3;

			if (delayCount > 0 && delayCount < fps * 2)					// Get Ready
			{
				frame_display = Scalar(0);
				frame_display = imread(text_ready);
				//putText(frame_display, "READY?", Point(frameDisplayWidth / 2.6, frameDisplayHeight / 1.6), FONT_HERSHEY_COMPLEX, 2, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 2 && delayCount < fps * 4)		// 3
			{
				countdownTime = 3;
				frame_display = Scalar(0);
				frame_display = imread(text_3);
				//putText(frame_display, to_string(countdownTime), Point(frameDisplayWidth / numOffsetW, frameDisplayHeight / numOffsetH), FONT_HERSHEY_COMPLEX, 5, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 4 && delayCount < fps * 6)		// 2
			{
				countdownTime = 2;
				frame_display = Scalar(0);
				frame_display = imread(text_2);
				//putText(frame_display, to_string(countdownTime), Point(frameDisplayWidth / numOffsetW, frameDisplayHeight / numOffsetH), FONT_HERSHEY_COMPLEX, 5, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 6 && delayCount < fps * 8)		// 1
			{
				countdownTime = 1;
				frame_display = Scalar(0);
				frame_display = imread(text_1);
				//putText(frame_display, to_string(countdownTime), Point(frameDisplayWidth / numOffsetW, frameDisplayHeight / numOffsetH), FONT_HERSHEY_COMPLEX, 5, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount > fps * 8 && delayCount < fps * 10)		// JUMP!
			{
				// start recording
				recording = true;
				countdownTime = 0;
				frame_display = Scalar(0);
				frame_display = imread(text_jump);
				//putText(frame_display, "JUMP!!!", Point(frameDisplayWidth / 4, frameDisplayHeight / numOffsetH), FONT_HERSHEY_COMPLEX, 5, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			}
			else if (delayCount >= fps * 12)
			{
				// display newtons from capture here TODO////////////////////
				frame_display = Scalar(0);
				//putText(frame_display, "600N", Point(frameDisplayWidth / 3.4, frameDisplayHeight / numOffsetH), FONT_HERSHEY_COMPLEX, 5, Scalar(255, 0, 0), 3, LINE_AA, false);
				imshow(frameDisplayName, frame_display);
			
				// cleanup, reset flags
				delayCount = -1;
				countdownTime = 3;
				countdown = false;
			}
	
			delayCount++;
		}
		
		// draw recording indicator
		if (recording == true && bRecordingIndicator == true)
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
void Playback(Mat frame_0, Mat frame_1, Mat frame_display)
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
		// Clear frame display
		frame_display = Scalar(0);

		// Frame display replay text
		if (playbackLoop == 0)
		{
			// Replay text display
			frame_display = imread(text_replay);
			//putText(frame_display, "REPLAY", Point(frameDisplayWidth / 2.6, frameDisplayHeight / 1.6), FONT_HERSHEY_COMPLEX, 2, Scalar(255, 0, 0), 3, LINE_AA, false);
			imshow(frameDisplayName, frame_display);
		}
		else if (playbackLoop >= 1)
		{
			// Slow-Mo text display
			frame_display = imread(text_slowmotionreplay);
			//putText(frame_display, "SLOW MOTION REPLAY", Point(frameDisplayWidth / 8, frameDisplayHeight / 1.6), FONT_HERSHEY_COMPLEX, 2, Scalar(255, 0, 0), 3, LINE_AA, false);
			imshow(frameDisplayName, frame_display);
		}

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

// clears the display frame
void ClearDisplay(Mat frame_display)
{
	frame_display = Scalar(0);
	imshow(frameDisplayName, frame_display);
}

// clears output from memory
void ClearOutput()
{
	if (remove("output_1.avi") != 0)
		cout << "Error clearing file" << endl;
	else
		cout << "File 1 successfully cleared" << endl;

	if (remove("output_2.avi") != 0)
		cout << "Error clearing file" << endl;
	else
		cout << "File 2 successfully cleared" << endl;
}
