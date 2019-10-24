
#include <iostream>
#include "opencv2/opencv.hpp"
#include "blooming.h"


using namespace cv;
using namespace std;


// Try to set the camera resolution. Note that this only works for some cameras on
// some computers and only for some drivers, so don't rely on it to work!
const int DESIRED_CAMERA_WIDTH = 640;
const int DESIRED_CAMERA_HEIGHT = 480;
const char* windowName = "Blooming";   // Name shown in the GUI window.
bool m_bloomingMode = false; // Set to true if you want to see blooming.


int main(int argc, char* argv[])
{
	cout << "Blooming" << endl;
	cout << endl;

	cout << "Keyboard commands (press in the GUI window):" << endl;
	cout << "    Esc:  Quit the program." << endl;
	cout << "    b:    Normal / Blooming mode." << endl;
	cout << endl;

	// Allow the user to specify a camera number, since not all computers will be the same camera number.
	int cameraNumber = 0;   // Change this if you want to use a different camera device.
	if (argc > 1) {
		cameraNumber = atoi(argv[1]);
	}

	// Get access to the camera.
	VideoCapture camera;
	camera.open(cameraNumber);
	if (!camera.isOpened()) {
		cerr << "ERROR: Could not access the camera or video!" <<
			endl;
		exit(1);
	}

	// Try to set the camera resolution. Note that this only works for some cameras on
	// some computers and only for some drivers, so don't rely on it to work!
	camera.set(CAP_PROP_FRAME_WIDTH, DESIRED_CAMERA_WIDTH);
	camera.set(CAP_PROP_FRAME_HEIGHT, DESIRED_CAMERA_HEIGHT);

	// Create a GUI window for display on the screen.
	namedWindow(windowName); // Resizable window, might not work on Windows.

	// Run forever, until the user hits Escape to "break" out of this loop.
	while (true) {

		// Grab the next camera frame. Note that you can't modify camera frames.
		Mat cameraFrame;
		camera >> cameraFrame;
		if (cameraFrame.empty()) {
			cerr << "ERROR: Couldn't grab the next camera frame." << endl;
			exit(1);
		}

		Mat displayedFrame = Mat(cameraFrame.size(), CV_8UC3);

		// Run the blooming filter.
		bloomifyImage(cameraFrame, displayedFrame, m_bloomingMode);

		imshow(windowName, displayedFrame);

		// IMPORTANT: Wait for atleast 20 milliseconds, so that the image can be displayed on the screen!
		// Also checks if a key was pressed in the GUI window. Note that it should be a "char" to support Linux.
		char keypress = waitKey(20);  // This is needed if you want to see anything!
		if (keypress == 27) {   // Escape Key
			// Quit the program!
			break;
		}
		// Process any other keypresses.
		if (keypress > 0) {
			switch (keypress) {
			case 'b':
				m_bloomingMode = !m_bloomingMode;
				cout << "Normal / Blooming mode: " << m_bloomingMode << endl;
				break;
			}
		}

	}//end while

	return EXIT_SUCCESS;
}
