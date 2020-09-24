[![Build status](https://ci.appveyor.com/api/projects/status/n558pyl787ymf3bd/branch/master?svg=true)](https://ci.appveyor.com/project/terminatorul/perl-odbc/branch/master)
[![Build status](https://travis-ci.org/terminatorul/perl-ODBC.svg?branch=master)](https://travis-ci.org/terminatorul/perl-ODBC)

# perl-ODBC
Perl 6 module for listing available ODBC database drivers. Tested for Windows, Linux and MacOS.

Only available functions are:
 - list installed drivers and driver attributes
 - show available ODBC version
 
The module will load iodbc library on MacOS, unixodbc on Linux and ODBC32 dll on Windows.

## Installation
Clone the repository and manually copy the module to Perl 6 modules directory, since there is no module distribution archive packaged or uploaded to the Perl6 ecosystem for this module.

## Notification messages
`ODBC` module will send notifications for informational SQLState values (error class '01000') reported by the Driver Manager. These notifications are exception classes of the form `X::ODBC::SQLInfo[$sqlcode]`, that inherit from the standard `CX::Warn` class.

The intended use is for the application to handle these notifications in a Perl 6 `CONTROL` phaser, that would maybe display the message to the console and then `.resume` the exception. With the current issues in Perl 6 language implementation, the `CONTROL` phaser must also handle the standard `CX::Warn` exception in a similar fasion, for example:

```perl6
use X::ODBC;

# ... call ODBC methods

CONTROL
{
    when (X::ODBC::SqlInfo['01001'])
    {
        note .message
        .resume
    }

    when (CX::Warn)
    {
        note .message
        .resume
    }
}
```

This is entirely similar to a `CATCH` phaser for actual exceptions (that are not intended to be `.resume`d and do not inherit from `CX::Warn` or from `X::Control`). SQLState exceptions from `ODBC` module have the form `X::ODBC::SQLState['07001']`. Both `SQLInfo` and `SQLState` inherit from `X::ODBC::SQLNotification`.

Alternatively, set `&ODBC::notificationHandle` to any routine with one argument, to directly handle the notifications without the `CONTROL` phaser above, for example:
```perl6

$ODBC::notificationHandler = sub (X::ODBC::SQLNotification $notification)
{
    note $notification.message;
}
```

Most `SQLInfo` and `SQLState` exceptions also have a descriptive name, for example `SQLState['07002']` is `ColumnCountMismatch`, and `SQLState['07005']` is `CursorExpected`.

There is another class for exceptions that are not related to SQL statements: `X::ODBC::SqlReturn[$code]`, for example: `SqlReturn[ODBC::SQL::INVALID_HANDLE]`, which also has the descriptive name `X::ODBC::InvalidHandle`.

Here is the parent class tree for `X::ODBC` exceptions:

```txt
SqlReturn[$retcode]
    Error
        Exception

SQLState[$sqlstate]
    SQLNotification
        Exception

SQLInfo[$sqlstate]
    SQLNotification
        Exception
    CX::Warn
        X::Control
            Exception
```

## Using ANSI/Unicode ODBC functions

Currently the module is using the ANSI functions by default, until auto-detection based on the version string can be implemented. Unicode functions require ODBC v3.50 or later. You can select which API functions to be used by setting the `$ODBC::unicode` variable to a `True` value for Unicode functions and `False` value for ANSI functions. This may potentially have an effect on performance, since the optimum case is when ODBC Driver Manager is connecting:

    - Unicode applications with Unicode drivers
    - ANSI applications with ANSI drivers.

However, for Windows:
 - ANSI functions require extra conversions in the ODBC module, based on WinAPI function `MultiByteToWideChar()`
 - in case of ANSI applications connected to ANSI drivers, a few conversions are still neede in the ODBC Driver Manager also.
 
For MacOS X:
 - Unicode functions require extra conversions in the ODBC module, based on the C run-time library function `wctomb`.
 - Unicode functions will change the current locale for the C library to the user-defined locale `""`. This locale setting is global and will affect the entire process.

In general the recommended use is to set:
```perl6
    $ODBC::unicode = True;
```
during startup.

## Reading ODBC version string
```perl6
use ODBC;

my ODBC::Environment $env .= new;
my ODBC::Connection  $con .= new($env);

say $con.version;
```

##  Listing installed ODBC drivers

```perl6

use ODBC;

my ODBC::Environment $env .= new;
say .key for $env.drivers:
```
Drivers are provided as a hash of hashes, with the registered driver name as the first-level key, and possible driver attributes as the nested (second-level) key. A `driverList()` method is also available to provided the drivers and attributes as a list of pairs holding lists, in the order enumerated by the ODBC API.
