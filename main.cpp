#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>
#include <vector>



using namespace std;
using namespace cv;

int slider_pos = 0;
int run = 1, dontset = 0;
std::shared_ptr<cv::VideoCapture> video = make_shared<cv::VideoCapture>();

void on_trackbar(int pos, void*) {
	
	video->set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!dontset) {
		run = 1;
	}
	dontset = 0;
}


int main() {
	// List of tracker types in OpenCV
	string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	// vector <string> trackerTypes(types, std::end(types));

	//List of different available tracker to test which performs best
    //for the particular use case
	string trackerType = trackerTypes[4];
	Ptr<Tracker> tracker;
	{
		if (trackerType == "BOOSTING")
			tracker = TrackerBoosting::create();
		if (trackerType == "MIL")
			tracker = TrackerMIL::create();
		if (trackerType == "KCF")
			tracker = TrackerKCF::create();
		if (trackerType == "TLD")
			tracker = TrackerTLD::create();
		if (trackerType == "MEDIANFLOW")
			tracker = TrackerMedianFlow::create();
		if (trackerType == "GOTURN")
			tracker = TrackerGOTURN::create();
		if (trackerType == "MOSSE")
			tracker = TrackerMOSSE::create();
		if (trackerType == "CSRT")
			tracker = TrackerCSRT::create();
	}

    string path = "[path_to_file]";
	cv::namedWindow("Trackbar", cv::WINDOW_AUTOSIZE);
	video->open(path);

	int frames = (int)video->get(cv::CAP_PROP_FRAME_COUNT);

	//Trackbar for moving backwards and forwards inside the video
	cv::createTrackbar("Position", "Trackbar", &slider_pos, frames, on_trackbar);
	const double fps = video->get(cv::CAP_PROP_FPS);
	cout << "The Fps is: " << fps << endl;

	cv::Mat frame;
	int skip_frame = 10;

	//implementation of the tracker
	video->read(frame);
	video->set(CAP_PROP_FPS, fps);
	Rect2d bbox;
	bool ok = false;
	bool init = false;
    /* For each iteration a frame will be read and the
     * tracker automatically updates its position. Based on the tracker,
     * The behavior of the tracker will be different.
     */
	for (;;) {
		if (run != 0) {
			video->read(frame);
			if (ok == true) {
				tracker->update(frame, bbox);
				rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
			}
			int current_position = (int)video->get(cv::CAP_PROP_POS_FRAMES);
			dontset = 1;

			cv::setTrackbarPos("Position", "Trackbar", current_position);
			cv::imshow("Trackbar", frame);
			run -= 1;
		}
		char c = (char)cv::waitKey(8);
		int current = (int)video->get(cv::CAP_PROP_POS_FRAMES);
		if (c == 'f') {
			run = 1;
			cout << "You're on single step mode" << endl;
			imshow("Trackbar",frame);
		}
		if (c == 'r') {
			run = -1;
			cout << "You're on run mode" << endl;
		}
		if (c == 'p') {
			run = 0;
			cout << "You've paused the video" << endl;
		}
		if (c == 'b') {
			run = 1;
			video->set(cv::CAP_PROP_POS_FRAMES, current - skip_frame);
			cout << "Backwards" << endl;
		}
        //The ROI will be initialized if 't' was pressed
		if (c == 't') {
			bbox = selectROI("Trackbar", frame, false);
			rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
			tracker->init(frame, bbox);
			if (init == false) {
				init == true;
			}
			init == true;
			ok = true;
		}
		if (c == 27)
			break;
	}

