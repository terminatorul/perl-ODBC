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
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <execution>

#include "odbc++/Environment.hh"
#include "odbc++/Connection.hh"
#include "Drivers.hh"

using std::set;
using std::string;
using std::cout;
using std::setw;
using std::left;
using std::max_element;
using namespace std::literals::string_literals;
namespace execution = std::execution;

using odbc::Environment;
using odbc::Connection;

set<string> const &Drivers::commandNames() const
{
    static set<string> names { ".drivers" };

    return names;
}

void Drivers::Functor::operator ()(string const &command, string::const_iterator it)
{
    while (it != command.end() && " \t\r\n\f\v"s.find(*it) != string::npos)
	it++;

    string cmdArg { it, command.end() };
    bool brief = false;

    if (cmdArg == "--brief"s)
	brief = true;
    else
	if (cmdArg.size())
	{
	    clog << "Option not supported: " << cmdArg << "\n\n";
	    return;
	}

    for (auto const &[description, attributes]: env.drivers())
    {
	cout << description.data() << (brief ? "" : ":") << '\n';

	if (!brief)
	{
	    auto it = max_element(execution::par_unseq, attributes.begin(), attributes.end(), [](auto const &elem, auto const &other)
		{
		    return elem.first.size() < other.first.size();
		});

	    auto maxNameWidth = it == attributes.end() ? 0u : it->first.size();

	    for (auto const &attribute : attributes)
		cout << '\t' << setw(maxNameWidth) << left << attribute.first << " => " << attribute.second << '\n';

	    cout << '\n';
	}
    }

    if (brief)
	cout << '\n';
}

static Drivers driversCmd;
