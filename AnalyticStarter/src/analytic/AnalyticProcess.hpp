
#ifndef ANALYTIC_ANALYTICPROCESS_HPP_
#define ANALYTIC_ANALYTICPROCESS_HPP_

#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <signal.h>
#include "../opencctv/Exception.hpp"
#include "xml/AnalyticMessage.hpp"

namespace analytic {

class AnalyticProcess {
private:
	FILE* _pReadStream;
	pid_t _pid;
public:
	AnalyticProcess();
	bool start(const std::string sPathToAnalyticRunnerExecutable, unsigned int iAnalyticInstanceId, const std::string& sAnalyticPluginDirLocation, const std::string& sAnalyticPluginFilename, const std::string& sInputAnalyticQueueAddress, const std::string& sOutputAnalyticQueueAddress);
	bool stop();
	bool close();
	bool readOutputStreamOfAnalyticProcess(std::string& sStreamOutput);
	virtual ~AnalyticProcess();
};

} /* namespace analytic */

#endif /* ANALYTIC_ANALYTICPROCESS_HPP_ */
