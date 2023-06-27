#if !defined(SQL_ODBCXX_ENVIRONMENT)
#define SQL_ODBCXX_ENVIRONMENT

#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>
#include <sql.h>

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <execution>

#include "Exports.h"
#include "Handle.hh"

namespace odbc3_0
{
    class ODBCXX_EXPORT Environment: protected Handle
    {
    protected:
	friend class Connection;
	void FetchDriver(SQLUSMALLINT direction, std::pair<sqlstring, sqlstring> &driverInfo);

    public:
	Environment(unsigned long ver = SQL_OV_ODBC3_80);
	std::map<std::string, std::map<std::string, std::string>> drivers();

	SQLHENV nativeHandle() const;

	template <typename StringT, typename CharT>
	    static std::map<std::string, std::string> splitAttributes(StringT const &inputLine, CharT separator = ';');
    };
}

inline odbc3_0::Environment::Environment(unsigned long ver)
    : Handle(SQL_HANDLE_ENV, SQL_NULL_HANDLE)
{
    SQLSetEnvAttr(sqlHandle, SQL_ATTR_ODBC_VERSION, (void *)uintptr_t { ver }, -1);
}

inline SQLHENV odbc3_0::Environment::nativeHandle() const
{
    return sqlHandle;
}

template<typename StringT, typename CharT>
    inline std::map<std::string, std::string> odbc3_0::Environment::splitAttributes(StringT const &attributes, CharT separator)
{
    using std::string;
    auto it = attributes.begin();
    auto jt = std::find(std::execution::par_unseq, it, attributes.end(), separator);

    std::map<string, string> attributesMap;

    while (it != jt)
    {
	auto kt = std::find(std::execution::par_unseq, it, jt, '=');

	if (kt != jt)
	    attributesMap[string(it, kt)] = string(kt + 1, jt);
	else
	    attributesMap[string(it, jt)] = string();

	if (jt != attributes.end())
	{
	    it = ++jt;
	    jt = std::find(std::execution::par_unseq, it, attributes.end(), separator);
	}
	else
	    it = jt;
    }

    return attributesMap;
}


#endif	    // !defined(SQL_ODBCXX_ENVIRONMENT)
