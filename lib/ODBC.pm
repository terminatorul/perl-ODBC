use v6;
use NativeCall;
use X::ODBC;
use ODBC::SQL;

module ODBC
{
    our &notificationHandler = sub (X::ODBC::SQLNotification:D $notification)
    {
	$notification.throw
    }

    our $unicode;

    my sub decodeNativeString(CArray[uint8] $arr, UInt $elems) returns Str
    {
	my Int $strlen = ODBC::SQL::MultiByteToWideChar(ODBC::SQL::CP_THREAD_ACP, ODBC::SQL::MB_PRECOMPOSED +| ODBC::SQL::MB_ERR_INVALID_CHARS, $arr, $elems, CArray[uint16], 0);

	if ($strlen > 0)
	{
	    my Buf[uint16] $decoded .= allocate($strlen);
	    $strlen = ODBC::SQL::MultiByteToWideChar(ODBC::SQL::CP_THREAD_ACP, ODBC::SQL::MB_PRECOMPOSED +| ODBC::SQL::MB_ERR_INVALID_CHARS,
		    $arr, $elems, nativecast(CArray[uint16], $decoded), $decoded.elems);

	    return $decoded.decode('utf-16')
	}

	die "Unicode conversion error"
    }

    my sub decodeNative(Buf $arr) returns Str
    {
	return decodeNativeString(nativecast(CArray[uint8], $arr), $arr.elems);
    }

    sub GetDiagRecA(ODBC::SQL::HANDLE $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
    {
	my Buf	 $sqlStateBuf .= allocate(6);
	my int32 $nativeCode   = 0;
	my int16 $messageLen   = 0;
	my int16 $sqlReturn    = ODBC::SQL::GetDiagRec($handleType, $handle, $recordNumber, nativecast(CArray[uint8], $sqlStateBuf), $nativeCode, CArray[uint8], 0, $messageLen);

	given $sqlReturn
	{
	    when ODBC::SQL::SUCCESS | ODBC::SQL::SUCCESS_WITH_INFO
	    {
		my Buf $messageBuf .= allocate($messageLen + 1);

		$sqlReturn = ODBC::SQL::GetDiagRec($handleType, $handle, $recordNumber, nativecast(CArray[uint8], $sqlStateBuf), $nativeCode, nativecast(CArray[uint8], $messageBuf), $messageBuf.elems, $messageLen);

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

    sub GetDiagRecW(ODBC::SQL::HANDLE $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
    {
	my Buf[uint16]	$sqlStateBuf .= allocate(6);
	my int32	$nativeCode   = 0;
	my int16	$messageLen   = 0;
	my int16	$sqlReturn    = ODBC::SQL::GetDiagRecW($handleType, $handle, $recordNumber, nativecast(CArray[uint16], $sqlStateBuf), $nativeCode, CArray[uint16], 0, $messageLen);

	given $sqlReturn
	{
	    when ODBC::SQL::SUCCESS | ODBC::SQL::SUCCESS_WITH_INFO
	    {
		my Buf[uint16] $messageBuf .= allocate($messageLen + 1);

		$sqlReturn = ODBC::SQL::GetDiagRecW($handleType, $handle, $recordNumber, nativecast(CArray[uint16], $sqlStateBuf), $nativeCode, nativecast(CArray[uint16], $messageBuf), $messageBuf.elems, $messageLen);

		if $sqlReturn == ODBC::SQL::SUCCESS
		{
		    $sqlStateBuf.reallocate($sqlStateBuf.elems - 1);
		    $messageBuf.reallocate($messageBuf.elems - 1);

		    $sqlState = $sqlStateBuf.decode('utf-16');
		    $message  = $messageBuf.decode('utf-16');
		    $nativeError = $nativeCode;
		}
	    }
	}

	return $sqlReturn;
    }

    our sub GetDiagRec(ODBC::SQL::HANDLE $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
    {
	return GetDiagRecW($handleType, $handle, $recordNumber, $sqlState, $nativeError, $message) if $unicode;
	return GetDiagRecA($handleType, $handle, $recordNumber, $sqlState, $nativeError, $message)
    }

    class Handle
    {
	has ODBC::SQL::HANDLE	$.handleType;
	has Pointer		$.handle;

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
			when ODBC::SQL::SUCCESS
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

			when ODBC::SQL::NO_DATA
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
		when (ODBC::SQL::SUCCESS)
		{
		    return
		}

		when (ODBC::SQL::SUCCESS_WITH_INFO)
		{
		    my Int:D $recordNumber = 1;

		    $recordNumber++ while $.showSqlError($recordNumber)
		}

		when (ODBC::SQL::ERROR)
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
		$.validateSqlReturn(ODBC::SQL::FreeHandle($!handleType, $!handle));

		$!handleType = ODBC::SQL::HANDLE;
		$!handle  = Pointer;
	    }
	}

	submethod BUILD(ODBC::SQL::HANDLE:D :$handleType, Pointer :$handle = Pointer.new)
	{
	    $!handleType = $handleType;
	    $!handle  = $handle
	}

	submethod DESTROY
	{
	    self.dispose
	}
    }

    class Environment is Handle
    {
	method new(ODBC::SQL::ODBC_VERSION $odbcVersion = ODBC::SQL::OV_ODBC3_80)
	{
	    my $self = self.bless(:handleType(ODBC::SQL::ENV));
	    $.validateSqlReturn(ODBC::SQL::AllocHandle(ODBC::SQL::HANDLE::ENV, Pointer, $self.handle));
	    $.validateSqlReturn(ODBC::SQL::SetEnvAttr($self.handle, ODBC::SQL::ATTR_ODBC_VERSION, Pointer[uint32].new($odbcVersion), nativesizeof(uint32)));

	    return $self
	}

	method !next-driver-buffers(ODBC::SQL::FETCH $whence)
	{
	    my int16 $descLen = 0;
	    my int16 $attrLen = 0;

	    {
		if $unicode
		{
		    $.validateSqlReturn(ODBC::SQL::DriversW($.handle, $whence, CArray[uint16], 0, $descLen, CArray[uint16], 0, $attrLen));
		}
		else
		{
		    $.validateSqlReturn(ODBC::SQL::Drivers($.handle, $whence, CArray[uint8],  0, $descLen, CArray[uint8],  0, $attrLen));
		}

		CATCH
		{
		    when X::ODBC::NoData
		    {
			return ()
		    }
		}
	    }

	    return Buf[uint16].allocate($descLen + 1), Buf[uint16].allocate($attrLen + 1) if $unicode;
	    return Buf.allocate($descLen + 1), Buf.allocate($attrLen + 1)
	}

	method !next-driver(ODBC::SQL::FETCH $whence, $buffers)
	{
	    my int16 $descLen = 0;
	    my int16 $attrLen = 0;

	    if $unicode
	    {
		my Buf[uint16] $descBuff = @$buffers[0];
		my Buf[uint16] $attrBuff = @$buffers[1];

		$.validateSqlReturn(ODBC::SQL::DriversW($.handle, $whence, nativecast(CArray[uint16], $descBuff), $descBuff.elems - 1, $descLen, nativecast(CArray[uint16], $attrBuff), $attrBuff.elems, $attrLen));

		return $descBuff.decode('utf-16') => %( map { my @pair = $_.split('='); @pair[1] //= True; pair(|@pair) }, grep { $_ }, $attrBuff.decode('utf-16').split("\0") );
	    }
	    else
	    {
		my Buf $descBuff = @$buffers[0];
		my Buf $attrBuff = @$buffers[1];

		$.validateSqlReturn(ODBC::SQL::Drivers($.handle, $whence, nativecast(CArray[uint8], $descBuff), $descBuff.elems - 1, $descLen, nativecast(CArray[uint8], $attrBuff), $attrBuff.elems, $attrLen));

		return decodeNative($descBuff) => %( map { my @pair = $_.split('='); @pair[1] //= True; pair(|@pair) }, grep { $_ }, decodeNative($attrBuff).split("\0") );
	    }
	}

	method drivers
	{
	    my Int $whence = ODBC::SQL::FETCH_FIRST;
	    my @buffer_list;
	    my @driver_list;

	    while (my $buffers = self!next-driver-buffers($whence))
	    {
		@buffer_list.append($buffers);
		$whence = ODBC::SQL::FETCH_NEXT
	    }

	    $whence = ODBC::SQL::FETCH_FIRST;

	    for @buffer_list -> $buffers
	    {
		@driver_list.append(self!next-driver($whence, $buffers));
		$whence = ODBC::SQL::FETCH_NEXT
	    }

	    return @driver_list
	}
    }

    class Connection is Handle
    {
	method new(Environment $env)
	{
	    my $self = self.bless(:handleType(ODBC::SQL::DBC));
	    $.validateSqlReturn(ODBC::SQL::AllocHandle(ODBC::SQL::HANDLE::DBC, $env.handle, $self.handle));

	    return $self
	}

	method getStringInfo(UInt $infoType)
	{
	    my Buf   $versionString .= new;
	    my int16 $versionStringLen;

	    $.validateSqlReturn(ODBC::SQL::GetInfo($.handle, $infoType, Pointer, 0, $versionStringLen));
	    $versionString.reallocate($versionStringLen + 1);

	    $.validateSqlReturn(ODBC::SQL::GetInfo($.handle, $infoType, nativecast(Pointer, $versionString), $versionString.elems, $versionStringLen));
	    $versionString.reallocate($versionString.elems - 1);

	    return decodeNative($versionString);
	}

	method version
	{
	    return $.getStringInfo(ODBC::SQL::SQL_ODBC_VER);
	}
    }
}

# vi:ft=perl6:ts=8:sw=4
