use v6;

use NativeCall;
use ODBC::SQL;
use ODBC::Handle;

class ODBC::Environment::Impl:api<1>
{
    has $sql-impl handles @ODBC::SQL::sql-methods;

    method new($impl is rw)
    {
	$self = self.bless;
	$.impl := $impl
    }
}

class ODBC::Environment::Impl:api<3> is ODBC::Environment::Impl:api<1> is ODBC::Handle::Impl:api<3>
{
    method handleType()
    {
	SQL::ENV
    }

}

role Environment[@odbc-libs, Pair :A, Pair :W] is Handle
{
    method new(SQL::ODBC_VERSION $odbcVersion = SQL::OV_ODBC3_80, Bool :$A, Bool :$W)
    {
	my $self = self.bless;
	$.validateSqlReturn($.SQLAllocHandle.($.handleType, Pointer, $self.handle));
	$.validateSqlReturn($.SQLSetEnvAttr.($self.handle, SQL::ATTR_ODBC_VERSION, Pointer[uint32].new($odbcVersion), nativesizeof(uint32)));

	return $self
    }

    method !next-driver-buffers(SQL::FETCH $whence)
    {
	my int16 $descLen = 0;
	my int16 $attrLen = 0;

	{
	    if $unicode
	    {
		$.validateSqlReturn(SQL::DriversW($.handle, $whence, CArray[uint16], 0, $descLen, CArray[uint16], 0, $attrLen));
	    }
	    else
	    {
		$.validateSqlReturn(SQL::Drivers($.handle, $whence, CArray[uint8],  0, $descLen, CArray[uint8],  0, $attrLen));
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

    method !next-driver(SQL::FETCH $whence, $buffers)
    {
	my int16 $descLen = 0;
	my int16 $attrLen = 0;

	if $unicode
	{
	    my Buf[uint16] $descBuff = @$buffers[0];
	    my Buf[uint16] $attrBuff = @$buffers[1];

	    $.validateSqlReturn($.SQLDriversW.($.handle, $whence, nativecast(CArray[uint16], $descBuff), $descBuff.elems, $descLen, nativecast(CArray[uint16], $attrBuff), $attrBuff.elems, $attrLen));

	    return $descBuff.decode('utf-16') => %( map { my @pair = $_.split('='); @pair[1] //= True; pair(|@pair) }, grep { $_ }, $attrBuff.decode('utf-16').split("\0") );
	}
	else
	{
	    my Buf $descBuff = @$buffers[0];
	    my Buf $attrBuff = @$buffers[1];

	    $.validateSqlReturn($.SQLDrivers.($.handle, $whence, nativecast(CArray[uint8], $descBuff), $descBuff.elems, $descLen, nativecast(CArray[uint8], $attrBuff), $attrBuff.elems, $attrLen));

	    return decodeNative($descBuff) => %( map { my @pair = $_.split('='); @pair[1] //= True; pair(|@pair) }, grep { $_ }, decodeNative($attrBuff).split("\0") );
	}
    }

    method drivers
    {
	my Int $whence = SQL::FETCH_FIRST;
	my @buffer_list;
	my @driver_list;

	while (my $buffers = self!next-driver-buffers($whence))
	{
	    @buffer_list.append($buffers);
	    $whence = SQL::FETCH_NEXT
	}

	$whence = SQL::FETCH_FIRST;

	for @buffer_list -> $buffers
	{
	    @driver_list.append(self!next-driver($whence, $buffers));
	    $whence = SQL::FETCH_NEXT
	}

	return @driver_list
    }
}

# vi:ft=perl6

