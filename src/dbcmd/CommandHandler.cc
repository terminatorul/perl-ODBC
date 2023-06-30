#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <utility>

#include "CommandHandler.hh"

using std::exchange;

CommandHandler *CommandHandler::first { };

CommandHandler::CommandHandler()
    : next(exchange(first, this))
{
}

CommandHandler::~CommandHandler()
{
    CommandHandler **command = &first;

    while (*command && *command != this)
	command = &(*command)->next;

    if (*command == this)
	*command = this->next;

    this->next = nullptr;
}

HandlerFunctor *CommandHandler::mainFunctor(Context &context, istream &cin, ostream &cout, ostream &cerr, ostream &clog)
{
    if (!functor)
	functor = handlerFunctor(context, cin, cout, cerr, clog);

    return functor.get();
}
