use v6;
use lib 'lib';
use X::ODBC;
use ODBC;
use Test;

plan 7;

ok(my ODBC::Environment $env .= new);
ok(my ODBC::Connection  $con .= new($env));

ok(my $odbcVer = $con.version);
say "ODBC version: {$odbcVer}";

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
