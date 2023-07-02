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

#include <cstddef>
#include <string>
#include <map>
#include <span>
#include <numeric>
#include <algorithm>
#include <execution>
#include <exception>
#include <stdexcept>

#include "odbc++/SQLDiagnosticException.hh"
#include "odbc++/Connection.hh"

using std::size_t;
using std::map;
using std::span;
using std::string;
using std::reduce;
using std::transform;
using std::find;
using std::runtime_error;
using std::uncaught_exceptions;
using namespace std::literals::string_literals;
namespace execution = std::execution;

odbc::Connection::State odbc::Connection::connected() const noexcept
{
    return state;
}

bool odbc::Connection::disconnect()
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
		throw SQLDiagnosticException(*this);
	    break;
	};

	state = State::Disconnected;

	return true;
    }

    return false;
}

map<string, string> odbc::Connection::browseConnect(map<string, string> const &request)
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

    return browseConnect(span<SQLCHAR> { requestString.data(), requestString.size() });
}

odbc::Connection::BrowseConnectResult odbc::Connection::nativeBrowseConnect(span<SQLCHAR> request, sqlstring &resultString)
{
    SQLSMALLINT resultStringSize = 0;

    switch (SQLBrowseConnect(sqlHandle, request.data(), request.size(), resultString.data(), resultString.size(), &resultStringSize))
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
	return BrowseConnectResult::Connected;

    case SQL_NEED_DATA:
	if (resultStringSize > resultString.size())
	{
	    resultString.resize(resultStringSize + 1u);
	    return BrowseConnectResult::Again;
	}

	resultString.resize(resultStringSize);

	return BrowseConnectResult::More;

    case SQL_ERROR:
    default:
	throw SQLDiagnosticException(*this);
    }
}

map<string, string> odbc::Connection::browseConnect(span<SQLCHAR> request)
{
    sqlstring resultString(1024u + 1u);

    BrowseConnectResult result;
    while ((result = nativeBrowseConnect(request, resultString)) == BrowseConnectResult::Again)
	;

    if (result == BrowseConnectResult::Connected)
    {
	state = State::Connected;
	return map<string, string> { };
    }

    state = State::InProgress;

    map<string, string> attributesMap;

    auto it = resultString.begin();
    auto jt = find(execution::par_unseq, it, resultString.end(), ';');

    while (it != jt)
    {
	auto kt = find(execution::par_unseq, it, jt, '=');

	if (kt != jt)
	    attributesMap[string(it, kt)] = string(kt + 1, jt);
	else
	    attributesMap[string(it, jt)] = string();

	if (jt != resultString.end())
	{
	    it = ++jt;
	    jt = find(execution::par_unseq, it, resultString.end(), ';');
	}
	else
	    it = jt;
    }

    return attributesMap;
}
