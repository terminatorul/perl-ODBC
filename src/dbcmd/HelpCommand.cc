#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "HelpCommand.hh"

using std::set;
using std::map;
using std::string;
using std::string_view;
using std::for_each;
using namespace std::literals::string_literals;

set<string> const &HelpCommand::commandNames() const
{
    static set<string> nameList { ".help" };

    return nameList;
}

string const &HelpCommand::helpSubject() const
{
    static string subjectLine = "\t.help <.command-name>			Show help for a command"s;

    return subjectLine;
}

string const &HelpCommand::helpText() const
{
    static string textLines =
	"\t.help			    Show summary of commands\n"
	"\t.help <.command-name>	    Show help for specified command\n"s;

    return textLines;
}

static set<string> helpSummary;
static map<string, CommandHandler *> helpCommandHandlers;

void HelpCommand::Functor::operator ()(string const &command, string::const_iterator it)
{
    string commandName;

    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    if (it != command.end())
	commandName = string { it, command.end() };

    if (helpSummary.empty())
    {
	for (string const &subjectLine : { "\t.exit					Exit DB Cmd", "\t.disconnect				Disconnect the active ODBC connection." })
	    helpSummary.insert(subjectLine);

	for (CommandHandler *handler = CommandHandler::first; handler; handler = handler->next)
	{
	    helpSummary.insert(handler->helpSubject());

	    for (auto const &commandName : handler->commandNames())
		helpCommandHandlers.emplace(commandName, handler);
	}
    }

    if (commandName.empty())
    {
	for_each(helpSummary.begin(), helpSummary.end(), [this](auto const &helpLine) -> void
	    {
		clog << helpLine << '\n';
	    });
    }
    else
    {
	auto it = helpCommandHandlers.find(commandName);

	if (it == helpCommandHandlers.end())
	    clog << "No such command\n";
	else
	    clog << it->second->helpText();
    }

    clog << '\n';
};

HelpCommand helpCommand;
