#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <cstddef>
#include <utility>
#include <set>
#include <tuple>
#include <string>
#include <string_view>
#include <iomanip>
#include <algorithm>
#include <execution>

#include "BrowseConnect.hh"

using std::size_t;
using std::set;
using std::pair;
using std::tuple;
using std::string;
using std::string_view;
using std::find;
using std::find_if;
using std::max;
using std::setw;
using std::left;
using namespace std::literals::string_literals;
namespace execution = std::execution;

using odbc::Environment;
using odbc::Connection;

set<string> const &BrowseConnect::commandNames() const
{
    static set<string> const names { ".browseConnect" };

    return names;
}

static tuple<string, string, string> parseConnectionAttribute(pair<string, string> const &attribute)
{
    auto it = find(execution::par_unseq, attribute.first.begin(), attribute.first.end(), ':');
    string optionalMarker, attributeName, displayName;

    if (!attribute.first.empty() && *attribute.first.cbegin() == '*')
    {
	optionalMarker = string { attribute.first.cbegin(), ++attribute.first.cbegin() };

	if (it == attribute.first.end())
	    attributeName = string { ++attribute.first.cbegin(), attribute.first.end() };
	else
	{
	    attributeName = string { ++attribute.first.cbegin(), it };
	    displayName = string { ++it, attribute.first.end() };
	}
    }
    else
	if (it == attribute.first.end())
	    attributeName = string { attribute.first.cbegin(), attribute.first.end() };
	else
	{
	    attributeName = string { attribute.first.cbegin(), it };
	    displayName = string { ++it, attribute.first.end() };
	}

    return tuple { optionalMarker, attributeName, displayName };
}

void BrowseConnect::Functor::operator ()(string const &command, string::const_iterator it)
{
    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    if (it != command.end() || conn->connected())
    {
	auto attributes = conn->browseConnect(Environment::splitAttributes(string(it, command.end()), ';'));

	unsigned optionalMarkerLength = 0u, attributeNameLength = 0u, displayNameLength = 0u;

	for (auto const &attribute: attributes)
	{
	    auto [optionalMarker, attributeName, displayName] = parseConnectionAttribute(attribute);

	    optionalMarkerLength = (std::max<size_t>)(optionalMarkerLength, optionalMarker.size());
	    attributeNameLength = (std::max<size_t>)(attributeNameLength, attributeName.size());
	    displayNameLength = (std::max<size_t>)(displayNameLength, displayName.size());
	}

	for (auto const &attribute: attributes)
	{
	    auto [optionalMarker, attributeName, displayName] = parseConnectionAttribute(attribute);

	    if (optionalMarkerLength && optionalMarker.empty())
		optionalMarker = string_view { " " };


	    cout << '\t' << optionalMarker
		<< setw(attributeNameLength) << left << attributeName << " : "
		<< setw(displayNameLength) << left << displayName << " => "
		<< attribute.second << '\n';
	}
    }
    else
    {
	cout
	    << '\t' << "DSN    = ?\n"
	    << '\t' << "DRIVER = ?\n";
    }

    cout << '\n';
}

static BrowseConnect browseConnectCmd;
