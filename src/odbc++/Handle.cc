#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <vector>
#include <tuple>
#include <stdexcept>

#include "odbc++/Handle.hh"

using std::runtime_error;
using std::tuple;
using std::get;
using std::vector;

std::tuple<odbc::Handle::sqlstring, SQLINTEGER, odbc::Handle::sqlstring>
    odbc::Handle::diagnosticRecord(SQLSMALLINT recordNumber)
{
    SQLINTEGER nativeError = 0;
    sqlstring sqlState(6u, '\0');
    sqlstring message;
    SQLSMALLINT messageSize;

    switch (SQLGetDiagRec(handleType, sqlHandle, recordNumber, sqlState.data(), &nativeError, nullptr, 0, &messageSize))
    {
    case SQL_SUCCESS:
	if (!messageSize)
	{
	    sqlState.resize(5u);
	    return tuple{ sqlState, nativeError, sqlstring() };
	}
	// fall-through

    case SQL_SUCCESS_WITH_INFO:
	message.resize(messageSize + 1u);

	switch (SQLGetDiagRec(handleType, sqlHandle, recordNumber, sqlState.data(), &nativeError, message.data(), message.size(), &messageSize))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
	    sqlState.resize(5u);
	    message.resize(messageSize);
	    return tuple { sqlState, nativeError, message };

	case SQL_NO_DATA:
	    return tuple { sqlstring { }, 0, sqlstring { } };

	case SQL_INVALID_HANDLE:
	    throw runtime_error("Invalid handle");

	case SQL_ERROR:
	default:
	    throw runtime_error("SQL Error");
	}

    case SQL_NO_DATA:
	return tuple { sqlstring { }, 0, sqlstring { } };

    case SQL_INVALID_HANDLE:
	throw runtime_error("Invalid handle");

    case SQL_ERROR:
    default:
	throw runtime_error("SQL Error");
    }
}

auto odbc::Handle::diagnosticRecords() -> vector<tuple<string, SQLINTEGER, string>>
{
    vector<tuple<string, SQLINTEGER, string>>
	records;

    SQLSMALLINT recordNumber = 1;

    tuple<sqlstring, SQLINTEGER, sqlstring> record = diagnosticRecord(recordNumber++);

    while (get<0>(record).size())
    {
	records.emplace_back
	(
	    string(get<0>(record).begin(), get<0>(record).end()),
	    get<1>(record),
	    string(get<2>(record).begin(), get<2>(record).end())
	);

	record = diagnosticRecord(recordNumber++);
    }

    return records;
}
