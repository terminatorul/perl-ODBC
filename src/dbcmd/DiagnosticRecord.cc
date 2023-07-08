#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <set>
#include <string>
#include <stdexcept>

#include <odbc++/SQLDiagnosticException.hh>

#include "DiagnosticRecord.hh"

using std::set;
using std::string;
using std::string_view;
using std::runtime_error;
using namespace std::literals::string_literals;

using odbc::SQLDiagnosticException;

set<string> const &DiagnosticRecord::commandNames() const
{
    static set<string> nameList { ".diagnosticRecord"s, ".diag"s };

    return nameList;
}

string const &DiagnosticRecord::helpSubject() const
{
    static string const subjectLine = "\t.diagnosticRecord			Outputs the last SQL diagnostic messages"s;

    return subjectLine;
}

string const &DiagnosticRecord::helpText() const
{
    static string const textLines =
	"\t.diagnosticRecord   [--connection | --environment ]    Outputs diagnostic messages from the last SQL command.\n"
	"\t.diag               [--conn | --env ]\n"s;

    return textLines;
}

void DiagnosticRecord::Functor::operator ()(string const &command, string::const_iterator it)
{
    string_view cmd { command.cbegin(), it };

    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    string subjectHandle { it, command.end() };

    static set<string> optionNames { "--connection"s, "--conn"s, "--environment"s, "--env"s, "--statement"s, "--stmt"s };

    if (!subjectHandle.empty() && optionNames.find(subjectHandle) == optionNames.end())
	throw runtime_error("Option not supported.");

    if (subjectHandle == "--statement"s || subjectHandle == "--stmt"s)
	throw runtime_error("Not implemented.");

    if (subjectHandle.empty() || subjectHandle == "--environment"s || subjectHandle == "--env"s)
    {
	SQLDiagnosticException exc(env.diagnosticRecords());

	if (exc.recordCount())
	    cout << exc.what() << '\n';
    }

    if (subjectHandle.empty() || subjectHandle == "--connection"s || subjectHandle == "--conn"s)
    {
	SQLDiagnosticException exc(conn->diagnosticRecords());

	if (exc.recordCount())
	    cout << exc.what() << '\n';
    }

    cout << '\n';
}

DiagnosticRecord diagnosticRecord;
