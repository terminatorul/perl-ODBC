use v6;
use ODBC::SQL;
use X::ODBC;

class ODBC::Connection is ODBC::Handle
{
    method handleType
    {
	SQL::DBC
    }

    method new(ODBC::Environment $env)
    {
	my $self = self.bless(:handleType(SQL::DBC));
	$.validateSqlReturn($.SQLAllocHandle.($.handleType, $env.handle, $self.handle));

	return $self
    }

    method getStringInfo(UInt $infoType)
    {
	my Buf   $versionString .= new;
	my int16 $versionStringLen;

	$.validateSqlReturn($SQLGetInfo.($.handle, $infoType, Pointer, 0, $versionStringLen));
	$versionString.reallocate($versionStringLen + 1);

	$.validateSqlReturn($.SQLGetInfo.($.handle, $infoType, nativecast(Pointer, $versionString), $versionString.elems, $versionStringLen));
	$versionString.reallocate($versionString.elems - 1);

	return decodeNative($versionString);
    }

    method version
    {
	return $.getStringInfo(SQL::SQL_ODBC_VER);
    }
}

# vi:ft=perl6
