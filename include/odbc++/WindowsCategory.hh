#include <string>
#include <system_error>

namespace odbc
{
    class WindowsCategory: public std::error_category
    {
    public:
	virtual char const *name() const noexcept;
	virtual std::error_condition default_error_condition(int ev) const noexcept;
	virtual bool equivalent(int code, std::error_condition const &condition) const noexcept;
	virtual bool equivalent(std::error_code const &code, int condition) const noexcept;
	virtual std::string message(int ev) const;
    }
	const &windows_category();
}

inline char const *odbc::WindowsCategory::name() const noexcept
{
    return "Windows";
}
