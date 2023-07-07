#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <algorithm>
#include <execution>
#include <stdexcept>

#include "EnvCommand.hh"

using std::tuple;
using std::set;
using std::map;
using std::vector;
using std::string;
using std::string_view;
using std::find;
using std::reduce;
using std::runtime_error;
using std::ostream;
using std::getenv;
using namespace std::literals::string_literals;
namespace execution = std::execution;

#if defined(_WINDOWS)
constexpr char const pathListDelimiter = ';';
#else
constexpr char const pathListDelimiter = ':';
#endif

set<string> const &EnvCommand::commandNames() const
{
    static set<string> const names { ".env", ".pathEnv", ".unsetEnv" };

    return names;
}

string const &EnvCommand::helpSubject() const
{
    static string subjectLine = "\t.env					Display or update environment variables";

    return subjectLine;
}
string const &EnvCommand::helpText() const
{
    static string textLines =
	"\t.env                                     List environment variables and their values\n"
	"\t.env      <variable_name [= value]>      Display or set an environment variable\n"
	"\t.unsetEnv <variable_name>                Remove an environment variable\n"
	"\t.pathEnv  <variable_name>                Show a path list variable like PATH, one path per line\n"
	"\t.pathEnv  <variable_name> -= <value>     Update a path variable to remove a single path from list\n"
	"\t.pathEnv  <variable_name> += <value>     Update a path variable to add a single path to the list\n"s;

    return textLines;
}

static tuple<string, string, string> parseEnvCommand(string const &command, string::const_iterator it)
{
    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    string_view varName, applyOp, newValue;

    if (it != command.end())
    {
	auto jt = find(execution::par_unseq, it, command.cend(), '=');

	if (jt != command.end())
	{
	    switch (*(jt - 1))
	    {
	    case '-':
	    case '+':
		jt--;
		applyOp = string_view { jt, jt + 2 };
		break;

	    default:
		applyOp = string_view { jt, jt + 1 };
		break;
	    }
	}

	if (jt != it)
	{
	    auto kt = jt - 1u;

	    while (" \t\r\n\f\v"s.find(*kt) != string::npos)
		kt--;

	    varName = string_view { it, ++kt };
	}

	if (jt != command.end())
	{
	    jt += applyOp.size();

	    while (jt != command.end() && " \t\r\n\f\v"s.find(*jt) != string::npos)
		jt++;

	    newValue = string_view { jt, command.end() };
	}
    }

    return tuple { string { varName }, string { applyOp }, string { newValue } };
}

static void removeEnvVar(string const &varName)
{
#if defined(_WINDOWS)
    string envLine = varName + "="s;
    _putenv(envLine.c_str());
//     if (BOOL fVarUpdated = SetEnvironmentVariable(varName.c_str(), NULL); !fVarUpdated)
// 	throw runtime_error("Environment variable update failed with WinAPI error "s + std::to_string(::GetLastError()));
#else
    unsetenv(varName.c_str());
#endif
}

static void setEnvVar(string const &varName, string const &newValue)
{
    string envLine = varName + "="s + newValue;
#if defined(_WINDOWS)
    _putenv(envLine.c_str());
//     if (BOOL fVarUpdated = SetEnvironmentVariable(varName.c_str(), newValue.c_str()); !fVarUpdated)
// 	throw runtime_error("Environment variable update failed with WinAPI error "s + std::to_string(::GetLastError()));
#else
    setenv(varName.c_str(), newValue.c_str());
#endif
}

static void showEnvironment(ostream &cout)
{
    char **var = environ;

    map<string, string> variables;

    while (*var)
    {
	char *p = *var;

	while (*p && *p != '=')
	    p++;

	if (*p)
	    variables.emplace(string { *var, p - 1 }, string(++p));
	else
	    variables.emplace(string { *var, p - 1 }, string());

	var++;
    }

    for (auto const &envPair : variables)
	cout << '\t' << envPair.first << '=' << envPair.second << '\n';

    cout << '\n';
}

static void showEnvironment(string const &varName, ostream &cout)
{
    char const *env = std::getenv(varName.c_str());

    if (!env)
	cout << '\n';
    else
        cout <<  env << "\n\n";
}

static vector<string> splitNativePathList(char const *pathList)
{
    vector<string> list;

    if (!pathList)
	pathList = "";

    while (*pathList)
    {
	while (*pathList == pathListDelimiter)
	    pathList++;

	if (*pathList)
	{
	    char const *p = pathList;

	    while (*p && *p != pathListDelimiter)
		p++;

	    list.emplace_back(pathList, p);

	    pathList = p;
	}
    }

    return list;
}

static void showPathEnvironment(vector<string> const &pathList, ostream &cout)
{
    for (auto const &path: pathList)
	cout << '\t' << path << '\n';

    cout << '\n';
}

static void adjustPathEnvironment(string const &varName, string const &applyOp, string const &newValue)
{
    vector<string> pathList = splitNativePathList(getenv(varName.c_str()));

    auto it = find(execution::par_unseq, pathList.begin(), pathList.end(), newValue);

    if (it != pathList.end() && applyOp == "-="s)
    {
	pathList.erase(it);
	string value = reduce(execution::seq, pathList.begin(), pathList.end(), string(), [](auto const &leftStr, auto const &rightStr) -> string
	    {
		return leftStr.empty() ? rightStr : leftStr + pathListDelimiter + rightStr;
	    });

	setEnvVar(varName, value);
    }
    else
	if (it == pathList.end() && applyOp == "+="s)
	{
	    pathList.push_back(newValue);
	    string value = reduce(execution::seq, pathList.begin(), pathList.end(), string(), [](auto const &leftStr, auto const &rightStr) -> string
		{
		    return leftStr.empty() ? rightStr : leftStr + pathListDelimiter + rightStr;
		});

	    setEnvVar(varName, value);
	}
}

void EnvCommand::Functor::operator ()(string const &command, string::const_iterator it)
{
    string_view cmd { command.cbegin(), it };

    auto [varName, applyOp, newValue] = parseEnvCommand(command, it);

    if (varName.empty() && applyOp.size())
	throw runtime_error("Environment variable name expected before assignament operator.");

    if (cmd == ".unsetEnv"s)
	if (applyOp.size())
	    throw runtime_error("Unexpected assignament operator for .unsetEnv command.");
	else
	{
	    removeEnvVar(varName);
	    cout << '\n';
	}
    else
	if (cmd == ".env")
	    if (applyOp.empty())
		if (varName.empty())
		    showEnvironment(cout);
		else
		    showEnvironment(varName, cout);
	    else
		if (applyOp != "="s)
		    throw runtime_error("Unexpected compund assignment.");
		else
		{
		    setEnvVar(varName, newValue);
		    cout << '\n';
		}
	else
	    if (cmd == ".pathEnv"s)
		if (varName.empty())
		    showPathEnvironment(splitNativePathList(getenv("PATH")), cout);
		else
		    if (applyOp.empty())
			showPathEnvironment(splitNativePathList(getenv(varName.c_str())), cout);
		    else
			if (applyOp == "="s)
			    throw runtime_error("Expected compound assignment operator or no operator.");
			else
			    {
				adjustPathEnvironment(varName, applyOp, newValue);
				cout << '\n';
			    }
}

EnvCommand envCommand;
