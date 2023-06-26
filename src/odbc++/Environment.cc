#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

#include "odbc++/Environment.hh"

auto odbc3_0::Environment::FetchDriver(SQLUSMALLINT direction) -> std::pair<sqlstring, sqlstring>
{
    SQLSMALLINT descLen = 0, attrLen = 0;
    std::pair<sqlstring, sqlstring> driverInfo;

    switch (SQLDrivers(sqlHandle, direction, nullptr, 0, &descLen, nullptr, 0, &attrLen))
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
	driverInfo.first.resize(1024 /*descLen + 1*/);
	driverInfo.second.resize(1024 /* attrLen + 1 */ );

	switch (SQLDrivers(sqlHandle, direction, driverInfo.first.data(), /*descLen + 1*/ 1024, &descLen, driverInfo.second.data(), /*attrLen + 1*/ 1024, &attrLen))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
	    return driverInfo;

	case SQL_NO_DATA:
	    return std::pair<sqlstring, sqlstring> { };

	default:
	    throw std::runtime_error("SQL Error");
	}

    case SQL_NO_DATA:
	return std::pair<sqlstring, sqlstring> { };

    default:
	throw std::runtime_error("SQL Error");
    }
}

auto odbc3_0::Environment::drivers() -> std::vector<std::pair<sqlstring, sqlstring>>
{
    std::vector<std::pair<sqlstring, sqlstring>> driverList;
    SQLUSMALLINT direction = SQL_FETCH_FIRST;

    do
    {
	driverList.emplace_back(FetchDriver(direction));
	direction = SQL_FETCH_NEXT;
    }
    while (!driverList.crbegin()->first.empty());

    driverList.pop_back();

    return driverList;
}
