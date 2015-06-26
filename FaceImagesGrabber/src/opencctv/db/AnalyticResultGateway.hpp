
#ifndef ANALYTICRESULTGATEWAY_HPP_
#define ANALYTICRESULTGATEWAY_HPP_

#include <vector>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "DbConnector.hpp"
#include "../Exception.hpp"
#include "../util/Config.hpp"
#include "../../analytic/AnalyticResult.hpp"

namespace opencctv {
namespace db {

class AnalyticResultGateway {
private:
private:
	sql::Connection* _pDbConnPtr;
	sql::PreparedStatement* _pStatementInsert;
	sql::PreparedStatement* _pStatementSelect;
	static const std::string _INSERT_ANALYTIC_RESULT_SQL;
	static const std::string _SELECT_ANALYTIC_RESULT_SQL;

public:
	AnalyticResultGateway();
	virtual ~AnalyticResultGateway();
	int insertResults(unsigned int iAnalyticInstanceId,analytic::AnalyticResult analyticResult);
	void getResult(unsigned int iId, analytic::AnalyticResult& analyticResult, unsigned int& iAnalyticInstanceId);
};

} /* namespace db */
} /* namespace opencctv */

#endif /* ANALYTICRESULTGATEWAY_HPP_ */
