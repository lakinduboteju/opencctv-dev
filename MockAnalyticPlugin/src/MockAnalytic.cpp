
#include "MockAnalytic.hpp"

MockAnalytic::MockAnalytic() : api::Analytic() {
}

bool MockAnalytic::init(std::string sAnalyticPluginDirLocation)
{
	// do initialization
	return true;
}

void MockAnalytic::process(analytic::ConcurrentQueue<analytic::api::Image_t>* pInputQueue, analytic::ConcurrentQueue<analytic::api::Image_t>* pOutputQueue)
{
	while(true)
	{
		/* 1. get a image from input queue */
		api::Image_t image = pInputQueue->pop();
		cv::Mat matInputImage = image.matImage; // got it
		/* 2. clone the input image */
		cv::Mat matToBeProcessed = matInputImage.clone();
		/* 3. release the input image */
		matInputImage.release(); //please do this release!
		/* 4. OK, now use cloned image for analysis */

		// Start processing
		cv::Mat matOutputImage;
		cv::cvtColor(matToBeProcessed, matOutputImage, CV_BGR2GRAY);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		// freeing generated Mats
		matOutputImage.release();
		matToBeProcessed.release();

		/* 5. set output */
		image.bGenerateAnalyticEvent = true;
		image.sCustomTextResult = "Did a mock process.";
		/* 6. push into output queue */
		pOutputQueue->push(image);
	}
}

MockAnalytic::~MockAnalytic() {
}

