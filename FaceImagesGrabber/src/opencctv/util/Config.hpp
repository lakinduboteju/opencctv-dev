
#ifndef OPENCCTV_CONFIG_HPP_
#define OPENCCTV_CONFIG_HPP_

#include <map> // std::map
#include <string> // std::string
#include <fstream> // to read files on disk
/* boost includes (/usr/local/include/) */
/* boost library links (/usr/local/lib/) -lboost_system */
#include <boost/config.hpp>
// boost library link for boost program options (/usr/local/lib/) -lboost_program_options
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include "../../opencctv/Exception.hpp"

namespace pod = boost::program_options::detail;

namespace opencctv {
namespace util {
const std::string PROPERTY_MQ_PORT = "mq_port";

const std::string PROPERTY_SERVER_IP = "hikvision_server_ip";
const std::string PROPERTY_SERVER_USERNAME = "hikvision_username";
const std::string PROPERTY_SERVER_PASSWORD = "hikvision_password";

const std::string PROPERTY_RESULTS_DB_SERVER_NAME = "results_db_server_name";
const std::string PROPERTY_RESULTS_DB_PORT = "results_db_port";
const std::string PROPERTY_RESULTS_DB_NAME = "results_db_name";
const std::string PROPERTY_RESULTS_DB_USERNAME = "results_db_username";
const std::string PROPERTY_RESULTS_DB_PASSWORD = "results_db_password";

const std::string PROPERTY_TIME_DIFF = "time_diff_in_sec";
const std::string PROPERTY_NUMB_SKIPS = "numb_of_frames_to_skip";

class Config {
private:
	static Config* _pConfig;
	std::map<std::string, std::string> _mConfigDetails;
	Config();
public:
	static Config* getInstance();
	std::string get(const std::string& sPropertyName);
	virtual ~Config();
};

} /* namespace util */
} /* namespace opencctv */

#endif /* OPENCCTV_CONFIG_HPP_ */
