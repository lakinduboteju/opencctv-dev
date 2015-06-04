/*
 * StreamGateway.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: anjana
 */

#include "StreamGateway.hpp"

namespace opencctv {
namespace db {

const std::string StreamGateway::_SELECT_STREAM_SQL = "SELECT DISTINCT s.id, s.width, s.height, s.keep_aspect_ratio, s.allow_upsizing, s.compression_rate, ca.camera_id, vms.vms_connector_id, vms.server_ip, vms.server_port, vms.username, vms.password, vmsc.filename FROM streams AS s, cameras AS ca, vmses as vms, vms_connectors as vmsc WHERE (s.verified = TRUE) AND (s.camera_id = ca.id) AND (ca.vms_id = vms.id) AND (vms.vms_connector_id = vmsc.id) AND (s.id IN (SELECT DISTINCT ais.stream_id FROM analytic_instance_streams as ais))";

StreamGateway::StreamGateway() {
	_pDbConnPtr = DbConnector::getConnection();
}

StreamGateway::~StreamGateway() {
	delete _pDbConnPtr;
	_pDbConnPtr = NULL;
}

void StreamGateway::findAll(std::vector<opencctv::dto::Stream>& vToStoreStreams)
{

	try
	{
		sql::Statement* pStatement = (*_pDbConnPtr).createStatement();
		sql::ResultSet* pResultsPtr = (*pStatement).executeQuery(_SELECT_STREAM_SQL);
		opencctv::dto::Stream stream;
		while((*pResultsPtr).next())
		{
			stream.setId((*pResultsPtr).getInt("id"));
			stream.setWidth((*pResultsPtr).getInt("width"));
			stream.setHeight((*pResultsPtr).getInt("height"));
			stream.setKeepAspectRatio((*pResultsPtr).getBoolean("keep_aspect_ratio"));
			stream.setAllowUpSizing((*pResultsPtr).getBoolean("allow_upsizing"));
			stream.setCompressionRate((*pResultsPtr).getInt("compression_rate"));
			stream.setCameraId((*pResultsPtr).getString("camera_id"));
			stream.setVmsTypeId((*pResultsPtr).getInt("vms_connector_id"));
			stream.setVmsServerIp((*pResultsPtr).getString("server_ip"));
			stream.setVmsServerPort((*pResultsPtr).getInt("server_port"));
			stream.setVmsUsername((*pResultsPtr).getString("username"));
			stream.setVmsPassword((*pResultsPtr).getString("password"));

			//opencctv::util::Config* pConfig = opencctv::util::Config::getInstance();
			//std::string sVmsConBasePath = pConfig->get(opencctv::util::PROPERTY_VMS_CONNECTOR_DIR);
			//std::string sVmsConnectorDir = (*pResultsPtr).getString("filename");
			//std::string sVmsConnectorDirLocation;
			//sVmsConnectorDirLocation.append(sVmsConBasePath);
			//sVmsConnectorDirLocation.append("/");
			//sVmsConnectorDirLocation.append(sVmsConnectorDir);
			//stream.setVmsConnectorDirLocation(sVmsConnectorDirLocation);

			stream.setVmsConnectorFilename((*pResultsPtr).getString("filename"));
			stream.setVmsConnectorDirLocation((*pResultsPtr).getString("filename"));

			vToStoreStreams.push_back(stream);
		}
		(*pResultsPtr).close();
		(*pStatement).close();
		delete pResultsPtr;
		delete pStatement;

	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "StreamGateway:findAll: ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
		//std::cerr << "StreamGateway:findAll: Error while finding all streams from the database. " << e.what() << std::endl;
		// TODO: log
	}
}

} /* namespace db */
} /* namespace opencctv */
