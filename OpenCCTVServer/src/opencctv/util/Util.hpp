
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

namespace opencctv {
namespace util {

class Util {
private:
	static int parseLine(char* line);
public:
	static std::string trim(std::string sToTrim);
	static unsigned long long getCurrentTimeMs();
	static int getCurrentVmUsageKb();
};

} /* namespace util */
} /* namespace opencctv */

#endif /* OPENCCTV_UTIL_HPP_ */
