#if !defined(SQL_ODBCXX_HANDLE)
#define SQL_ODBCXX_HANDLE

#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>

#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

#include "Exports.h"

namespace odbc
{
    class ODBCXX_EXPORT Handle
    {
    protected:
	SQLSMALLINT handleType;
	SQLHANDLE sqlHandle = SQL_NULL_HANDLE;
	Handle(SQLSMALLINT handleType, SQLHANDLE inputHandle);

	using string = std::string;
	using sqlstring = std::vector<SQLCHAR>;

	std::tuple<sqlstring, SQLINTEGER, sqlstring>
	    diagnosticRecord(SQLSMALLINT recordNumber);

    public:
	Handle(Handle const &other) = delete;
	Handle(Handle &&other) noexcept;
	Handle(SQLSMALLINT handleType, Handle const &inputHandle);
	~Handle();

	std::vector<std::tuple<string, SQLINTEGER, string>>
	    diagnosticRecords();

	Handle &operator =(Handle const &other) = delete;
	Handle &operator =(Handle &&other);
    };
}

inline odbc::Handle::Handle(SQLSMALLINT handleType, SQLHANDLE inputHandle)
    : handleType(handleType)
{
    switch (SQLAllocHandle(handleType, inputHandle, &sqlHandle))
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
	return;

    case SQL_INVALID_HANDLE:
	throw std::runtime_error("Invalid handle");

    default:
	throw std::runtime_error("SQL Error");
    }
}

inline odbc::Handle::Handle(SQLSMALLINT handleType, Handle const &inputHandle)
    : Handle(handleType, inputHandle.sqlHandle)
{
}

inline odbc::Handle::Handle(Handle &&other) noexcept
    : handleType(other.handleType), sqlHandle(std::exchange(other.sqlHandle, SQLHANDLE { SQL_NULL_HANDLE }))
{
}

inline odbc::Handle &odbc::Handle::operator =(Handle &&other)
{
    handleType = other.handleType;
    sqlHandle = other.sqlHandle;

    other.sqlHandle = SQL_NULL_HANDLE;

    return *this;
}

inline odbc::Handle::~Handle()
{
    if (sqlHandle != SQL_NULL_HANDLE)
    {
	SQLFreeHandle(handleType, sqlHandle);
	sqlHandle = SQL_NULL_HANDLE;
    }
}

#endif	    // !defined(SQL_ODBCXX_HANDLE)
