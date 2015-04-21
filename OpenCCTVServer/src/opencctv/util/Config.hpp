
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

const std::string PROPERTY_ANALYTIC_SERVER_IP = "analytic_server_ip";
const std::string PROPERTY_ANALYTIC_SERVER_PORT = "analytic_server_port";
const std::string PROPERTY_INTERNAL_QUEUE_SIZE = "internal_queue_size";
const std::string PROPERTY_REMOTE_QUEUE_SIZE = "remote_queue_size";

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
