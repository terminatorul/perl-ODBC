#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>
#include <sql.h>

#include <vector>
#include <map>
#include <utility>

#include "odbc++/Exports.h"
#include "odbc++/Handle.hh"

namespace odbc3_0
{
    class ODBCXX_EXPORT Environment: protected Handle
    {
    public:
        using sqlstring = std::vector<SQLCHAR>;

    protected:
        void FetchDriver(SQLUSMALLINT direction, std::pair<sqlstring, sqlstring> &driverInfo);

    public:
	Environment(unsigned long ver = SQL_OV_ODBC3_80);
	std::map<std::string, std::map<std::string, std::string>> drivers();
    };
}

inline odbc3_0::Environment::Environment(unsigned long ver)
    : Handle(SQL_HANDLE_ENV, SQL_NULL_HANDLE)
{
    SQLSetEnvAttr(sqlHandle, SQL_ATTR_ODBC_VERSION, (void *)uintptr_t { ver }, -1);
}
