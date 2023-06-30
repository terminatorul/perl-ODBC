#if !defined(DBCMD_COMMAND_DRIVERS_HH)
#define DBCMD_COMMAND_DRIVERS_HH

#include "odbc++/Environment.hh"
#include "odbc++/Connection.hh"
#include "CommandHandler.hh"

class Drivers: public CommandHandler
{
protected:
    class Functor: public HandlerFunctor
    {
    public:
	using HandlerFunctor::HandlerFunctor;
	virtual void operator ()(string const &command, string::const_iterator it) override;
    };

public:
    virtual set<string> const &commandNames() const override;
    virtual unique_ptr<HandlerFunctor> handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog) override;
};

inline std::unique_ptr<HandlerFunctor> Drivers::handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog)
{
    return std::make_unique<Functor>(*this, context, cin, cout, cerr, clog);
}

#endif	    // !defined(DBCMD_COMMAND_DRIVERS_HH)
