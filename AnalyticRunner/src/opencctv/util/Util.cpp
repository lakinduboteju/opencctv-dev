
#include "Util.hpp"

namespace opencctv {
namespace util {

std::string Util::trim(std::string sToTrim)
{
	boost::algorithm::trim(sToTrim);
	return sToTrim;
}

unsigned long long Util::getCurrentTimeMs()
{
	unsigned long long ret = 0;
	struct timeval t;
	gettimeofday(&t, NULL);
	ret = (((unsigned long long) t.tv_sec) * 1000)
			+ (((unsigned long long) t.tv_usec) / 1000);
	return ret;
}

int Util::getCurrentVmUsageKb()
{
	FILE* file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];
	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmRSS:", 6) == 0) {
			result = parseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}

int Util::parseLine(char* line)
{
	int i = strlen(line);
	while (*line < '0' || *line > '9')
		line++;
	line[i - 3] = '\0';
	i = atoi(line);
	return i;
}

} /* namespace util */
} /* namespace opencctv */
