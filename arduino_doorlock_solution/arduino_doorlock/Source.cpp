#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include "serialcomm.h"
using namespace std;
using namespace cv;
/* Function Headers */
void detectAndDisplay(Mat frame);
/* Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection";
CSerialComm serialComm; //SerialComm 객체 생성
char detectStatus = '0';
char temp = '0';
/* @function main */
int main(void)
{
	VideoCapture capture;
	Mat frame;
	char buffer;
	if (!serialComm.connect("COM159")) //COM25번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	{
		cout << "connect faliled" << endl;
		return -1;
	}
	else
		cout << "connect successed" << endl;
	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -1; };
	//-- 2. Read the video stream
	capture.open(-1);
	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return -1; }
	while (capture.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		//-- 3. Apply the classifier to the frame
		detectAndDisplay(frame);

		int c = waitKey(10);
		if ((char)c == 27) { break; } // escape
	}
	return 0;
}
/* @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	if (faces.size() > 0) {

		detectStatus = '1';
		if (temp == '0') {
			cout << "detect!!" << endl;
			serialComm.sendCommand('1');
			temp = '1';
		}
	}
	else {
		detectStatus = '0';
		if (temp == '1') {
			cout << "Unknown" << endl;
			serialComm.sendCommand('0');
			temp = '0';
		}
	}
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		//ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 255, 0), 4, 8, 0);
		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;
		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
			//circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
			rectangle(frame, Point(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y), Point(faces[i].x + eyes[j].x + eyes[j].width, faces[i].y + eyes[j].y + eyes[j].height), Scalar(255, 0, 0), 4, 8, 0);
		}

	}
	//-- Show what you got
	imshow(window_name, frame);
}