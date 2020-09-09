use v6;
use NativeCall;

module ODBC::SQL
{
    constant base-library-name is export(:base-library-name) = $*DISTRO.is-win ?? 'ODBC32' !! $*DISTRO.name ~~ /macosx?/ ?? 'iodbc' !! 'odbc';

    sub LIBNAME is export(:LIBNAME)
    {
	state Str $libname = %*ENV{'SQLODBC_LIBRARY_NAME'} // base-library-name;

	$libname;
    }

    our Int enum CP is export(:WinTypes :CP)
    (
	CP_ACP	      => 0,
	CP_OEMCP      => 1,
	CP_MACCP      => 2,
	CP_THREAD_ACP => 3,
	CP_SYMBOL     => 42,
	CP_UTF7	      => 65000,
	CP_UTF8	      => 65001
    );

    our Int enum MB is export(:WinTypes :MB)
    (
	MB_PRECOMPOSED		=> 1,
	MB_COMMPOSITE	    	=> 2,
	MB_USEGLYPHCHARS    	=> 4,
	MB_ERR_INVALID_CHARS	=> 8
    );

    our Int enum HANDLE is export(:Types :HANDLE)
    (
	ENV  => 1,
	DBC  => 2,
	STMT => 3,
	DESC => 4,
	SENV => 5	# v30 or later
    );

    our Int enum SQL_RETURN is export(:Types :SQL_RETURN)
    (
	INVALID_HANDLE		=> -2,
	ERROR		    	=> -1,
	SUCCESS		    	=> 0,
	SUCCESS_WITH_INFO   	=> 1,
	STILL_EXECUTING	    	=> 2,
	NEED_DATA	    	=> 99,
	NO_DATA		    	=> 100,    # v3.0 or later
	PARAM_DATA_AVAILABLE	=> 101	   # v3.8 or later
    );

    our Int enum FETCH is export(:Types :FETCH)
    (
	FETCH_NEXT	=> 1,
	FETCH_FIRST 	=> 2,
	FETCH_LAST  	=> 3,
	FETCH_PRIOR 	=> 4,
	FETCH_ABSOLUTE	=> 5,
	FETCH_RELATIVE 	=> 6
    );

    our Int enum ENV_ATTR is export(:Types :ENV_ATTR)
    (
	ATTR_ODBC_VERSION	=>   200,
	ATTR_CONNECTION_POOLING =>   201,
	ATTR_CP_MATCH		=>   202,
	ATTR_APPLICATIOON_KEY	=>   203,

	ATTR_OUTPUT_NTS		=> 10001
    );

    our Int enum CONN_ATTR is export(:Types :CONN_ATTR)
    (
	ATTR_AUTO_IPD		=> 10001,
	ATTR_METADATA_ID	=> 10014
    );

    our Int enum STMT_ATTR is export(:Types :STMT_ATTR)
    (
	ATTR_CURSOR_SENSITIVEY	=>    -2,
	ATTR_CURSOR_SCROLLABLE	=>    -1,
	ATTR_APP_ROW_DESC	=> 10010,
	ATTR_APP_PARAM_DESC	=> 10011,
	ATTR_IMP_ROW_DESC	=> 10012,
	ATTR_IMP_PARAM_DESC	=> 10013
    );

    our Int enum ODBC_VERSION is export(:Types :ODBC_VERSION)
    (
	OV_ODBC2    => 2,
	OV_ODBC3    => 3,
	OV_ODBC3_80 => 380
    );

    constant SQL_ODBC_VER is export(:const :SQL_ODBC_VER) = 10;

    constant SQLRETURN    is export(:Native :NativeTypes :SQLRETURN)    = int16;
    constant SQLHANDLE    is export(:Native :NativeTypes :SQLHANDLE)    = Pointer;
    constant SQLHENV      is export(:Native :NativeTypes :SQLHENV)      = Pointer;
    constant SQLHDBC      is export(:Native :NativeTypes :SQLHDBC)      = Pointer;
    constant SQLHSTMT     is export(:Native :NativeTypes :SQLHSTMT)	= Pointer;
    constant SQLPOINTER	  is export(:Native :NativeTypes :SQLPOINTER)   = Pointer;
    constant SQLCHAR      is export(:Native :NativeTypes :SQLCHAR)      = uint8;
    constant SQLWCHAR 	  is export(:Native :NativeTypes :SQLWCAHR)     = $*DISTRO.name ~~ /macosx?/ ?? uint32 !! uint16;
    constant SQLUSMALLINT is export(:Native :NativeTypes :SQLUSMALLINT) = uint16;
    constant SQLSMALLINT  is export(:Native :NativeTypes :SQLSMALLINT)  = int16;
    constant SQLINTEGER   is export(:Native :NativeTypes :SQLINTEGER)   = int32;

    our sub MultiByteToWideChar
		(
		    uint32 $codePage,
		    uint32 $dwFlags,
		    CArray[uint8]  $lpMultiByteStr, int32 $cbMultiByte,
		    CArray[uint16] $lpWideCharStr,  int32 $ccWideChar
		)
		    returns int32 is native('KERNEL32') is export(:Native :WinAPI :MultiByteToWideChar)
    {
	*
    }

    ## ODBC v1.0
    our sub Error
		(
		    SQLHENV $hEnv, SQLHDBC $hDBC, SQLHSTMT $hStmt,
		    CArray[SQLCHAR] $sqlState, SQLINTEGER $nativeError is rw,
		    CArray[SQLCHAR] $errorMsg, SQLSMALLINT $errorMsgMax, SQLSMALLINT $errorMsgCount is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLError') is export(:Native :API :Error)
    {
	*
    }

    ## ODBC v3.5
    our sub ErrorW
		(
		    SQLHENV $hEnv, SQLHDBC $hDBC, SQLHSTMT $hStmt,
		    CArray[SQLWCHAR] $sqlState, SQLINTEGER $nativeError is rw,
		    CArray[SQLWCHAR] $errorMsg, SQLSMALLINT $errorMsgMax, SQLSMALLINT $errorMsgCount is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLErrorW') is export(:Native :API :ErrorW)
    {
	*
    }

    ## ODBC v3.0
    our sub GetDiagRec
		(
		    SQLSMALLINT	    $handleType, SQLHANDLE $handle,
		    SQLSMALLINT	    $recordNumber,
		    CArray[SQLCHAR] $sqlState,
		    SQLINTEGER	    $nativeError is rw,
		    CArray[SQLCHAR] $message, SQLSMALLINT $messageMaxLen, SQLSMALLINT $messageLenPtr is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLGetDiagRec') is export(:Native :API :GetDiagRec)
    {
	*
    }

    ## ODBC v3.5
    our sub GetDiagRecW
		(
		    SQLSMALLINT      $handleType, SQLHANDLE $handle,
		    SQLSMALLINT	     $recordNumber,
		    CArray[SQLWCHAR] $sqlState,
		    SQLINTEGER	     $nativeError is rw,
		    CArray[SQLWCHAR] $message, SQLSMALLINT $messageMaxLen, SQLSMALLINT $messageLenPtr is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLGetDiagRecW') is export(:Native :API :GetDiagRecW)
    {
	*
    }

    ## ODBC v3.0
    our sub AllocHandle(SQLSMALLINT $handleType, SQLHANDLE $handle, SQLHANDLE $resultHandle is rw)
	    returns SQLRETURN is native(LIBNAME) is symbol('SQLAllocHandle') is export(:Native :API :AllocHandle)
    {
	*
    }

    ## ODBC v3.0
    our sub FreeHandle(SQLSMALLINT $handleType, SQLHANDLE $handle) returns SQLRETURN is native(LIBNAME) is symbol('SQLFreeHandle') is export(:Native :API :FreeHandle)
    {
	*
    }

    ## ODBC v3.0
    our sub SetEnvAttr(SQLHENV $hEnv, SQLINTEGER $attribute, SQLPOINTER $valuePtr, SQLINTEGER $valueLength)
	    returns SQLRETURN is native(LIBNAME) is symbol('SQLSetEnvAttr') is export(:Native :API :SetEnvAttr)
    {
	*
    }

    ## 2.0
    our sub Drivers
		(
		    SQLHENV         $hEnv,
		    SQLUSMALLINT    $whence,
		    CArray[SQLCHAR] $driverDescription, SQLSMALLINT $descriptionMaxLen, SQLSMALLINT $descriptionLenPtr is rw,
		    CArray[SQLCHAR] $driverAttributes,  SQLSMALLINT $attributesMaxLen,  SQLSMALLINT $attributesLenPtr  is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLDrivers') is export(:Native :API :Drivers)
    {
	*
    }

    ## 3.5
    our sub DriversW
		(
		    SQLHENV          $hEnv,
		    SQLUSMALLINT     $whence,
		    CArray[SQLWCHAR] $driverDescription, SQLSMALLINT $descriptionMaxLen, SQLSMALLINT $descriptionLenPtr is rw,
		    CArray[SQLWCHAR] $driverAttributes,  SQLSMALLINT $attributesMaxLen,  SQLSMALLINT $attributesLenPtr  is rw
		)
		    returns SQLRETURN is native(LIBNAME) is symbol('SQLDriversW') is export(:Native :API :DriversW)
    {
	*
    }

    ## 1.0
    our sub GetInfo(SQLHDBC $hConn, SQLUSMALLINT $infoType, SQLPOINTER $infoValuePtr, SQLSMALLINT $bufferLength, SQLSMALLINT $stringLength is rw)
	returns SQLRETURN is native(LIBNAME) is symbol('SQLGetInfo') is export(:Native :API :GetInfo)
    {
	*
    }

    ## 3.5
    our sub GetInfoW(SQLHDBC $hConn, SQLUSMALLINT $infoType, SQLPOINTER $infoValuePtr, SQLSMALLINT $bufferLength, SQLSMALLINT $stringLength is rw)
	returns SQLRETURN is native(LIBNAME) is symbol('SQLGetInfoW') is export(:Native :API :GetInfoW)
    {
	*
    }
}

# vi=ft=perl6;ts=8;sw=4
