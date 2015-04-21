
#include "opencctv/mq/MqUtil.hpp"
#include "analytic/xml/AnalyticMessage.hpp"
#include "opencctv/Exception.hpp"
#include "opencctv/util/log/Loggers.hpp"
#include "analytic/AnalyticProcess.hpp"
#include "analytic/util/Config.hpp"
#include <string>
#include <sstream>
#include <map>

using namespace opencctv;

int main()
{
	// Loading Configuration file
	analytic::util::Config* pConfig;
	try
	{
		pConfig = analytic::util::Config::getInstance();
	}
	catch(Exception &e)
	{
		std::string sErrMsg = "Failed to load Configuration file. ";
		sErrMsg.append(e.what());
		opencctv::util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		return -1;
	}
	opencctv::util::log::Loggers::getDefaultLogger()->info("Loading Configuration file done.");

	// Initializing values
	std::string sStartingPort = pConfig->get(analytic::util::PROPERTY_STARTER_PORT);
	if(sStartingPort.empty())
	{
		opencctv::util::log::Loggers::getDefaultLogger()->error("Failed to retrieve Analytic Starter port from Configuration file.");
		return -1;
	}
	unsigned int iStartingPort = boost::lexical_cast<unsigned int>(sStartingPort);
	std::map<unsigned int, analytic::AnalyticProcess> mAnalyticProcesses;
	opencctv::util::log::Loggers::getDefaultLogger()->info("Initializing values done.");

	// Creating Analytic Server's request-reply MQ
	zmq::socket_t* pSocket = NULL;
	try
	{
		pSocket = mq::MqUtil::createNewMq(sStartingPort, ZMQ_REP);
	}
	catch(std::runtime_error &e)
	{
		std::string sErrMsg = "Failed to create Analytic Server's request-reply MQ. ";
		sErrMsg.append(e.what());
		util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		return -1;
	}
	opencctv::util::log::Loggers::getDefaultLogger()->info("Creating Analytic Server's request-reply MQ done.");

	// Starting listening for requests
	opencctv::util::log::Loggers::getDefaultLogger()->info("Listening for requests started.");
	while(pSocket)
	{
		std::string sRequest;
		std::string sReply = "Error";
		try
		{
			mq::MqUtil::readFromSocket(pSocket, sRequest);
		}
		catch(std::runtime_error &e)
		{
			std::string sErrMsg = "Failed to read request from the request-reply MQ. ";
			sErrMsg.append(e.what());
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}
		std::string sOperation;
		try
		{
			sOperation = analytic::xml::AnalyticMessage::extractAnalyticRequestOperation(sRequest);
		}
		catch(Exception &e)
		{
			std::string sErrMsg = "Failed to extract the operation from XML request. ";
			sErrMsg.append(e.what());
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}
		// Analytic Start request
		if(sOperation.compare(analytic::xml::OPERATION_START_ANALYTIC) == 0)
		{
			// Extracting request data
			unsigned int iAnalyticInstanceId;
			std::string sAnalyticDirPath;
			std::string sAnalyticFilename;
			try
			{
				analytic::xml::AnalyticMessage::extractAnalyticStartRequestData(sRequest, iAnalyticInstanceId, sAnalyticDirPath, sAnalyticFilename);
			}
			catch(Exception &e)
			{
				std::string sErrMsg = "Failed to extract data from Analytic Start request. ";
				sErrMsg.append(e.what());
				util::log::Loggers::getDefaultLogger()->error(sErrMsg);
			}
			// Creating Analytic process
			if(iAnalyticInstanceId > 0 && !sAnalyticDirPath.empty() && !sAnalyticFilename.empty())
			{
				bool bAIStarted = false;
				analytic::AnalyticProcess analyticProcess;
				std::string sAnalyticInputQueueAddress = boost::lexical_cast<std::string>(++iStartingPort);
				std::string sAnalyticOutputQueueAddress = boost::lexical_cast<std::string>(++iStartingPort);
				try
				{
					std::string sAnalyticRunnerPath;
					sAnalyticRunnerPath.append(pConfig->get(analytic::util::PROPERTY_ANALYTIC_RUNNER_DIR));
					sAnalyticRunnerPath.append("/");
					sAnalyticRunnerPath.append(pConfig->get(analytic::util::PROPERTY_ANALYTIC_RUNNER_FILENAME));
					bAIStarted = analyticProcess.start(sAnalyticRunnerPath, iAnalyticInstanceId, sAnalyticDirPath, sAnalyticFilename, sAnalyticInputQueueAddress, sAnalyticOutputQueueAddress);
				}
				catch(opencctv::Exception &e)
				{
					std::stringstream ssErrMsg;
					ssErrMsg << "Failed to start Analytic Instance " << iAnalyticInstanceId << ". ";
					ssErrMsg << e.what();
					util::log::Loggers::getDefaultLogger()->error(ssErrMsg.str());
				}
				if(bAIStarted)
				{
					std::stringstream ssMsg;
					ssMsg << "Analytic Instance " << iAnalyticInstanceId << " started. ";
					ssMsg << "Input queue at: " << sAnalyticInputQueueAddress << ", ";
					ssMsg << "Output queue at: " << sAnalyticOutputQueueAddress;
					util::log::Loggers::getDefaultLogger()->info(ssMsg.str());
					try {
						sReply = analytic::xml::AnalyticMessage::getAnalyticStartReply( sAnalyticInputQueueAddress, sAnalyticOutputQueueAddress);
					} catch (Exception &e) {
						std::string sErrMsg = "Failed to create Analytic Start Reply XML message. ";
						sErrMsg.append(e.what());
						util::log::Loggers::getDefaultLogger()->error(sErrMsg);
					}
				}
				else
				{
					std::stringstream ssErrMsg;
					ssErrMsg << "Failed to start Analytic Instance " << iAnalyticInstanceId << ". ";
					util::log::Loggers::getDefaultLogger()->error(ssErrMsg.str());
				}
				// Sending Analytic Start reply
				bool bASRSent = false;
				try
				{
					bASRSent = opencctv::mq::MqUtil::writeToSocket(pSocket, sReply);
				}
				catch(std::runtime_error &e)
				{
					std::string sErrMsg = "Failed to send Analytic Start Reply. ";
					sErrMsg.append(e.what());
					util::log::Loggers::getDefaultLogger()->error(sErrMsg);
				}
				if(!bASRSent)
				{
					util::log::Loggers::getDefaultLogger()->error("Failed to send Analytic Start Reply.");
				}
			}
			else
			{
				std::string sErrMsg = "Request with invalid data.\nRequest: ";
				sErrMsg.append(sRequest);
				util::log::Loggers::getDefaultLogger()->error(sErrMsg);
			}
		}
		else
		{
			std::string sErrMsg = "Request with an unknown Operation.\nRequest: ";
			sErrMsg.append(sRequest);
			util::log::Loggers::getDefaultLogger()->error(sErrMsg);
		}
	}
	if(pSocket) delete pSocket;
	return 0;
}
