#if !defined(DBCMD_CONTEXT_HH)
#define DBCMD_CONTEXT_HH

#include <string>
#include <vector>
#include <map>

#include <odbc++/Environment.hh>
#include <odbc++/Connection.hh>

class Context
{
public:
    odbc::Environment env;
    std::vector<odbc::Connection> connections;
    unsigned connectionIndex = 0u;
    odbc::Connection *conn = nullptr;

    std::map<std::string, std::map<std::string, std::string>>
	drivers;

    void appendNewConnection();
    void deleteConnection(unsigned connectionIndex);

    Context(unsigned long ver = SQL_OV_ODBC3_80);
};

inline Context::Context(unsigned long ver)
    : env(ver)
{
    appendNewConnection();
}

inline void Context::appendNewConnection()
{
    connections.emplace_back(env);
    connectionIndex = connections.size() - 1u;
    conn = &connections[connectionIndex];
}

inline void Context::deleteConnection(unsigned connectionIndex)
{
    if (connectionIndex < connections.size())
	connections.erase(connections.begin() + connectionIndex);
}

#endif
