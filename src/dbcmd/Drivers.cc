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
namespace execution = std::execution;

using odbc3_0::Environment;
using odbc3_0::Connection;

set<string> const &Drivers::commandNames() const
{
    static set<string> names { ".drivers" };

    return names;
}

void Drivers::Functor::operator ()(string const &command, string::const_iterator it)
{
    for (auto const &[description, attributes]: env.drivers())
    {
	cout << description.data() << ":\n";

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

static Drivers driversCmd;
