
#include "AnalyticProcess.hpp"

namespace analytic {

AnalyticProcess::AnalyticProcess() {
	_pReadStream = NULL;
	_pid = 0;
}

bool AnalyticProcess::start(const std::string sPathToAnalyticRunnerExecutable, unsigned int iAnalyticInstanceId,
		const std::string& sAnalyticPluginDirLocation, const std::string& sAnalyticPluginFilename,
		const std::string& sInputAnalyticQueueAddress, const std::string& sOutputAnalyticQueueAddress) {
	bool bRet = false;
	std::stringstream ssCommand;
	ssCommand << sPathToAnalyticRunnerExecutable << " ";
	ssCommand << iAnalyticInstanceId << " ";
	ssCommand << sAnalyticPluginDirLocation << " ";
	ssCommand << sAnalyticPluginFilename << " ";
	ssCommand << sInputAnalyticQueueAddress << " ";
	ssCommand << sOutputAnalyticQueueAddress;
	_pReadStream = popen(ssCommand.str().data(), "r");
	if (!_pReadStream) {
		std::string sErrMsg = "Failed to start Analytic process. Failed to execute command: ";
		sErrMsg.append(ssCommand.str());
		throw opencctv::Exception(sErrMsg);
	} else {
		const int iBufSize = 1024;
		char buf[iBufSize];
		char* pCh = fgets(buf, iBufSize, _pReadStream);
		if (pCh) {
			std::string sStreamOutput = std::string(buf);
			try {
				_pid = xml::AnalyticMessage::getPid(sStreamOutput);
			} catch (opencctv::Exception &e) {
				std::string sErrMsg = "Failed to retrieve PID of Analytic process. Analytic process output: ";
				sErrMsg.append(sStreamOutput);
				sErrMsg.append(e.what());
				throw opencctv::Exception(sErrMsg);
			}
		}
		if (_pid > 0) {
			bRet = true;
		}
	}
	return bRet;
}

bool AnalyticProcess::stop() {
	bool bRet = false;
	if (_pid > 0) {
		if (kill(_pid, SIGKILL) == 0) {
			if (_pReadStream) {
				pclose(_pReadStream);
				// delete _pReadStream;
			}
			bRet = true;
		}
	}
	return bRet;
}

bool AnalyticProcess::readOutputStreamOfAnalyticProcess(
		std::string& sStreamOutput) {
	const int iSize = 2048;
	char buf[iSize];
	char* pCh = fgets(buf, iSize, _pReadStream);
	if (pCh) {
		sStreamOutput = std::string(buf);
		return true;
	}
	return false;
}

AnalyticProcess::~AnalyticProcess() {
	if (_pReadStream) {
		// pclose(_pReadStream);
		// delete _pReadStream;
	}
}

} /* namespace analytic */
