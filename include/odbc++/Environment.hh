#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>
#include <sql.h>

#include <vector>
#include <utility>

#include "Handle.hh"

namespace odbc3_0
{
	class Environment: protected Handle
	{
	public:
		using sqlstring = std::vector<SQLCHAR>;

	protected:
		std::pair<sqlstring, sqlstring> FetchDriver(SQLUSMALLINT direction);

	public:
		Environment(unsigned long ver = SQL_OV_ODBC3_80);
		std::vector<std::pair<sqlstring, sqlstring>> drivers();
	};
}

inline odbc3_0::Environment::Environment(unsigned long ver)
	: Handle(SQL_HANDLE_ENV, SQL_NULL_HANDLE)
{
	SQLSetEnvAttr(sqlHandle, SQL_ATTR_ODBC_VERSION, (void *)uintptr_t { ver }, -1);
}
