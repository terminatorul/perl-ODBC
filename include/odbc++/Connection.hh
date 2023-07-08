#if !defined(SQL_ODBCXX_CONNECTION)
#define SQL_ODBCXX_CONNECTION

#include "Exports.h"
#include "Handle.hh"
#include "Environment.hh"

#include <span>
#include <map>
#include <string>

namespace odbc
{
    class ODBCXX_EXPORT Connection: protected Handle
    {
    public:
	enum State
	{
	    Disconnected, InProgress, Connected
	};
    protected:
	State state = State::Disconnected;

	enum class BrowseConnectResult
	{
	    Connected,
	    Again,
	    More
	}
	    nativeBrowseConnect(std::span<SQLCHAR> request, sqlstring &result);

	std::map<std::string, std::string> browseConnect(std::span<SQLCHAR> request);

    public:
	Connection(Environment &env);
	Connection(Connection const &other) = delete;
	Connection(Connection &&other) = default;
	~Connection();

	SQLHDBC nativeHandle() const;
	using Handle::diagnosticRecord;
	using Handle::diagnosticRecords;

	Connection &operator =(Connection const &other) = delete;
	Connection &operator =(Connection &&other) = default;

	std::map<std::string, std::string> browseConnect(std::map<std::string, std::string> const &request);
	bool disconnect();
	State connected() const noexcept;
    };
}

inline odbc::Connection::Connection(Environment &env)
    : Handle(SQL_HANDLE_DBC, env)
{
}

inline odbc::Connection::~Connection()
{
    disconnect();
}

inline SQLHDBC odbc::Connection::nativeHandle() const
{
    return sqlHandle;
}

#endif	    // !defined(SQL_ODBCXX_CONNECTION)
