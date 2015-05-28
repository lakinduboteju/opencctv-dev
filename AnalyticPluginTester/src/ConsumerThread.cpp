
#include "ConsumerThread.hpp"

ConsumerThread::ConsumerThread(const string& sOutputFilename, ConcurrentQueue<api::Image_t>* pResultsOutputQueue) {
	_sOutputFilename = sOutputFilename;
	_pResultsOutputQueue = pResultsOutputQueue;
}

void ConsumerThread::operator ()()
{
	// open a file in write mode.
	ofstream outfile;
	outfile.open(_sOutputFilename.c_str());
	int i = 0;
	while(1)
	{
		api::Image_t outputImage = _pResultsOutputQueue->pop();
		outfile << i << ": " << "Event? " << outputImage.bGenerateAnalyticEvent << ", Timestamp: " << outputImage.sTimestamp << ", ResultText: " << outputImage.sCustomTextResult << endl;
		cout << i << ": " << "Event? " << outputImage.bGenerateAnalyticEvent << ", Timestamp: " << outputImage.sTimestamp << ", ResultText: " << outputImage.sCustomTextResult << endl; //print
		++i;
	}
}

ConsumerThread::~ConsumerThread() {
}

