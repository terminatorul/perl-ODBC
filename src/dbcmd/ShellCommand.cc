#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <cstdlib>
#include <string>
#include <set>
#include <algorithm>
#include <exception>

#include "ShellCommand.hh"

using std::system;
using std::getenv;
using std::string;
using std::string_view;
using std::set;
using std::find;
using std::runtime_error;
using namespace std::literals::string_literals;

set<string> const &ShellCommand::commandNames() const
{
    static set<string> const names { ".shell", ".sh", ".bash", ".dash", ".zsh", ".fish", ".cmd", ".powershell", ".PowerShell", ".pwsh" };

    return names;
}

string const &ShellCommand::helpSubject() const
{
    static string subjectLine = "\t.shell					Run given command in a shell or start interactive shell session"s;

    return subjectLine;
}

string const &ShellCommand::helpText() const
{
    static string textLines =
	"\t!<Shell-Command args...>    Run given command in the shell\n"
	"\t.shell <cmd args...>\n"
	"\t!                           Run interactive shell\n"
	"\t.shell\n"
	"\n"
	"Or select a named shell to run:\n"
	"\t.cmd         [cmd args...]\n"
	"\t.pwsh        [cmd args...]\n"
	"\t.powershell  [cmd args...]\n"
	"\t.PowerShell  [cmd args...]\n"
	"\t.bash        [cmd args...]\n"
	"\t.dash        [cmd args...]\n"
	"\t.sh          [cmd args...]\n"
	"\t.fish        [cmd args...]\n"
	"\t.zsh         [cmd args...]\n"s;

	return textLines;
}


enum class ShellType { Native, Cmd, Shell, PowerShell };

static string makeShellCommand(ShellType shellType, string const &shell, string cmdArgs)
{
    switch (shellType)
    {
    case ShellType::Native:
	return cmdArgs;

    case ShellType::Cmd:
	return shell + " /C \"" + cmdArgs + '\"';

    case ShellType::Shell:
    {
	auto it = cmdArgs.begin();

	while (it != cmdArgs.end())
	{
	    it = find(it, cmdArgs.end(), '\'');

	    if (it != cmdArgs.end())
	    {
		it++;
		it = cmdArgs.insert(it, '\'');
	    }
	}

	return shell + " -c '" + cmdArgs + '\'';
    }

    case ShellType::PowerShell:
	return shell + " -Command " + cmdArgs;

    default:
	throw runtime_error("Unknown shell.");
    }
}

static ShellType selectShellCommand(string &shellCmd, string &shell, bool &nativeShell)
{
    ShellType shellType = ShellType::Native;

    if (shellCmd == ".shell"s)
    {
#if defined(_WINDOWS)
	shellCmd = ".cmd"s;
#else
	shellCmd = ".sh"s;
#endif
    }

    if (shellCmd == ".cmd"s)
    {
#if defined(_WINDOWS)
	nativeShell = true;
#endif

	char const *comSpec = getenv("ComSpec");

	if (comSpec && *comSpec)
	    shell = comSpec;
	else
	{
#if defined(_WINDOWS)
	    throw runtime_error("Environment variable COMSPEC is needed to find the shell."s);
#else
	    shell = "cmd.exe"s;
#endif
	}

	shellType = ShellType::Cmd;
    }
    else
	if (shellCmd == ".sh"s)
	{
#if !defined(_WINDOWS)
	    nativeShell = true;
#endif

	    char const *sh = getenv("SHELL");

	    if (sh && *sh)
		shell = sh;
	    else
	    {
#if !defined(_WINDOWS)
		throw runtime_error("Environment variable SHELL is needed to find the shell."s);
#else
		shell = "sh"s;
#endif
	    }

	    shellType = ShellType::Shell;
	}
	else
	    if (shellCmd == ".powershell"s || shellCmd == ".PowerShell"s || shellCmd == ".pwsh"s)
	    {
		shell = "PowerShell -ExecutionPolicy Unrestricted";
		shellType = ShellType::PowerShell;
	    }
	    else
		if (shellCmd == ".bash"s || shellCmd == ".dash" || shellCmd == ".zsh"s || shellCmd == ".fish"s)
		{
		    shell = string(++shellCmd.cbegin(), shellCmd.cend());
		    shellType = ShellType::Shell;
		}

    return shellType;
}

void ShellCommand::Functor::operator ()(string const &command, string::const_iterator it)
{
    string shellCmd { command.begin(), it };

    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    string shell;
    string cmdArg { it, command.end() };

    bool nativeShell = false;

    ShellType shellType = selectShellCommand(shellCmd, shell, nativeShell);

    if (cmdArg.empty())
    {
	if (!shell.empty())
	    system(shell.c_str());
    }
    else
    {
	if (nativeShell)
	    system(cmdArg.c_str());
	else
	    system(makeShellCommand(shellType, shell, cmdArg).c_str());
    }

    cout << '\n';
}

ShellCommand shellCommand;
