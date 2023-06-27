#if !defined(SQL_ODBCXX_EXPORTS)
#define SQL_ODBCXX_EXPORTS

#if defined(odbc___EXPORTS)
# define ODBCXX_EXPORT __declspec(dllexport)
#else
# define ODBCXX_EXPORT // __declspec(dllimport)
#endif

#endif	    // !defined(SQL_ODBCXX_EXPORTS)
