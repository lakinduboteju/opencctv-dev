
#ifndef PRODUCERTHREAD_HPP_
#define PRODUCERTHREAD_HPP_

#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include "analytic/api/Analytic.hpp"
using namespace analytic;

#include <ctime>
#include <unistd.h>

class ProducerThread {
private:
	string _sVideoFilePath;
	ConcurrentQueue<api::Image_t>* _pImageInputQueue;
public:
	ProducerThread(const string& sVideoFilePath, ConcurrentQueue<api::Image_t>* pImageInputQueue);
	void operator ()();
	virtual ~ProducerThread();
};

#endif /* PRODUCERTHREAD_HPP_ */
