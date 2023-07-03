#if !defined(DBCMD_CH_DIR_HH)
#define DBCMD_CH_DIR_HH

#include <memory>
#include "HandlerFunctor.hh"
#include "CommandHandler.hh"

class ChDirCommand: public CommandHandler
{
protected:
    class Functor: public HandlerFunctor
    {
    public:
	using HandlerFunctor::HandlerFunctor;
	virtual void operator ()(string const &command, string::const_iterator it) override;
    };

    virtual set<string> const &commandNames() const override;
    virtual unique_ptr<HandlerFunctor> handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog) override;
};

std::unique_ptr<HandlerFunctor> ChDirCommand::handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog)
{
    return std::make_unique<Functor>(*this, context, cin, cout, cerr, clog);
}

#endif	    // !defined(DBCMD_CH_DIR_HH)
