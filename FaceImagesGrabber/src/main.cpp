#include <iostream>
#include <stdlib.h>     /* atoi */
using namespace std;

#include "opencctv/mq/MqUtil.hpp"
#include "opencctv/util/log/Loggers.hpp"
#include "opencctv/util/Config.hpp"

#include "opencctv/db/AnalyticResultGateway.hpp"
using namespace opencctv;

#include "analytic/AnalyticResult.hpp"
using namespace analytic;

#include <string>
#include <vector>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
using namespace boost;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <time.h>

time_t toTime(const string& sTimestamp)
{
	struct tm tm;
	strptime(sTimestamp.c_str(), "%Y%m%dT%H%M%SZ", &tm);
	time_t t = mktime(&tm);
	return t;
}

string toTimestamp(time_t t)
{
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&t);
	strftime(buf, sizeof(buf), "%Y%m%dT%H%M%SZ", &tstruct);
	return buf;
}

void getRects(const string& sResultText, std::vector<Rect>& vBoundingBoxes)
{
	using namespace boost::property_tree;
	ptree pt;
	std::istringstream iss(sResultText);
	try {
		read_xml(iss, pt);
		BOOST_FOREACH( ptree::value_type const& v, pt.get_child("result.boxes") ) {
			if(v.first == "box")
			{
				Rect bb;
				bb.x = v.second.get<int>("x");
				bb.y = v.second.get<int>("y");
				bb.width = v.second.get<int>("w");
				bb.height = v.second.get<int>("h");
				vBoundingBoxes.push_back(bb);
			}
		}
	} catch (boost::property_tree::xml_parser::xml_parser_error &e) {
		std::string sErrMsg = "Failed to parse result text. ";
		sErrMsg.append(e.what());
		throw opencctv::Exception(sErrMsg);
	}
}

void parseMessage(const string& sMsg, unsigned int& iId, string& sCameraId, string& sDstDirPath)
{
	boost::property_tree::ptree pt;
	std::istringstream iss(sMsg);
	try {
		read_xml(iss, pt);
		iId = lexical_cast<unsigned int>(pt.get<std::string>("request.id"));
		sCameraId = pt.get<std::string>("request.cameraid");
		sDstDirPath = pt.get<std::string>("request.dstdirpath");
	} catch (boost::property_tree::xml_parser::xml_parser_error &e) {
		std::string sErrMsg = "Failed to parse Msg. ";
		sErrMsg.append(e.what());
		throw opencctv::Exception(sErrMsg);
	}
	// boost::algorithm::trim (sTimestamp);
}

int main(int argc, char* argv[])
{
	// Loading Configuration file
	opencctv::util::Config* pConfig;
	try {
		pConfig = opencctv::util::Config::getInstance();
	} catch (opencctv::Exception &e) {
		std::string sErrMsg = "Failed to load Configuration file. ";
		sErrMsg.append(e.what());
		opencctv::util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		return -1;
	}
	opencctv::util::log::Loggers::getDefaultLogger()->info(
			"Loading Configuration file done.");

	// Creating MQ
	zmq::socket_t* pSocket = NULL;
	try {
		pSocket = mq::MqUtil::createNewMq(pConfig->get(util::PROPERTY_MQ_PORT), ZMQ_PULL);
	} catch (std::runtime_error &e) {
		std::string sErrMsg =
				"Failed to create MQ. ";
		sErrMsg.append(e.what());
		util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		return -1;
	}
	opencctv::util::log::Loggers::getDefaultLogger()->info(
				"MQ created.");

	while(pSocket)
	{
		std::string sRequest;
		try {
			mq::MqUtil::readFromSocket(pSocket, sRequest);
		} catch (std::runtime_error &e) {
			std::string sErrMsg =
					"Failed to read request from the MQ. ";
			sErrMsg.append(e.what());
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}

		unsigned int iId = 0;
		string sTimestamp, sCameraId, sDstDirPath;
		try
		{
			parseMessage(sRequest, iId, sCameraId, sDstDirPath);
		}
		catch(std::exception &e)
		{
			std::string sErrMsg = "Failed to parse request message. ";
			sErrMsg.append(e.what());
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}

		if(iId > 0 && !sCameraId.empty() && !sDstDirPath.empty())
		{
			// Get Result from Results DB
			opencctv::db::AnalyticResultGateway gateway;
			AnalyticResult result;
			unsigned int iAnalyticInstanceId;
			gateway.getResult(iId, result, iAnalyticInstanceId);
			sTimestamp = result.getTimestamp();

			time_t time = toTime(sTimestamp);
			std::vector<Rect> vBoundingBoxes;
			try
			{
				getRects(result.getCustomText(), vBoundingBoxes);
			}
			catch(std::exception &e)
			{
				std::string sErrMsg = "Failed to parse XML result text. ";
				sErrMsg.append(e.what());
				util::log::Loggers::getDefaultLogger()->error(sErrMsg);
			}

			stringstream ssRtspUri;
			ssRtspUri << "rtsp://"
					<< pConfig->get(util::PROPERTY_SERVER_USERNAME) << ":";
			ssRtspUri << pConfig->get(util::PROPERTY_SERVER_PASSWORD) << "@";
			ssRtspUri << pConfig->get(util::PROPERTY_SERVER_IP)
					<< ":554/ISAPI/Streaming/tracks/";
			ssRtspUri << sCameraId;

			int iTimeDiff = lexical_cast<int>(pConfig->get(util::PROPERTY_TIME_DIFF));
			int iSkips = lexical_cast<int>(pConfig->get(util::PROPERTY_NUMB_SKIPS));
			++iSkips;

			ssRtspUri << "?starttime=" << toTimestamp((time + iTimeDiff));

			VideoCapture cap(ssRtspUri.str());
			if(cap.isOpened())
			{
				Mat matImage;
				for (int i = 0; i < iSkips; ++i) {
					cap >> matImage;
				}
				if (!matImage.empty()) {
					for (size_t i = 0; i < vBoundingBoxes.size(); ++i) {
						Mat mati = matImage(vBoundingBoxes[i]);
						stringstream ss;
						ss << sDstDirPath << sTimestamp << "_" << i
								<< ".jpg";
						cv::imwrite(ss.str(), mati);
						std::string sMsg = "Image: ";
						sMsg.append(ss.str());
						sMsg.append(" created.");
						util::log::Loggers::getDefaultLogger()->debug(sMsg);
					}
				}
				matImage.release();
			}
			else
			{
				std::string sErrMsg = "Failed to open RTSP stream. ";
				util::log::Loggers::getDefaultLogger()->error(sErrMsg);
			}
		}
		else
		{
			std::string sErrMsg = "Empty Timestamp or CameraID or Destination Dir path. ";
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}
		stringstream ss;
		ss << "Done. " << sTimestamp;
		util::log::Loggers::getDefaultLogger()->info(ss.str());
	}
	return 0;
}
