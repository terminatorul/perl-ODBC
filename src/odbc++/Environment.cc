#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <windef.h>
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <execution>
#include <stdexcept>

#include "odbc++/Environment.hh"

using std::string;
using std::basic_string;
using std::vector;
using std::map;
using std::pair;
using std::runtime_error;
using std::find;
using std::move;

namespace execution = std::execution;

void odbc3_0::Environment::FetchDriver(SQLUSMALLINT direction, pair<sqlstring, sqlstring> &driverInfo)
{
    SQLSMALLINT descLen = 0, attrLen = 0;

    switch
	(
	    SQLDrivers
		(
		    sqlHandle, direction,
		    driverInfo.first.data(),  static_cast<SQLSMALLINT>(driverInfo.first.size()), &descLen,
		    driverInfo.second.data(), static_cast<SQLSMALLINT>(driverInfo.second.size()), &attrLen
		)
	)
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
	driverInfo.first.pop_back();
	driverInfo.second.pop_back();
	return;

    case SQL_NO_DATA:
	throw runtime_error("No driver info.");

    case SQL_ERROR:
    default:
	throw runtime_error("SQL Error.");
    }
}

static pair<string, map<string, string>> MakeDriverInfo(vector<SQLCHAR> &description, vector<SQLCHAR> &attributes)
{
    string strDescription, attributeName, attributeValue;

    strDescription.reserve(description.size());
    strDescription.assign(description.begin(), description.end());

    auto it = attributes.begin();
    auto jt = find(execution::par_unseq, it, attributes.end(), '\0');

    map<string, string> attributesMap;

    while (jt != attributes.end())
    {
	auto kt = find(execution::par_unseq, it, jt, '=');

	if (kt != jt)
	    attributesMap[string(it, kt)] = string(kt + 1, jt);
	else
	    attributesMap[string(it, jt)] = string();

	it = ++jt;
	jt = find(execution::par_unseq, it, attributes.end(), '\0');
    }

    return { move(strDescription), move(attributesMap) };
}

auto odbc3_0::Environment::drivers() -> map<string, map<string, string>>
{
    SQLSMALLINT descLen = 0, attrLen = 0;
    vector<pair<sqlstring, sqlstring>> driverList;
    SQLUSMALLINT direction = SQL_FETCH_FIRST;

    do
    {
	switch (SQLDrivers(sqlHandle, direction, nullptr, 0, &descLen, nullptr, 0, &attrLen))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
	    driverList.emplace_back(sqlstring(descLen + 1u), sqlstring(attrLen + 1u));
	    break;

	case SQL_NO_DATA:
	    descLen = -1;
	    break;

	case SQL_ERROR:
	    throw runtime_error("SQL Error");
	}

	direction = SQL_FETCH_NEXT;
    }
    while (descLen >= 0);

    map<string, map<string, string>> drivers;

    for (auto &driverInfo: driverList)
    {
	FetchDriver(&driverInfo == &*driverList.begin() ? SQL_FETCH_FIRST : SQL_FETCH_NEXT, driverInfo);
	drivers.emplace(MakeDriverInfo(driverInfo.first, driverInfo.second));
    }

    return drivers;
}
