#if !defined(SQL_ODBCXX_CONNECTION)
#define SQL_ODBCXX_CONNECTION

#include "Exports.h"
#include "Handle.hh"
#include "Environment.hh"

#include <map>
#include <string>

namespace odbc3_0
{
    class ODBCXX_EXPORT Connection: protected Handle
    {
    protected:
	enum State
	{
	    Disconnected, InProgress, Connected
	}
	    state = State::Disconnected;

	std::map<std::string, std::string> browseConnect(SQLCHAR *request, SQLSMALLINT requestSize);

    public:
	Connection(Environment &env);
	Connection(Connection const &other) = delete;
	Connection(Connection &&other) = default;
	~Connection();

	SQLHDBC nativeHandle() const;

	Connection &operator =(Connection const &other) = delete;
	Connection &operator =(Connection &&other) = default;

	std::map<std::string, std::string> browseConnect(std::map<std::string, std::string> const &request);
	void disconnect();
    };
}

inline odbc3_0::Connection::Connection(Environment &env)
    : Handle(SQL_HANDLE_DBC, env)
{
}

inline odbc3_0::Connection::~Connection()
{
    disconnect();
}

inline SQLHDBC odbc3_0::Connection::nativeHandle() const
{
    return sqlHandle;
}

#endif	    // !defined(SQL_ODBCXX_CONNECTION)
