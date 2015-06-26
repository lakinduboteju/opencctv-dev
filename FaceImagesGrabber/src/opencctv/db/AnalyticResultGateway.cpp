
#include "AnalyticResultGateway.hpp"

namespace opencctv {
namespace db {

const std::string AnalyticResultGateway::_INSERT_ANALYTIC_RESULT_SQL = "INSERT INTO results (analytic_instance_id, timestamp, result_text) VALUES (?, ?, ?)";
const std::string AnalyticResultGateway::_SELECT_ANALYTIC_RESULT_SQL = "SELECT analytic_instance_id, timestamp, result_text FROM results WHERE id = ?";

AnalyticResultGateway::AnalyticResultGateway()
{
	try
	{
		_pDbConnPtr = DbConnector::getConnection_ResultsDB();
		_pStatementInsert = (*_pDbConnPtr).prepareStatement(_INSERT_ANALYTIC_RESULT_SQL);
		_pStatementSelect = (*_pDbConnPtr).prepareStatement(_SELECT_ANALYTIC_RESULT_SQL);

	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "Error while initializing the AnalyticResultGateway - .";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
	}
	catch(opencctv::Exception& e)
	{
		throw opencctv::Exception(e);
	}
}

AnalyticResultGateway::~AnalyticResultGateway() {
	(*_pStatementInsert).close();
	delete _pStatementInsert;
	_pStatementInsert = NULL;

	(*_pStatementSelect).close();
	delete _pStatementSelect;
	_pStatementSelect = NULL;

	delete _pDbConnPtr;
	_pDbConnPtr = NULL;
}

int AnalyticResultGateway::insertResults(unsigned int iAnalyticInstanceId,analytic::AnalyticResult analyticResult)
{
	int result = 0;

	try{
		(*_pStatementInsert).setInt(1, iAnalyticInstanceId);
		(*_pStatementInsert).setString(2, analyticResult.getTimestamp());
		(*_pStatementInsert).setString(3, analyticResult.getCustomText());
		result = (*_pStatementInsert).executeUpdate();

	}catch(sql::SQLException &e)
	{
		result = -1;
		std::string sErrorMsg = "Error while Inserting analytics results to the database. - ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
	}

	return result;
}

void AnalyticResultGateway::getResult(unsigned int iId, analytic::AnalyticResult& analyticResult, unsigned int& iAnalyticInstanceId)
{
	try
	{
		(*_pStatementSelect).setInt(1, iId);
		sql::ResultSet* pResult = (*_pStatementSelect).executeQuery();
		if((*pResult).next())
		{
			iAnalyticInstanceId = (*pResult).getInt("analytic_instance_id");
			analyticResult.setTimestamp((*pResult).getString("timestamp"));
			analyticResult.setCustomText((*pResult).getString("result_text"));
		}
	}
	catch (sql::SQLException &e) {
		std::string sErrorMsg = "Error while selecting analytic result from the database. - ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
	}
}


} /* namespace db */
} /* namespace opencctv */
