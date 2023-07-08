#if defined(_WINDOWS)
# if defined(_M_AMD64) && !defined(_AMD64_)
#   define _AMD64_
# endif
# if defined(_M_IX86) && !defined(_X68_)
#  define _X86_
# endif
#endif

#include <windef.h>
#include <winerror.h>
#include <WinBase.h>
#include <errhandlingapi.h>

#include <cerrno>
#include <string>
#include <system_error>

#include "odbc++/WindowsCategory.hh"

using std::string;
using std::to_string;
using std::error_condition;
using std::generic_category;
using namespace std::literals::string_literals;;

odbc::WindowsCategory const &odbc::windows_category()
{
    static WindowsCategory windowsCategory;

    return windowsCategory;
}

error_condition odbc::WindowsCategory::default_error_condition(int ev) const noexcept
{
    if (ev == ERROR_SUCCESS)
	return error_condition(0, generic_category());

    int error = ERROR_SUCCESS;

    switch (ev)
    {
	case ERROR_ACCESS_DENIED:		error = EACCES;		break;
	case ERROR_ACCOUNT_DISABLED:		error = EACCES;		break;
	case ERROR_ACCOUNT_RESTRICTION:		error = EACCES;		break;
	case ERROR_ACTIVE_CONNECTIONS:		error = EAGAIN;		break;
	case ERROR_ALREADY_ASSIGNED:		error = EBUSY;		break;
	case ERROR_ALREADY_EXISTS:		error = EEXIST;		break;
	case ERROR_ARITHMETIC_OVERFLOW:		error = ERANGE;		break;
	case ERROR_BAD_COMMAND:			error = EIO;		break;
	case ERROR_BAD_DEVICE:			error = ENODEV;		break;
	case ERROR_BAD_DRIVER_LEVEL:		error = ENXIO;		break;
	case ERROR_BAD_EXE_FORMAT:		error = ENOEXEC;	break;
	case ERROR_BAD_FORMAT:			error = ENOEXEC;	break;
	case ERROR_BAD_LENGTH:			error = EINVAL;		break;
	case ERROR_BAD_NETPATH:			error = ENOENT;		break;
	case ERROR_BAD_NET_NAME:		error = ENOENT;		break;
	case ERROR_BAD_NET_RESP:		error = ENOSYS;		break;
	case ERROR_BAD_PATHNAME:		error = ENOENT;		break;
	case ERROR_BAD_PIPE:			error = EPIPE;		break;
	case ERROR_BAD_UNIT:			error = ENODEV;		break;
	case ERROR_BAD_USERNAME:		error = EINVAL;		break;
	case ERROR_BEGINNING_OF_MEDIA:		error = EIO;		break;
	case ERROR_BROKEN_PIPE:			error = EPIPE;		break;
	case ERROR_BUFFER_OVERFLOW:		error = ENAMETOOLONG;	break;
	case ERROR_BUSY:			error = EBUSY;		break;
	case ERROR_BUSY_DRIVE:			error = EBUSY;		break;
	case ERROR_BUS_RESET:			error = EIO;		break;
	case ERROR_CALL_NOT_IMPLEMENTED:	error = ENOSYS;		break;
	case ERROR_CANCELLED:			error = EINTR;		break;
	case ERROR_CANNOT_MAKE:			error = EACCES;		break;
	case ERROR_CANTOPEN:			error = EIO;		break;
	case ERROR_CANTREAD:			error = EIO;		break;
	case ERROR_CANTWRITE:			error = EIO;		break;
	case ERROR_CHILD_NOT_COMPLETE:		error = EBUSY;		break;
	case ERROR_COMMITMENT_LIMIT:		error = EAGAIN;		break;
	case ERROR_CONNECTION_REFUSED:		error = ECONNREFUSED;	break;
	case ERROR_CRC:				error = EIO;		break;
	case ERROR_CURRENT_DIRECTORY:		error = EACCES;		break;
	case ERROR_DEVICE_DOOR_OPEN:		error = EIO;		break;
	case ERROR_DEVICE_IN_USE:		error = EBUSY;		break;
	case ERROR_DEVICE_REQUIRES_CLEANING:	error = EIO;		break;
	case ERROR_DEV_NOT_EXIST:		error = ENODEV;		break;
	case ERROR_DIRECTORY:			error = ENOTDIR;	break;
	case ERROR_DIR_NOT_EMPTY:		error = ENOTEMPTY;	break;
	case ERROR_DISK_CHANGE:			error = EIO;		break;
	case ERROR_DISK_CORRUPT:		error = EIO;		break;
	case ERROR_DISK_FULL:			error = ENOSPC;		break;
	case ERROR_DRIVE_LOCKED:		error = EBUSY;		break;
	case ERROR_DS_GENERIC_ERROR:		error = EIO;		break;
	case ERROR_DUP_NAME:			error = EEXIST;		break;
	case ERROR_EAS_DIDNT_FIT:		error = ENOSPC;		break;
	case ERROR_EAS_NOT_SUPPORTED:		error = ENOTSUP;	break;
	case ERROR_EA_LIST_INCONSISTENT:	error = EINVAL;		break;
	case ERROR_EA_TABLE_FULL:		error = ENOSPC;		break;
	case ERROR_END_OF_MEDIA:		error = ENOSPC;		break;
	case ERROR_ENVVAR_NOT_FOUND:		error = EINVAL;		break;
	case ERROR_EOM_OVERFLOW:		error = EIO;		break;
	case ERROR_EXE_MACHINE_TYPE_MISMATCH:	error = ENOEXEC;	break;
	case ERROR_EXE_MARKED_INVALID:		error = ENOEXEC;	break;
	case ERROR_FILEMARK_DETECTED:		error = EIO;		break;
	case ERROR_FILENAME_EXCED_RANGE:	error = ENAMETOOLONG;	break;
	case ERROR_FILE_CORRUPT:		error = EINVAL;		break;
	case ERROR_FILE_EXISTS:			error = EEXIST;		break;
	case ERROR_FILE_INVALID:		error = ENXIO;		break;
	case ERROR_FILE_NOT_FOUND:		error = ENOENT;		break;
	case ERROR_GEN_FAILURE:			error = EIO;		break;
	case ERROR_HANDLE_DISK_FULL:		error = ENOSPC;		break;
	case ERROR_HANDLE_EOF:			error = ENODATA;	break;
	case ERROR_INSUFFICIENT_BUFFER:		error = ENOMEM;		break;
	case ERROR_INVALID_ACCESS:		error = EACCES;		break;
	case ERROR_INVALID_ADDRESS:		error = EINVAL;		break;
	case ERROR_INVALID_AT_INTERRUPT_TIME:	error = EINTR;		break;
	case ERROR_INVALID_BLOCK:		error = EFAULT;		break;
	case ERROR_INVALID_BLOCK_LENGTH:	error = EIO;		break;
	case ERROR_INVALID_DATA:		error = EINVAL;		break;
	case ERROR_INVALID_DRIVE:		error = ENODEV;		break;
	case ERROR_INVALID_EA_NAME:		error = EINVAL;		break;
	case ERROR_INVALID_EXE_SIGNATURE:	error = ENOEXEC;	break;
	case ERROR_INVALID_FLAGS:		error = EINVAL;		break;
	case ERROR_INVALID_FUNCTION:		error = ENOSYS;		break;
	case ERROR_INVALID_HANDLE:		error = EBADF;		break;
	case ERROR_INVALID_LOGON_HOURS:		error = EACCES;		break;
	case ERROR_INVALID_NAME:		error = EINVAL;		break;
	case ERROR_INVALID_OWNER:		error = EINVAL;		break;
	case ERROR_INVALID_PARAMETER:		error = EINVAL;		break;
	case ERROR_INVALID_PASSWORD:		error = EPERM;		break;
	case ERROR_INVALID_PRIMARY_GROUP:	error = EINVAL;		break;
	case ERROR_INVALID_SIGNAL_NUMBER:	error = EINVAL;		break;
	case ERROR_INVALID_TARGET_HANDLE:	error = EIO;		break;
	case ERROR_INVALID_WORKSTATION:		error = EACCES;		break;
	case ERROR_IOPL_NOT_ENABLED:		error = ENOEXEC;	break;
	case ERROR_IO_DEVICE:			error = EIO;		break;
	case ERROR_IO_INCOMPLETE:		error = EINTR;		break;
	case ERROR_IO_PENDING:			error = EAGAIN;		break;
	case ERROR_LOCKED:			error = EBUSY;		break;
	case ERROR_LOCK_VIOLATION:		error = EBUSY;		break;
	case ERROR_LOGON_FAILURE:		error = EACCES;		break;
	case ERROR_MAPPED_ALIGNMENT:		error = EINVAL;		break;
	case ERROR_MAX_THRDS_REACHED:		error = EAGAIN;		break;
	case ERROR_META_EXPANSION_TOO_LONG:	error = E2BIG;		break;
	case ERROR_MOD_NOT_FOUND:		error = ENOENT;		break;
	case ERROR_MORE_DATA:			error = EMSGSIZE;	break;
	case ERROR_NEGATIVE_SEEK:		error = EINVAL;		break;
	case ERROR_NETNAME_DELETED:		error = ENOENT;		break;
	case ERROR_NOACCESS:			error = EFAULT;		break;
	case ERROR_NONE_MAPPED:			error = EINVAL;		break;
	case ERROR_NONPAGED_SYSTEM_RESOURCES:	error = EAGAIN;		break;
	case ERROR_NOT_CONNECTED:		error = ENOLINK;	break;
	case ERROR_NOT_ENOUGH_MEMORY:		error = ENOMEM;		break;
	case ERROR_NOT_ENOUGH_QUOTA:		error = EIO;		break;
	case ERROR_NOT_OWNER:			error = EPERM;		break;
	case ERROR_NOT_READY:			error = EAGAIN;		break;
	case ERROR_NOT_SAME_DEVICE:		error = EXDEV;		break;
	case ERROR_NOT_SUPPORTED:		error = ENOSYS;		break;
	case ERROR_NO_DATA:			error = EPIPE;		break;
	case ERROR_NO_DATA_DETECTED:		error = EIO;		break;
	case ERROR_NO_MEDIA_IN_DRIVE:		error = EAGAIN;		break;
	case ERROR_NO_MORE_FILES:		error = EMFILE;		break;
	case ERROR_NO_MORE_SEARCH_HANDLES:	error = ENFILE;		break;
	case ERROR_NO_PROC_SLOTS:		error = EAGAIN;		break;
	case ERROR_NO_SIGNAL_SENT:		error = EIO;		break;
	case ERROR_NO_SUCH_PRIVILEGE:		error = EACCES;		break;
	case ERROR_NO_SYSTEM_RESOURCES:		error = EFBIG;		break;
	case ERROR_NO_TOKEN:			error = EINVAL;		break;
	case ERROR_OPEN_FAILED:			error = EIO;		break;
	case ERROR_OPEN_FILES:			error = EBUSY;		break;
	case ERROR_OPERATION_ABORTED:		error = EINTR;		break;
	case ERROR_OUTOFMEMORY:			error = ENOMEM;		break;
	case ERROR_PAGED_SYSTEM_RESOURCES:	error = EAGAIN;		break;
	case ERROR_PAGEFILE_QUOTA:		error = EAGAIN;		break;
	case ERROR_PASSWORD_EXPIRED:		error = EACCES;		break;
	case ERROR_PATH_BUSY:			error = EBUSY;		break;
	case ERROR_PATH_NOT_FOUND:		error = ENOENT;		break;
	case ERROR_PIPE_BUSY:			error = EBUSY;		break;
	case ERROR_PIPE_CONNECTED:		error = EBUSY;		break;
	case ERROR_PIPE_LISTENING:		error = EIO;		break;
	case ERROR_PIPE_NOT_CONNECTED:		error = EIO;		break;
	case ERROR_POSSIBLE_DEADLOCK:		error = EDEADLOCK;	break;
	case ERROR_PRIVILEGE_NOT_HELD:		error = EPERM;		break;
	case ERROR_PROCESS_ABORTED:		error = EFAULT;		break;
	case ERROR_PROC_NOT_FOUND:		error = ESRCH;		break;
	case ERROR_READ_FAULT:			error = EIO;		break;
	case ERROR_REM_NOT_LIST:		error = ENOENT;		break;
	case ERROR_SECTOR_NOT_FOUND:		error = EINVAL;		break;
	case ERROR_SEEK:			error = EIO;		break;
	case ERROR_SEEK_ON_DEVICE:		error = ESPIPE;		break;
	case ERROR_SERVICE_REQUEST_TIMEOUT:	error = EBUSY;		break;
	case ERROR_SETMARK_DETECTED:		error = EIO;		break;
	case ERROR_SHARING_BUFFER_EXCEEDED:	error = ENOLCK;		break;
	case ERROR_SHARING_VIOLATION:		error = EBUSY;		break;
	case ERROR_SIGNAL_PENDING:		error = EBUSY;		break;
	case ERROR_SIGNAL_REFUSED:		error = EIO;		break;
	case ERROR_STACK_OVERFLOW:		error = ENOMEM;		break;
	case ERROR_SWAPERROR:			error = ENOENT;		break;
	case ERROR_SXS_CANT_GEN_ACTCTX:		error = EIO;		break;
	case ERROR_THREAD_1_INACTIVE:		error = EINVAL;		break;
	case ERROR_TIMEOUT:			error = EBUSY;		break;
	case ERROR_TOO_MANY_LINKS:		error = EMLINK;		break;
	case ERROR_TOO_MANY_MODULES:		error = EMFILE;		break;
	case ERROR_TOO_MANY_OPEN_FILES:		error = EMFILE;		break;
	case ERROR_UNEXP_NET_ERR:		error = EIO;		break;
	case ERROR_UNRECOGNIZED_MEDIA:		error = ENXIO;		break;
	case ERROR_UNRECOGNIZED_VOLUME:		error = ENODEV;		break;
	case ERROR_WAIT_NO_CHILDREN:		error = ECHILD;		break;
	case ERROR_WORKING_SET_QUOTA:		error = EAGAIN;		break;
	case ERROR_WRITE_FAULT:			error = EIO;		break;
	case ERROR_WRITE_PROTECT:		error = EROFS;		break;
    }

    if (error == ERROR_SUCCESS)
	return error_condition(ev, windows_category());
    else
	return error_condition(error, generic_category());
}

inline string odbc::WindowsCategory::message(int ev) const
{
    class LocalHandle
    {
    public:
	HLOCAL localText = NULL;

	~LocalHandle()
	{
	    ::LocalFree(localText);
	    localText = NULL;
	}
    }
	localHandle;

    DWORD dwCharCount =
	::FormatMessageA
	    (
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		static_cast<DWORD>(ev),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)(&localHandle.localText),
		0u,
		NULL
	    );

    if (!dwCharCount)
	return "(Failed to format message for Windows API error "s + to_string(static_cast<DWORD>(ev)) + ')';

    string errorMessage { static_cast<char const *>(localHandle.localText), dwCharCount };

    while (errorMessage.size() && (*errorMessage.crbegin() == '\n' || *errorMessage.crbegin() == '\r'))
	errorMessage.pop_back();

    return errorMessage;
}
