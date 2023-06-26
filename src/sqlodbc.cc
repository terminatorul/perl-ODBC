#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <cstdlib>
#include <string>
#include <exception>
#include <iostream>
#include <iomanip>

#include "odbc++/Environment.hh"
#include "odbc++/Connection.hh"

using std::string;
using namespace std::literals::string_literals;
using std::exception;
using std::getline;
using std::clog;
using std::cin;
using std::cout;

namespace odbc = odbc3_0;
using odbc::Environment;

bool executeCommand(Environment &env, string const &inputLine)
try
{
    if (inputLine.empty() || *inputLine.begin() == '#')
	return true;

    if (inputLine == ".quit"s || inputLine == ".exit" || inputLine == ".close" || inputLine == "\x04"s)
	return false;
	
    if (inputLine == ".drivers"s)
	for (auto const &[description, attributes] : env.drivers())
	{
	    cout << description.data() << ": " << attributes.data() << '\n';
	}
    else
	clog << "No such command: " << inputLine << '\n';

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
    string inputLine;

    while (cin.good())
    {
	cout << "ODBC:>";
	getline(cin, inputLine);

	trimWs(inputLine);

	if (!executeCommand(env, inputLine))
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
