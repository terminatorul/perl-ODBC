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
#include <filesystem>
#include <stdexcept>

#include "ChDirCommand.hh"

using std::set;
using std::string;
using std::string_view;
using std::runtime_error;
namespace filesystem = std::filesystem;
using namespace std::literals::string_literals;

set<string> const &ChDirCommand::commandNames() const
{
    static set<string> const names { ".cd", ".chdir", ".cwd", ".pwd" };

    return names;
}

string const &ChDirCommand::helpSubject() const
{
    static string subjectLine = "\t.chdir					Display or change current directory."s;

    return subjectLine;
}

string const &ChDirCommand::helpText() const
{
    static string textLines =
	"\t.chdir <directory_name>         Change current directory.\n"
	"\t.cd    <directory_name>\n"
	"\t.cwd                            Display current directory\n"
	"\t.pwd\n"s;

    return textLines;
}

void ChDirCommand::Functor::operator ()(string const &command, string::const_iterator it)
{
    string_view cmd { command.cbegin(), it };

    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    string_view dirname { it, command.end() };

    if (cmd == ".cd"s || cmd == ".chdir"s)
	if (dirname.empty())
	    throw runtime_error("Specify directory name.");
	else
	    filesystem::current_path(filesystem::path(dirname));
    else
	if (cmd == ".cwd"s || cmd == ".pwd"s)
	    if (dirname.empty())
		cout << filesystem::current_path().string() << "\n\n";
	    else
		throw runtime_error("Command takes no arguments.");
}

ChDirCommand chdirCommand;
