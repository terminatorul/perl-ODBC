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

#include "EchoCommand.hh"

using std::set;
using std::string;
using std::string_view;
using namespace std::literals::string_literals;

set<string> const &EchoCommand::commandNames() const
{
    static set<string> commands { ".echo", ".echoerr" };

    return commands;
}

string const &EchoCommand::helpSubject() const
{
    static string subjectLine = "\t.echo Text...				Show text on standard output on standard error"s;

    return subjectLine;
}

string const &EchoCommand::helpText() const
{
    static string textLines =
	"\t.echo Text...            Show text on standard output\n"
	"\t.echoerr Text...         Show text on standard error\n"s;

    return textLines;

}

void EchoCommand::Functor::operator ()(string const &command, string::const_iterator it)
{
    string_view cmd { command.begin(), it };

    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    if (cmd == ".echo"s)
	cout << string_view { it, command.end() } << "\n\n";
    else
	if (cmd == ".echoerr")
	    clog << string_view { it, command.end() } << "\n\n";
}

EchoCommand echoCommand;
