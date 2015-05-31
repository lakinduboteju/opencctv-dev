
#ifndef OPENCCTV_UTIL_HPP_
#define OPENCCTV_UTIL_HPP_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <string>
#include <sys/time.h> // to get time
/* boost includes (/usr/local/include/) */
/* boost library links (/usr/local/lib/) -lboost_system */
#include <boost/algorithm/string.hpp> // boost::algorithm::trim()

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"

namespace opencctv {
namespace util {

class Util {
private:
	static int parseLine(char* line);
	static bool scanDir(const std::string& sDirPath, int& iFileCount, std::string& sLastFilePath, int& iSharedLibFileCount, std::string& sLastSharedLibFilePath);
public:
	static std::string trim(std::string sToTrim);
	static unsigned long long getCurrentTimeMs();
	static int getCurrentVmUsageKb();
	static bool findSharedLibOfPlugin(const std::string& sPathToPluginDir, std::string& sPathToSharedLibOfPlugin);
};

} /* namespace util */
} /* namespace opencctv */

#endif /* OPENCCTV_UTIL_HPP_ */
