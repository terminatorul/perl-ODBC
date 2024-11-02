use v6;
use lib 'lib';
use X::ODBC;
use ODBC;
use Test;
use Xmod 'DBC2';
use Xmod 'DDD2';
use Xmod 'Delta';

role SQL[:$lib, :$A, :$W]
{
    method show-lib
    {
	note "Library name: {$lib}"
    }

    method show-char-width
    {
	note "Narrow: $A, wide: $W"
    }
}


class Impl
{
    method show
    {
	$.show-lib;
	$.show-char-width
    }
}

class D is Impl
{
    has $.slq handles * = SQL[:lib('ODBC32'), :W].new;
}

D.new.show;

DBC2;
DDD2;
Delta;

role DB[:$lib]
{
    has $.lib-name = $lib
}

say "Library name: ", DB[:lib('Xodbc')].new.lib-name;

plan 7;

ok(my ODBC::Environment $env .= new);
ok(my ODBC::Connection  $con .= new($env));

ok(my $odbcVer = $con.version);
say "ODBC version: {Version($odbcVer)}";

ok(my @drivers = $env.drivers);
.say for @drivers;
ok([?&] (.value{'Driver'} // .value{'UsageCount'} for @drivers));

$ODBC::unicode = True;
ok(@drivers = $env.drivers);

.say for @drivers;
ok([?&] (.value{'Driver'} // .value{'UsageCount'} for @drivers));

LEAVE
{
    .dispose with $con;
    .dispose with $env;

    done-testing
}

CONTROL
{
    when (X::ODBC::StringTruncated)
    {
	say .code, ' ', .message;
	flunk("Unexpected result " ~ .code ~ ' ' ~ .message);
	.resume
    }

    default
    {
	say .message;
	.resume
    }
}

# vi:ft=perl6:ts=8:sw=4
