
#include "ProducerThread.hpp"

ProducerThread::ProducerThread(const string& sVideoFilePath, ConcurrentQueue<api::Image_t>* pImageInputQueue) {
	_sVideoFilePath = sVideoFilePath;
	_pImageInputQueue = pImageInputQueue;
}

void ProducerThread::operator ()()
{
	VideoCapture cap(_sVideoFilePath);
	if(!cap.isOpened()) {
		cerr << "Failed to open input video file: " << _sVideoFilePath << endl;
		return;
	}
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	unsigned int delay = ((unsigned int)(((double)1.0 / fps) * (double)1000)); //delay between 2 frames (ms)
	clock_t begin = clock();
	int i = 0;
	while(1)
	{
		Mat matImage;
		cap >> matImage;
		if(!matImage.empty())
		{
			stringstream ss;
			clock_t end = clock();
			ss << (((double)1.0)/fps)*((double)i); // calculate timestamp in sec
			ss << "sec";
			api::Image_t inputImage = {matImage, 1, "default", ss.str(), std::string(), false};
			_pImageInputQueue->push(inputImage);
			usleep(delay);
			++i;
		}
	}
}

ProducerThread::~ProducerThread() {
}

