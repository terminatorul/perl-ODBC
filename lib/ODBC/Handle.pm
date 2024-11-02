use v6;

use ODBC::SQL;
use X::ODBC;

class ODBC::Handle::Impl:api<1>
{
    our constant SQL = ODBC::SQL;

    our &notificationHandler = sub (X::ODBC::SQLNotification:D $notification)
    {
	$notification.throw
    }

}

unit class ODBC::Handle;

our constant SQL = ODBC::SQL;

our &notificationHandler := &ODBC::Handle::Impl:api<1>::notificationHandler;

has $.unicode;
has $.impl handles @SQL::sql-methods;
has Pointer $.handle .= new;

our sub decodeNativeString(CArray[uint8] $arr, UInt $elems) returns Str
{
    my Int $strlen = SQL::MultiByteToWideChar(SQL::CP_THREAD_ACP, SQL::MB_PRECOMPOSED +| SQL::MB_ERR_INVALID_CHARS, $arr, $elems, CArray[uint16], 0);

    if ($strlen > 0)
    {
	my Buf[uint16] $decoded .= allocate($strlen);
	$strlen = ODBC::SQL::MultiByteToWideChar(SQL::CP_THREAD_ACP, SQL::MB_PRECOMPOSED +| SQL::MB_ERR_INVALID_CHARS,
		$arr, $elems, nativecast(CArray[uint16], $decoded), $decoded.elems);

	return $decoded.decode('utf16')
    }

    die "Unicode conversion error"
}

our sub decodeNativeWin(Buf $arr) returns Str
{
    decodeNativeString(nativecast(CArray[uint8], $arr), $arr.elems)
}

our sub decodeNativeUnix(Buf $arr) returns Str
{
    $arr.decode
}

our constant &decodeNative = $*DISTRO.is-win ?? &decodeNativeWin !! &decodeNativeUnix;

method handleType() returns SQL::HANDLE
{
    ...
}

method GetDiagRecA(UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
{
    my Buf   $sqlStateBuf .= allocate(6);
    my int32 $nativeCode   = 0;
    my int16 $messageLen   = 0;
    my int16 $sqlReturn    = $.SQLGetDiagRec.($.handleType, $.handle, $recordNumber, nativecast(CArray[uint8], $sqlStateBuf), $nativeCode, CArray[uint8], 0, $messageLen);

    given $sqlReturn
    {
	when SQL::SUCCESS | SQL::SUCCESS_WITH_INFO
	{
	    my Buf $messageBuf .= allocate($messageLen + 1);

	    $sqlReturn = $SQLGetDiagRec.($.handleType, $.handle, $recordNumber, nativecast(CArray[uint8], $sqlStateBuf), $nativeCode, nativecast(CArray[uint8], $messageBuf), $messageBuf.elems, $messageLen);

	    if $sqlReturn == ODBC::SQL::SUCCESS
	    {
		$sqlStateBuf.reallocate($sqlStateBuf.elems - 1);
		$messageBuf.reallocate($messageBuf.elems - 1);

		$sqlState = decodeNative($sqlStateBuf);
		$message  = decodeNative($messageBuf);
		$nativeError = $nativeCode;
	    }
	}
    }

    return $sqlReturn;
}

method GetDiagRecW(UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
{
    my Buf[uint16] $sqlStateBuf .= allocate(6);
    my int32	   $nativeCode   = 0;
    my int16	   $messageLen   = 0;
    my int16	   $sqlReturn    = $.SQLGetDiagRecW.($.handleType, $.handle, $recordNumber, nativecast(CArray[uint16], $sqlStateBuf), $nativeCode, CArray[uint16], 0, $messageLen);

    given $sqlReturn
    {
	when SQL::SUCCESS | SQL::SUCCESS_WITH_INFO
	{
	    my Buf[uint16] $messageBuf .= allocate($messageLen + 1);

	    $sqlReturn = $.SQLGetDiagRecW.($.handleType, $.handle, $recordNumber, nativecast(CArray[uint16], $sqlStateBuf), $nativeCode, nativecast(CArray[uint16], $messageBuf), $messageBuf.elems, $messageLen);

	    if $sqlReturn == SQL::SUCCESS
	    {
		$sqlStateBuf.reallocate($sqlStateBuf.elems - 1);
		$messageBuf.reallocate($messageBuf.elems - 1);

		$sqlState = $sqlStateBuf.decode('utf16');
		$message  = $messageBuf.decode('utf16');
		$nativeError = $nativeCode;
	    }
	}
    }

    return $sqlReturn;
}

has &.GetDiagRec = GetDiagRecA;

method enableUnicodeFunctions()
{
    &.GetDiagRec = GetDiagRecW
}

method enabledAnsiFunctions()
{
    &.GetDiagRec = GetDiagRecA
}

method emitSqlError(Int:D $recordNumber, Bool:D $warnOnly)
{
    my Str $sqlState;
    my Int $nativeCode;
    my Str $message;

    with self
    {
	with $.handle
	{
	    given GetDiagRec($.handleType, $.handle, $recordNumber, $sqlState, $nativeCode, $message)
	    {
		when SQL::SUCCESS
		{
		    if ($warnOnly)
		    {
			notificationHandler(X::ODBC::SQLInfo[$sqlState].new(:$nativeCode, :message($message)));
		    }
		    else
		    {
			X::ODBC::SQLState[$sqlState].new(:$nativeCode, :message($message)).throw;
		    }

		    return True
		}

		when SQL::NO_DATA
		{
		    return False
		}

		default
		{
		    $*ERR.say("Error reading diagnostic for SQLRETURN code {$_}")
		}
	    }
	}
    }

    return False
}

method raiseSqlError(Int:D $recordNumber)
{
    $.emitSqlError($recordNumber, False)
}

method showSqlError(Int:D $recordNumber)
{
    $.emitSqlError($recordNumber, True)
}

method validateSqlReturn(Int:D $sqlReturn)
{
    given $sqlReturn
    {
	when (SQL::SUCCESS)
	{
	    return
	}

	when (SQL::SUCCESS_WITH_INFO)
	{
	    my Int:D $recordNumber = 1;

	    $recordNumber++ while $.showSqlError($recordNumber)
	}

	when (SQL::ERROR)
	{
	    my Int:D $recordNumber = 1;

	    $recordNumber++ while $.raiseSqlError($recordNumber);

	    if $recordNumber == 1
	    {
		X::ODBC::SqlReturn[$sqlReturn].new.throw
	    }
	}

	default
	{
	    X::ODBC::SqlReturn[$sqlReturn].new.throw
	}
    }
}

method dispose
{
    with $.handleType
    {
	$.validateSqlReturn($.SQLFreeHandle.($!handleType, $!handle));

	$!handleType = ODBC::SQL::HANDLE;
	$!handle  = Pointer;
    }
}

submethod DESTROY
{
    self.dispose
}

# vi: ft=perl6
