#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <cstdlib>
#include <string>
#include <exception>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <execution>

#include "odbc++/Environment.hh"
#include "odbc++/Connection.hh"

using std::string;
using namespace std::literals::string_literals;
using std::exception;
using std::getline;
using std::setw;
using std::left;
using std::clog;
using std::cin;
using std::cout;
using std::max_element;
using std::find_if;
namespace execution = std::execution;

namespace odbc = odbc3_0;
using odbc::Environment;
using odbc::Connection;

static string::const_iterator ScanFirstWord(std::string const &line)
{
    return find_if(execution::par_unseq, line.begin(), line.end(), [](char ch)
	{
	    return " \t\r\n\f\v"s.find(ch) != string::npos;
	});
}

bool executeCommand(Environment &env, Connection &conn, string const &inputLine)
try
{
    if (inputLine.empty() || *inputLine.begin() == '#')
	return true;

    if (inputLine == ".quit"s || inputLine == ".exit" || inputLine == ".close" || inputLine == "\x04"s)
	return false;
	
    if (inputLine == ".drivers"s)
	for (auto const &[description, attributes] : env.drivers())
	{
	    cout << description.data() << ":\n";
	    auto it = max_element(execution::par_unseq, attributes.begin(), attributes.end(), [](auto const &elem, auto const &other)
		{
		    return elem.first.size() < other.first.size();
		});

	    auto maxNameWidth = it == attributes.end() ? 0u : it->first.size();
	    
	    for (auto const &attribute: attributes)
		cout << '\t' << setw(maxNameWidth) << left << attribute.first << " => " << attribute.second << '\n';

	    cout << '\n';
	}
    else
    {
	auto it = ScanFirstWord(inputLine);

	if (string(inputLine.begin(), it) == ".browseConnect")
	{
	    while (it != inputLine.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
		it++;

	    if (it != inputLine.end())
	    {
		auto attributes = conn.browseConnect(Environment::splitAttributes(string(it, inputLine.end()), ';'));

		for (auto const &attribute: attributes)
		    cout << '\t' << attribute.first << '=' << attribute.second << '\n';

		cout << '\n';
	    }
	    else
		clog << "Missing connection string\n";
	}
	else
	    clog << "No such command: " << inputLine << '\n';
    }

    return true;
}
catch (exception const &ex)
{
    clog << "Error: " << ex.what() << '\n';

    return true;
}

void trimWs(string &inputLine)
{
    auto it = inputLine.begin();

    while (it != inputLine.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    if (it != inputLine.begin())
	inputLine.erase(inputLine.begin(), it);

    auto jt = inputLine.rbegin();

    while (jt != inputLine.rend() && " \t\r\n\f\v"s.find(*jt) != string::npos)
	jt++;

    if (jt != inputLine.rbegin())
	inputLine.erase(jt.base(), inputLine.end());
}

int main(int argc, char const *argv[])
try
{
    Environment env;
    Connection conn(env);
    string inputLine;

    while (cin.good())
    {
	cout << "ODBC:>";
	getline(cin, inputLine);

	trimWs(inputLine);

	if (!executeCommand(env, conn, inputLine))
	    break;
    }

    if (cin.bad() || cin.fail() && !cin.eof())
	    return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
catch (exception const &ex)
{
    clog << "Application error: " << ex.what() << '\n';
    return EXIT_FAILURE;
}
catch (...)
{
    clog << "Application error!\nTerminated!\n";
    return EXIT_FAILURE;
}
