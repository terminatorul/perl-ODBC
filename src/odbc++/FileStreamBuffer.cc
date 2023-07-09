#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <fileapi.h>

#include "odbc++/FileStreamBuffer.hh"

// template class odbc::FileStreamBuffer<char>;
