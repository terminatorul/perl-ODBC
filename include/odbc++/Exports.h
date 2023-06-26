
#if defined(odbc___EXPORTS)
# define ODBCXX_EXPORT __declspec(dllexport)
#else
# define ODBCXX_EXPORT // __declspec(dllimport)
#endif
