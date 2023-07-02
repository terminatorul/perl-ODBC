#include <utility>
#include <tuple>
#include <string>
#include <stdexcept>

#include "Handle.hh"

namespace odbc
{
    class Handle;

    class SQLDiagnosticException: public std::runtime_error
    {
    protected:
	std::vector<std::tuple<std::string, SQLINTEGER, std::string>>
	    records;

	mutable std::string recordListMessage;
    public:
	SQLINTEGER recordCount();
	std::tuple<std::string, SQLINTEGER, std::string> const
	    &record(unsigned recordNumber) const;
	std::string const &sqlState(unsigned recordNumber) const;
	SQLINTEGER nativeError(unsigned recordNumber) const;
	std::string const &message(unsigned recordNumber) const;

	SQLDiagnosticException(std::vector<std::tuple<std::string, SQLINTEGER, std::string>> &&recods);
	SQLDiagnosticException(std::vector<std::tuple<std::string, SQLINTEGER, std::string>> const &recods);
	SQLDiagnosticException(Handle &sqlHandle);

	virtual char const *what() const override;
    };
}

inline odbc::SQLDiagnosticException::SQLDiagnosticException(std::vector<std::tuple<std::string, SQLINTEGER, std::string>> const &recods)
    : runtime_error(std::string()), records(records)
{
}

inline odbc::SQLDiagnosticException::SQLDiagnosticException(std::vector<std::tuple<std::string, SQLINTEGER, std::string>> &&recods)
    : runtime_error(std::string()), records(std::move(records))
{
}

inline odbc::SQLDiagnosticException::SQLDiagnosticException(Handle &sqlHandle)
    : runtime_error(std::string()), records(sqlHandle.diagnosticRecords())
{
}

SQLINTEGER odbc::SQLDiagnosticException::recordCount()
{
    return records.size();
}

std::tuple<std::string, SQLINTEGER, std::string> const
    &odbc::SQLDiagnosticException::record(unsigned recordNumber) const
{
    return records[recordNumber];
}

std::string const &odbc::SQLDiagnosticException::sqlState(unsigned recordNumber) const
{
    return std::get<0>(records[recordNumber]);
}

SQLINTEGER odbc::SQLDiagnosticException::nativeError(unsigned recordNumber) const
{
    return std::get<1>(records[recordNumber]);
}

std::string const &odbc::SQLDiagnosticException::message(unsigned recordNumber) const
{
    return std::get<2>(records[recordNumber]);
}

char const *odbc::SQLDiagnosticException::what() const
{
    if (recordListMessage.empty())
    {
	for (auto const &record : records)
	    recordListMessage += std::get<0>(record) + " " + std::get<2>(record) + '\n';

	recordListMessage.pop_back();
    }

    return recordListMessage.c_str();
}
