#if defined(_WINDOWS) && defined(_M_AMD64) && !defined(_AMD64_)
# define _AMD64_
#endif

#include <windef.h>

#include <stdexcept>
#include <utility>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

namespace odbc3_0
{
	class Handle
	{
	protected:
		SQLSMALLINT handleType;
		SQLHANDLE sqlHandle = SQL_NULL_HANDLE;
		Handle(SQLSMALLINT handleType, SQLHANDLE inputHandle);

	public:
		Handle(Handle const &other) = delete;
		Handle(Handle &&other);
		Handle(SQLSMALLINT handleType, Handle const &inputHandle);
		~Handle();

		Handle &operator =(Handle const &other) = delete;
		Handle &operator =(Handle &&other);
	};
}

inline odbc3_0::Handle::Handle(SQLSMALLINT handleType, SQLHANDLE inputHandle)
	: handleType(handleType)
{
	switch (SQLAllocHandle(handleType, inputHandle, &sqlHandle))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return;

	case SQL_INVALID_HANDLE:
		throw std::runtime_error("Invalid handle");

	default:
		throw std::runtime_error("SQL Error");
	}
}

inline odbc3_0::Handle::Handle(SQLSMALLINT handleType, Handle const &inputHandle)
	: Handle(handleType, inputHandle.sqlHandle)
{
}

inline odbc3_0::Handle::Handle(Handle &&other)
	: handleType(other.handleType), sqlHandle(std::exchange(other.sqlHandle, SQLHANDLE { SQL_NULL_HANDLE }))
{
}

inline odbc3_0::Handle &odbc3_0::Handle::operator =(Handle &&other)
{
	handleType = other.handleType;
	sqlHandle = other.sqlHandle;

	other.sqlHandle = SQL_NULL_HANDLE;

	return *this;
}

inline odbc3_0::Handle::~Handle()
{
	if (sqlHandle != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(handleType, sqlHandle);
		sqlHandle = SQL_NULL_HANDLE;
	}
}
