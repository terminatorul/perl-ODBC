#if !defined(DBCMD_COMMAND_BROWSE_CONNECT_HH)
#define DBCMD_COMMAND_BROWSE_CONNECT_HH

#include <memory>
#include "HandlerFunctor.hh"
#include "CommandHandler.hh"

class BrowseConnect: public CommandHandler
{
public:
    class Functor: public HandlerFunctor
    {
    public:
	using HandlerFunctor::HandlerFunctor;
	virtual void operator ()(string const &command, string::const_iterator it) override;
    };

    virtual set<string> const &commandNames() const override;
    virtual unique_ptr<HandlerFunctor> handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog) override;
};

std::unique_ptr<HandlerFunctor> BrowseConnect::handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog)
{
    return std::make_unique<Functor>(*this, context, cin, cout, cerr, clog);
}

#endif	    // !defined(DBCMD_COMMAND_BROWSE_CONNECT_HH)
