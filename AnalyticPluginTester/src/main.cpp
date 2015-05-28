#include <iostream>
#include <string>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace cv;

#include "analytic/ConcurrentQueue.hpp"
using namespace analytic;

#include "opencctv/PluginLoader.hpp"
using namespace opencctv;

#include "ProducerThread.hpp"
#include "ConsumerThread.hpp"

#include <stdio.h>
#include <time.h>

/* boost includes (/user/local/include) */
/* boost libraries (/usr/local/lib) -lboost_system */
#include <boost/thread/thread.hpp> // -lboost_thread -pthread
#include <boost/lockfree/queue.hpp>

// Get current date/time, format is YYYY-MM-DD_HH:mm:ss
std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d_%X", &tstruct);
    return buf;
}

int main(int argc, char* argv[]) {
	if (argc < 4) {
		cerr << "Invalid number of arguments." << endl;
		cout << "Usage: " << argv[0] << " <input_video_file_path> <analytic_plugin_dir_path> <analytic_plugin_filename>" << endl;
		cout << "Example: " << argv[0] << "input.mp4 ~/analytic/ analytic_plugin.so" << endl;
		return -1;
	}
	string input_video_file_path = argv[1];
	string analytic_plugin_dir_path = argv[2];
	string analytic_plugin_filename = argv[3];

	ConcurrentQueue<api::Image_t>* pImageInputQueue = new ConcurrentQueue<api::Image_t>(10);
	ConcurrentQueue<api::Image_t>* pResultsOutputQueue = new ConcurrentQueue<api::Image_t>(10);

	// Load Analytic
	api::Analytic* pAnalytic = NULL;
	PluginLoader<api::Analytic> loader;
	string sPathToAnalyticPlugin = analytic_plugin_dir_path;
	if(*sPathToAnalyticPlugin.rbegin() != '/') // last char
	{
		sPathToAnalyticPlugin.append("/");
	}
	sPathToAnalyticPlugin.append(analytic_plugin_filename);
	try
	{
		loader.loadPlugin(sPathToAnalyticPlugin);
		pAnalytic = loader.createPluginInstance();
	}
	catch(opencctv::Exception &e)
	{
		cerr << "Failed to load the Analytic plugin: " << sPathToAnalyticPlugin << endl;
		return -1;
	}

	// Create Consumer thread to write results into a txt file
	string sOutputFilename = analytic_plugin_dir_path;
	if(*sOutputFilename.rbegin() != '/') // last char
	{
		sOutputFilename.append("/");
	}
	sOutputFilename.append(currentDateTime());
	sOutputFilename.append(".txt");
	ConsumerThread resultsConsumer(sOutputFilename, pResultsOutputQueue);
	boost::thread* pConsumerThread = new boost::thread(resultsConsumer);

	// Create Producer thread to read frames from input video files
	ProducerThread imageProducer(input_video_file_path, pImageInputQueue);
	boost::thread* pProducerThread = new boost::thread(imageProducer);

	// Run analytic
	try
	{
		if(pAnalytic->init(analytic_plugin_dir_path))
		{
			pAnalytic->process(pImageInputQueue, pResultsOutputQueue);
		}
		else
		{
			cerr << "Failed to initialize the Analytic. init() failed.";
		}
	}
	catch(exception &e)
	{
		cerr << "Exception in Analytic: " << e.what() << endl;
	}

	return 0;
}
