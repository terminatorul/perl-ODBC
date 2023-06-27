#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>
#include <sql.h>
#include <sqlext.h>

#include <cstddef>
#include <string>
#include <map>
#include <numeric>
#include <algorithm>
#include <execution>
#include <exception>
#include <stdexcept>

#include "odbc++/Connection.hh"

using std::size_t;
using std::map;
using std::string;
using std::reduce;
using std::transform;
using std::find;
using std::runtime_error;
using std::uncaught_exceptions;
using namespace std::literals::string_literals;
namespace execution = std::execution;

void odbc3_0::Connection::disconnect()
{
    if (state != State::Disconnected)
    {
	switch (SQLDisconnect(sqlHandle))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
	    break;

	case SQL_STILL_EXECUTING:
	    if (!uncaught_exceptions())
		throw runtime_error("Operation in progress");
	    break;

	case SQL_ERROR:
	default:
	    if (!uncaught_exceptions())
		throw runtime_error("SQL Error.");
	    break;
	};

	state = State::Disconnected;
    }
}

map<string, string> odbc3_0::Connection::browseConnect(map<string, string> const &request)
{
    if (state == State::Connected)
	throw runtime_error("Already connected.");

    size_t requestStringSize = 2u;

    for (auto const &attribute: request)
	requestStringSize += attribute.first.size() + 1u + attribute.second.size() + 1u;

    sqlstring requestString;
    requestString.reserve(requestStringSize);

    for (auto const &requestAttribute: request)
    {
	if (!requestString.empty())
	    requestString.push_back(';');

	requestString.insert(requestString.end(), requestAttribute.first.begin(), requestAttribute.first.end());
	requestString.push_back('=');

	string attributeName(requestAttribute.first.size(), '\0');
	transform(execution::par_unseq, requestAttribute.first.begin(), requestAttribute.first.end(), attributeName.begin(), [](char ch) { return tolower(ch); });
	
	if (attributeName == "driver"s && (requestAttribute.second.empty() || *requestAttribute.second.begin() != '{'))
	    requestString.push_back('{');

	requestString.insert(requestString.end(), requestAttribute.second.begin(), requestAttribute.second.end());

	if (attributeName == "driver"s && (requestAttribute.second.empty() || *requestAttribute.second.begin() != '{'))
	    requestString.push_back('}');
    }

    return browseConnect(requestString.data(), static_cast<SQLSMALLINT>(requestString.size()));
}

map<string, string> odbc3_0::Connection::browseConnect(SQLCHAR *request, SQLSMALLINT requestSize)
{
    SQLSMALLINT resultStringSize;

    switch (SQLBrowseConnect(sqlHandle, request, requestSize, nullptr, 0, &resultStringSize))
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
    case SQL_NEED_DATA:
    {
	state = State::InProgress;
	sqlstring resultString(resultStringSize + 1u);

	switch (SQLBrowseConnect(sqlHandle, request, requestSize, resultString.data(), static_cast<SQLSMALLINT>(resultString.size()), &resultStringSize))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
	    state = State::Connected;
	    // fall-through

	case SQL_NEED_DATA:
	{
	    map<string, string> attributesMap;

	    auto it = resultString.begin();
	    auto jt = find(execution::par_unseq, it, resultString.end(), ';');

	    while (jt != resultString.end())
	    {
		auto kt = find(execution::par_unseq, it, jt, '=');

		if (kt != jt)
		    attributesMap[string(it, kt)] = string(kt + 1, jt);
		else
		    attributesMap[string(it, jt)] = string();

		it = ++jt;
		jt = find(execution::par_unseq, it, resultString.end(), ';');
	    }

	    return attributesMap;
	}
	case SQL_ERROR:
	default:
	    throw runtime_error("SQL Error");
	}

	break;
    }
    default:
	throw runtime_error("SQL Error");
    }
}
