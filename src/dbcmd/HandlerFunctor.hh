#if !defined(DBCMD_HANDLER_FUNCTOR_HH)
#define DBCMD_HANDLER_FUNCTOR_HH

#include <string>
#include <iostream>

#include "Context.hh"

class CommandHandler;

class HandlerFunctor
{
public:
    using string = std::string;
    using istream = std::istream;
    using ostream = std::ostream;

protected:
    CommandHandler *handler;
    odbc3_0::Environment &env;
    std::vector<odbc3_0::Connection> &connections;
    unsigned &connectionIndex;
    odbc3_0::Connection *&conn;

    istream &cin;
    ostream &cout, &cerr, &clog;

public:
    HandlerFunctor(CommandHandler &handler, Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog);
    virtual ~HandlerFunctor();

    virtual CommandHandler *commandHandler();
    virtual void operator ()(string const &command, string::const_iterator it) = 0;
};

inline HandlerFunctor::HandlerFunctor(CommandHandler &commandHandler, Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog):
    handler(&commandHandler),
    env(context.env),
    connections(context.connections),
    connectionIndex(context.connectionIndex),
    conn(context.conn),
    cin(cin),
    cout(cout),
    cerr(cerr),
    clog(clog)
{
}

inline HandlerFunctor::~HandlerFunctor()
{
}

inline CommandHandler *HandlerFunctor::commandHandler()
{
    return handler;
}

#endif
