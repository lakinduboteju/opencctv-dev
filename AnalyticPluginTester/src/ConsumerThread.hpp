
#ifndef CONSUMERTHREAD_HPP_
#define CONSUMERTHREAD_HPP_

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

#include "analytic/api/Analytic.hpp"
using namespace analytic;

class ConsumerThread {
private:
	string _sOutputFilename;
	ConcurrentQueue<api::Image_t>* _pResultsOutputQueue;
public:
	ConsumerThread(const string& sOutputFilename, ConcurrentQueue<api::Image_t>* pResultsOutputQueue);
	void operator ()();
	virtual ~ConsumerThread();
};

#endif /* CONSUMERTHREAD_HPP_ */
