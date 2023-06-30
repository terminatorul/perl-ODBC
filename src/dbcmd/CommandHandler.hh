#if !defined(DBCMD_COMMAND_HANDLER_HH)
#define DBCMD_COMMAND_HANDLER_HH

#include <memory>
#include <set>
#include <string>
#include <iostream>

#include "odbc++/Environment.hh"
#include "odbc++/Connection.hh"
#include "Context.hh"
#include "HandlerFunctor.hh"

class CommandHandler
{
protected:
    std::unique_ptr<HandlerFunctor> functor;

public:
    template <typename ElemT>
	using set = std::set<ElemT>;

    template <typename ObjectT, typename DeleterT = std::default_delete<ObjectT>>
	using unique_ptr = std::unique_ptr<ObjectT, DeleterT>;

    using string = std::string;

    static CommandHandler *first;

    CommandHandler *next;

    CommandHandler();
    virtual ~CommandHandler();

    using istream = std::istream;
    using ostream = std::ostream;

    virtual set<string> const &commandNames() const = 0;
    virtual unique_ptr<HandlerFunctor> handlerFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog) = 0;
    virtual HandlerFunctor *mainFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog);
};

#endif	    // !defined(DBCMD_COMMAND_HANDLER_HH)
