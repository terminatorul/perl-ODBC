use v6;
use NativeCall;

module ODBC::SQL
{

    constant base-library-name = $*DISTRO.is-win ?? 'ODBC32' !! $*DISTRO.name ~~ /macosx?/ ?? 'iodbc' !! 'odbc';

    sub LIBNAME
    {
	state Str $libname = %*ENV{'SQLODBC_LIBRARY_NAME'} // base-library-name;

	$libname;
    }

    our Int enum CP
    (
	CP_ACP	      => 0,
	CP_OEMCP      => 1,
	CP_MACCP      => 2,
	CP_THREAD_ACP => 3,
	CP_SYMBOL     => 42,
	CP_UTF7	      => 65000,
	CP_UTF8	      => 65001
    );

    our Int enum MB
    (
	MB_PRECOMPOSED		=> 1,
	MB_COMMPOSITE	    	=> 2,
	MB_USEGLYPHCHARS    	=> 4,
	MB_ERR_INVALID_CHARS	=> 8
    );

    if ($*DISTRO.is-win)
    {
	constant MAX_DEFAULTCHAR =   2;
	constant MAX_LEADBYTES	 =  12;
	constant MAX_PATH	 = 260;

	class CP-INFO-EX
	{
	    has uint32	$.MaxCharSize;
	    has Blob	$.DefaultChar;
	    has Blob	$.LeadBytes;
	    has uint16	$.UnicodeDefaultChar;
	    has uint32	$.CodePage;
	    has Str	$.CodePageName;

	    method BUILD(Blob:D :$buf)
	    {
		my Int:D $pos = 0;

		$!MaxCharSize = $buf.read-uint32($pos);
		$pos += nativesizeof(uint32);

		# $.DefaultChar = Blob.new($buf[$pos ..^ ($pos + MAX_DEFAULTCHAR)]).decode;
		$pos += MAX_DEFAULTCHAR;

		# $.Leadbytes   = Blob.new($buf[$pos ..^ ($pos + MAX_LEADBYTES)]).decode;
		$pos += MAX_LEADBYTES;

		$!UnicodeDefaultChar = $buf.read-uint16($pos);
		$pos += nativesizeof(uint16);

		$!CodePage = $buf.read-uint32($pos);
		$pos += nativesizeof(uint32);

		$!CodePageName = Blob.new($buf[$pos ..^ ($pos + MAX_PATH)]).decode('iso-8859-1');
	    }
	}

	sub GetCPInfoExA(uint32 $CodePage, uint32 $dwFlags, Pointer[uint8] $cp-info) returns Bool is native('KERNEL32')
	{
	    *
	}

	# my Buf $cp-info .= allocate(284);

	# GetCPInfoExA(CP_THREAD_ACP, 0, nativecast(Pointer[uint8], $cp-info));

	# say "Thread active code page: ", CP-INFO-EX.new(buf => $cp-info).CodePageName
    }

    our Int enum HANDLE
    (
	ENV  => 1,
	DBC  => 2,
	STMT => 3,
	DESC => 4,
	SENV => 5	# v30 or later
    );

    our Int enum SQLRETURN
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

    our Int enum FETCH
    (
	FETCH_NEXT	=> 1,
	FETCH_FIRST 	=> 2,
	FETCH_LAST  	=> 3,
	FETCH_PRIOR 	=> 4,
	FETCH_ABSOLUTE	=> 5,
	FETCH_RELATIVE 	=> 6
    );

    our  Int enum ENV_ATTR
    (
	ATTR_ODBC_VERSION	=>   200,
	ATTR_CONNECTION_POOLING =>   201,
	ATTR_CP_MATCH		=>   202,
	ATTR_APPLICATIOON_KEY	=>   203,

	ATTR_OUTPUT_NTS		=> 10001
    );

    our Int enum CONN_ATTR
    (
	ATTR_AUTO_IPD		=> 10001,
	ATTR_METADATA_ID	=> 10014
    );

    our Int enum STMT_ATTR
    (
	ATTR_CURSOR_SENSITIVEY	=>    -2,
	ATTR_CURSOR_SCROLLABLE	=>    -1,
	ATTR_APP_ROW_DESC	=> 10010,
	ATTR_APP_PARAM_DESC	=> 10011,
	ATTR_IMP_ROW_DESC	=> 10012,
	ATTR_IMP_PARAM_DESC	=> 10013
    );

    our Int enum ODBC_VERSION
    (
	OV_ODBC2    => 2,
	OV_ODBC3    => 3,
	OV_ODBC3_80 => 380
    );

    constant SQL_ODBC_VER = 10;

    constant SQLRETURN_   =  int16;
    constant SQLHENV	  = Pointer;
    constant SQLHDBC	  = Pointer;
    constant SQLPOINTER	  = Pointer;
    constant SQLWCHAR 	  = $*DISTRO.name ~~ /macosx?/ ?? uint32 !! uint16;
    constant SQLUSMALLINT = uint16;
    constant SQLSMALLINT  =  int16;

    our sub MultiByteToWideChar
		(
		    uint32 $codePage,
		    uint32 $dwFlags,
		    CArray[uint8]  $lpMultiByteStr, int32 $cbMultiByte,
		    CArray[uint16] $lpWideCharStr,  int32 $ccWideChar
		)
		    returns int32 is native('KERNEL32')
    {
	*
    }

    our sub GetDiagRec
		(
		    int16	   $handleType, Pointer $handle,
		    int16	   $recordNumber,
		    CArray[uint8]  $sqlState,
		    int32	   $nativeError is rw,
		    CArray[uint8]  $message, int16 $messageMaxLen, int16 $messageLenPtr is rw
		)
		    returns int16 is native(LIBNAME) is symbol('SQLGetDiagRec')
    {
	*
    }

    our sub GetDiagRecW
		(
		    int16	   $handleType, Pointer $handle,
		    int16	   $recordNumber,
		    CArray[uint32] $sqlState,
		    int32	   $nativeError is rw,
		    CArray[uint32] $message, int16 $messageMaxLen, int16 $messageLenPtr is rw
		)
		    returns int16 is native(LIBNAME) is symbol('SQLGetDiagRecW')
    {
	*
    }

    our sub AllocHandle(int16 $handleType, Pointer $handle, Pointer $resultHandle is rw) returns int16 is native(LIBNAME) is symbol('SQLAllocHandle')
    {
	*
    }

    our sub FreeHandle(int16 $handleType, Pointer $handle) returns int16 is native(LIBNAME) is symbol('SQLFreeHandle')
    {
	*
    }

    our sub SetEnvAttr(Pointer $hEnv, int32 $attribute, Pointer $valuePtr, int32 $valueLength) returns int16 is native(LIBNAME) is symbol('SQLSetEnvAttr')
    {
	*
    }

    our sub Drivers
		(
		    SQLHENV       $hEnv,
		    SQLUSMALLINT  $whence,
		    CArray[uint8] $driverDescription, SQLSMALLINT $descriptionMaxLen, SQLSMALLINT $descriptionLenPtr is rw,
		    CArray[uint8] $driverAttributes,  SQLSMALLINT $attributesMaxLen,  SQLSMALLINT $attributesLenPtr  is rw
		)
		    returns SQLRETURN_ is native(LIBNAME) is symbol('SQLDrivers')
    {
	*
    }

    our sub DriversW
		(
		    SQLHENV          $hEnv,
		    SQLUSMALLINT     $whence,
		    CArray[SQLWCHAR] $driverDescription, SQLSMALLINT $descriptionMaxLen, SQLSMALLINT $descriptionLenPtr is rw,
		    CArray[SQLWCHAR] $driverAttributes,  SQLSMALLINT $attributesMaxLen,  SQLSMALLINT $attributesLenPtr  is rw
		)
		    returns SQLRETURN_ is native(LIBNAME) is symbol('SQLDriversW')
    {
	*
    }

    our sub GetInfo(SQLHDBC $hConn, SQLUSMALLINT $infoType, SQLPOINTER $infoValuePtr, SQLSMALLINT $bufferLength, SQLSMALLINT $stringLength is rw)
	returns SQLRETURN_ is native(LIBNAME) is symbol('SQLGetInfo')
    {
	*
    }

    our sub GetInfoW(SQLHDBC $hConn, SQLUSMALLINT $infoType, SQLPOINTER $infoValuePtr, SQLSMALLINT $bufferLength, SQLSMALLINT $stringLength is rw)
	returns SQLRETURN_ is native(LIBNAME) is symbol('SQLGetInfoW')
    {
	*
    }
}

# vi=ft=perl6;ts=8;sw=4
