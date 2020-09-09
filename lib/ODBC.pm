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

    sub decodeNativeWin(Buf $arr) returns Str
    {
	decodeNativeString(nativecast(CArray[uint8], $arr), $arr.elems)
    }

    sub decodeNativeUnix(Buf $arr) returns Str
    {
	$arr.decode
    }

    sub wcsrtombs(CArray[uint8] $dst, CArray[int32] $src is rw , uint64 $len, CArray[uint8] $ps) returns uint64 is native
    {
	*
    }

    sub wcrtomb(CArray[uint8] $pmb, uint32 $wc, CArray[uint8] $ps) returns uint64 is native
    {
	*
    }

    sub setlocale(int32 $lc-category, CArray[uint8] $locale) returns Pointer is native
    {
	*
    }

    constant SQLWCHAR 		 = ODBC::SQL::SQLWCHAR;
    constant LC_CTYPE 		 = 2;
    constant MULTIBYTE_CHAR_SIZE = 6;
    constant MBSTATE_NATIVE_SIZE = 128;
    constant INVALID_STRING_SIZE = 0xFFFFFFFF_FFFFFFFF;

    my Buf[uint8] $mbstate	   .= allocate(MBSTATE_NATIVE_SIZE);
    my Buf[uint8] $multi-byte-char .= allocate(MULTIBYTE_CHAR_SIZE);

    sub decodeWideNativeMac(Blob[SQLWCHAR] $arr) returns Str
    {
	state $old-locale = setlocale(LC_CTYPE, CArray[uint8].new(0));

	my Buf[uint8] $string .= new;
	my uint64 $multi-byte-size;

	$_ = 0 for @$mbstate;

	for @$arr -> $wc
	{
	    $multi-byte-size = wcrtomb(nativecast(CArray[uint8], $multi-byte-char), $wc, nativecast(CArray[uint8], $mbstate));

	    if $multi-byte-size != INVALID_STRING_SIZE
	    {
		$string.append($multi-byte-char[^$multi-byte-size])
	    }
	    else
	    {
		X::EncodingError('Illegal character or character sequence').throw;
		last
	    }
	}

	$multi-byte-size = wcrtomb(nativecast(CArray[uint8], $multi-byte-char), 0, nativecast(CArray[uint8], $mbstate));

	if $multi-byte-size != INVALID_STRING_SIZE
	{
	    $string.append($multi-byte-char[^($multi-byte-size - 1)])
	}
	else
	{
	    X::EncodingError('Illegal character or character sequence').throw;
	}


	$string.decode
    }

    sub decodeWideNativeXOpen(Blob[SQLWCHAR] $arr) returns Str
    {
	$arr.decode('utf16')
    }

    my constant &decodeNative     = $*DISTRO.is-win	       ?? &decodeNativeWin     !! &decodeNativeUnix;
    my constant &decodeWideNative = $*DISTRO.name ~~ /macosx?/ ?? &decodeWideNativeMac !! &decodeWideNativeXOpen;

    sub GetDiagRecA(ODBC::SQL::HANDLE:D $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
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

    sub GetDiagRecW(ODBC::SQL::HANDLE:D $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
    {
	my Buf[SQLWCHAR]	$sqlStateBuf .= allocate(6);
	my int32	$nativeCode   = 0;
	my int16	$messageLen   = 0;
	my int16	$sqlReturn    = ODBC::SQL::GetDiagRecW($handleType, $handle, $recordNumber, nativecast(CArray[SQLWCHAR], $sqlStateBuf), $nativeCode, CArray[SQLWCHAR], 0, $messageLen);

	given $sqlReturn
	{
	    when ODBC::SQL::SUCCESS | ODBC::SQL::SUCCESS_WITH_INFO
	    {
		my Buf[SQLWCHAR] $messageBuf .= allocate($messageLen + 1);

		$sqlReturn = ODBC::SQL::GetDiagRecW($handleType, $handle, $recordNumber, nativecast(CArray[SQLWCHAR], $sqlStateBuf), $nativeCode, nativecast(CArray[SQLWCHAR], $messageBuf), $messageBuf.elems, $messageLen);

		if $sqlReturn == ODBC::SQL::SUCCESS
		{
		    $sqlStateBuf.reallocate($sqlStateBuf.elems - 1);
		    $messageBuf.reallocate($messageBuf.elems - 1);

		    $sqlState = $sqlStateBuf.decode('utf32');
		    $message  = $messageBuf.decode('utf32');
		    $nativeError = $nativeCode;
		}
	    }
	}

	return $sqlReturn;
    }

    our sub GetDiagRec(ODBC::SQL::HANDLE:D $handleType, Pointer $handle, UInt $recordNumber, Str $sqlState is rw, Int $nativeError is rw, Str $message is rw) returns Int
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
	method new(ODBC::SQL::ODBC_VERSION $odbcVersion = ODBC::SQL::OV_ODBC3)
	{
	    my $self = self.bless(:handleType(ODBC::SQL::ENV));
	    $.validateSqlReturn(ODBC::SQL::AllocHandle(ODBC::SQL::HANDLE::ENV, Pointer, $self.handle));
	    $.validateSqlReturn(ODBC::SQL::SetEnvAttr($self.handle, ODBC::SQL::ATTR_ODBC_VERSION, Pointer[SQLWCHAR].new($odbcVersion), nativesizeof(uint32)));

	    return $self
	}

	method !driver(ODBC::SQL::FETCH:D $whence, Buf $desc is rw, Buf $attr is rw)
	{
	    my int16 ($descLen, $attrLen);

	    if $unicode
	    {
		$.validateSqlReturn(ODBC::SQL::DriversW($.handle, $whence, nativecast(CArray[SQLWCHAR], $desc), $desc.elems, $descLen, nativecast(CArray[SQLWCHAR], $attr), $attr.elems, $attrLen))
	    }
	    else
	    {
		$.validateSqlReturn(ODBC::SQL::Drivers($.handle, $whence, nativecast(CArray[uint8], $desc), $desc.elems, $descLen, nativecast(CArray[uint8], $attr), $attr.elems, $attrLen))
	    }

	    CONTROL
	    {
		when X::ODBC::StringTruncated
		{
		    note .message;
		    .resume
		}
	    }

	    CATCH
	    {
		when X::ODBC::NoData
		{
		    return ()
		}
	    }

	    return $descLen, $attrLen;
	}

	method !parse-drivers(@buffer-list, @buffer-size-list)
	{
	    my @driver-list;

	    for ^@buffer-list.elems -> $i
	    {
		my $desc-subbuf = @buffer-list[$i][0].subbuf-rw(0, @buffer-size-list[$i][0]);
		my $attr-subbuf = @buffer-list[$i][1].subbuf-rw(0, @buffer-size-list[$i][1]);
		my ($desc, $attr);

		if $unicode
		{
		    $desc = decodeWideNative($desc-subbuf);
		    $attr = decodeWideNative($attr-subbuf);
		}
		else
		{
		    $desc = decodeNative($desc-subbuf);
		    $attr = decodeNative($attr-subbuf);
		}

		@driver-list.push($desc => map { my @pair = $_.split('=', 2); @pair[1] //= True; pair(|@pair) }, grep { $_ }, $attr.split("\0"))
	    }

	    return @driver-list;
	}

	method driverList()
	{
	    my Int:D $whence = ODBC::SQL::FETCH_FIRST;
	    my @buffer-list;
	    my @buffer-size-list;
	    my Buf[$unicode ?? SQLWCHAR !! uint8] $desc .= new;
	    my Buf[$unicode ?? SQLWCHAR !! uint8] $attr .= new;

	    while (my $buffer-sizes = self!driver($whence, $desc, $attr))
	    {
		@buffer-list.push([ map { Buf[$unicode ?? SQLWCHAR !! uint8].allocate($_ + 2) }, @$buffer-sizes ]);

		$whence = ODBC::SQL::FETCH_NEXT
	    }

	    my Bool:D $fit-buffer-size = True;

	    repeat
	    {
		$fit-buffer-size = True;
		@buffer-size-list = ();
		$whence = ODBC::SQL::FETCH_FIRST;

		for @buffer-list -> $buffers
		{
		    my $buffer-sizes = self!driver($whence, |@$buffers);

		    for ^$buffer-sizes.elems -> $i
		    {
			if $buffers[$i].elems <= $buffer-sizes[$i] + 1
			{
			    $buffers[$i].reallocate($buffers[$i].elems * 2 max $buffer-sizes[$i] + 2);
			    $fit-buffer-size = False
			}
		    }

		    @buffer-size-list.push($buffer-sizes);
		    $whence = ODBC::SQL::FETCH_NEXT
		}
	    }
	    until $fit-buffer-size;

	    return self!parse-drivers(@buffer-list, @buffer-size-list);
	}

	method drivers()
	{
	    my %drivers = %( |$.driverList );

	    for %drivers.keys -> $driver-name
	    {
		%drivers{$driver-name} = %( |%drivers{$driver-name} );
	    }

	    return %drivers
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


	    repeat
	    {
		$versionString.reallocate($versionStringLen + 1 max $versionString.elems * 2 max 8);
		$.validateSqlReturn(ODBC::SQL::GetInfo($.handle, $infoType, nativecast(Pointer, $versionString), $versionString.elems, $versionStringLen));
	    }
	    until ($versionString.elems > $versionStringLen + 1);

	    $versionString.reallocate($versionStringLen);

	    decodeNative($versionString)
	}

	method version
	{
	    $.getStringInfo(ODBC::SQL::SQL_ODBC_VER)
	}
    }
}

# vi:ft=perl6:ts=8:sw=4
