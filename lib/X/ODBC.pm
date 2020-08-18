use v6;
use ODBC::SQL;

module X::ODBC
{
    role Error is Exception
    {
	has Int $.code;

	method message()
	{
	    given $.code
	    {
		when ODBC::SQL::INVALID_HANDLE		{ return 'Invalid handle' }
		when ODBC::SQL::ERROR	    	    	{ return 'SQL error' }
		when ODBC::SQL::SUCCESS		    	{ return '' }
		when ODBC::SQL::SUCCESS_WITH_INFO   	{ return 'Notice' }
		when ODBC::SQL::STILL_EXECUTING	    	{ return 'Still executing' }
		when ODBC::SQL::NEED_DATA	    	{ return 'Need data' }
		when ODBC::SQL::NO_DATA		    	{ return 'No data' }
		when ODBC::SQL::PARAM_DATA_AVAILABLE	{ return 'Param data available' }
		default					{ return 'ODBC Error' }
	    }

	    return 'ODBC Error'
	}
    }

    role SqlReturn[Int $code] is Error
    {
	method new
	{
	    return self.bless(:code($code));
	}
    }

    role SQLNotification is Exception
    {
	has Str $.code;
	has Int $.nativeCode;
	has Str $.message;

	method errorClass
	{
	    return $.code.substr(0, 2);
	}

	method errorSubclass
	{
	    return $.code.substr(2);
	}
    }

    role SQLState[Str $code] does SQLNotification
    {
	method new(:$nativeCode, :$message)
	{
	    return self.bless(:$code, :$nativeCode, :$message)
	}
    }

    role SQLInfo[Str $code] is CX::Warn does SQLNotification
    {
	method new(:$nativeCode, :$message)
	{
	    return self.bless(:$code, :$nativeCode, :$message)
	}
    }

    # SQLRETURN codes
    constant InvalidHandle	 := SqlReturn[ODBC::SQL::INVALID_HANDLE];
    constant SqlError	    	 := SqlReturn[ODBC::SQL::ERROR];
    constant Success	    	 := SqlReturn[ODBC::SQL::SUCCESS];
    constant SuccessWithInfo	 := SqlReturn[ODBC::SQL::SUCCESS_WITH_INFO];
    constant StillExecuting	 := SqlReturn[ODBC::SQL::STILL_EXECUTING];
    constant NeedData		 := SqlReturn[ODBC::SQL::NEED_DATA];
    constant NoData		 := SqlReturn[ODBC::SQL::NO_DATA];
    constant ParamDataAvailable  := SqlReturn[ODBC::SQL::PARAM_DATA_AVAILABLE];

    # SQLSTATE Info codes
    constant Warning			:= SQLInfo['01000'];
    constant CursorOperationConflict	:= SQLInfo['01001'];
    constant DisconnectError		:= SQLInfo['01002'];
    constant NullValueEliminated    	:= SQLInfo['01003'];
    constant StringTruncated	    	:= SQLInfo['01004'];
    constant TooManyDescriptors	    	:= SQLInfo['01005'];
    constant PrivilegeNotFound	    	:= SQLInfo['01006'];
    constant PrivilegeNotGrantable  	:= SQLInfo['01007'];
    constant InvalidAttribute	    	:= SQLInfo['01S00'];
    constant EncounteredRowError    	:= SQLInfo['01S01'];
    constant OptionChanged	    	:= SQLInfo['01S02'];
    constant FetchBeforeRowset	    	:= SQLInfo['01S06'];
    constant FractionalTruncation   	:= SQLInfo['01S07'];
    constant FileDsnNotSaved	    	:= SQLInfo['01S08'];
    constant InvalidKeyword	    	:= SQLInfo['01S09'];

    #SQLSTATE error codes
    constant ParamCountMismatch		:= SQLState['07001'];
    constant ColumnCountMismatch    	:= SQLState['07002'];
    constant CursorExpected	    	:= SQLState['07005'];
    constant AttributeTypeMismatch  	:= SQLState['07006'];
    constant InvalidDescriptorIndex 	:= SQLState['07007'];
    constant InvalidDefaultParamterUse	:= SQLState['07S01'];
    constant ConnectFailed              := SQLState['08001'];
    constant ConnectionNameIsUse	:= SQLState['08002'];
    constant ConnectionNotOpen		:= SQLState['08003'];
    constant ConnectionRejected		:= SQLState['08004'];
    constant ConnectionFailed		:= SQLState['08007'];
    constant CommunicationFailure	:= SQLState['08S01'];
    constant InsertListMismatch		:= SQLState['21S01'];
    constant ColumnListMismatch		:= SQLState['21S02'];
    constant StringDataTruncated	:= SQLState['22001'];
    constant IndicatorVariableRequired  := SQLState['22002'];
    constant NumericValueOutOfRange	:= SQLState['22003'];
    constant InvalidDateTimeFormat	:= SQLState['22007'];
    constant DateTimeFieldOverflow	:= SQLState['22008'];
    constant DivisionByZero		:= SQLState['22012'];
    constant IntervalFieldOverflow	:= SQLState['22015'];
    constant InvalidCharacterValue	:= SQLState['22018'];
    constant InvalidEscapeCharacter	:= SQLState['22019'];
    constant InvalidEscapeSequence	:= SQLState['22025'];
    constant StringLengthMismatch	:= SQLState['22026'];
    constant ConstraintViolation	:= SQLState['23000'];
    constant InvalidCursorState		:= SQLState['24000'];
    constant InvalidTransactionState	:= SQLState['25000'];

    constant SyntaxErrOrAccessViolation := SQLState['42000'];
    constant TableOrViewAlreayExists	:= SQLState['42S01'];
    constant TableOrViewNotFound	:= SQLState['42S02'];
    constant IndexAlreadyExists		:= SQLState['42S11'];
    constant IndexNotFound		:= SQLState['42S12'];
    constant ColumnAlreadyExists	:= SQLState['42S21'];
    constant ColumnNotFound		:= SQLState['42S22'];
    constant CheckOptionViolation	:= SQLState['44000'];

    constant GeneralError		:= SQLState['HY000'];
    constant AllocationError		:= SQLState['HY001'];
    constant InvalidBufferType		:= SQLState['HY003'];
    constant InvalidSqlDataType		:= SQLState['HY004'];
    constant StatementNotPrepared	:= SQLState['HY007'];
    constant OperationCanceled		:= SQLState['HY008'];

    constant FunctionTypeOutOfRange	:= SQLState['HY095'];
    constant InvalidInformationType	:= SQLState['HY096'];
    constant ColumnTypeOutOfRange	:= SQLState['HY097'];
    constant ScopeTypeOutOfRange	:= SQLState['HY098'];
    constant NullableTypeOutOfRange	:= SQLState['HY099'];
    constant UniquenessTypeOutOfRange	:= SQLState['HY100'];
    constant AccuracyTypeOutOfRange	:= SQLState['HY101'];
    constant InvalidRetrievalCode	:= SQLState['HY103'];
    constant InvalidPrecisionOrScale	:= SQLState['HY104'];
    constant InvalidParameterType	:= SQLState['HY105'];
    constant FetchTypeOutForRange	:= SQLState['HY106'];
    constant RowValueOutOfRange		:= SQLState['HY107'];
    constant InvalidCursorPosition	:= SQLState['HY109'];
    constant InvalidDriverCompletion	:= SQLState['HY110'];
    constant InvalidBookmark		:= SQLState['HY111'];
    constant NotImplemented		:= SQLState['HYC00'];
    constant TimeoutExpired		:= SQLState['HYT00'];
    constant ConnectionTimeoutExpired	:= SQLState['HYT01'];

    constant FunctionNotSupported	:= SQLState['IM001'];
    constant NoDataSourceOrDriver	:= SQLState['IM002'];
    constant DriverLoadingFailed	:= SQLState['IM003'];
    constant DriverEnvAllocFailed	:= SQLState['IM004'];
    constant DriverConnAllocFailed	:= SQLState['IM005'];
    constant DriverSetAttrFaild		:= SQLState['IM006'];
    constant DialogWithNoDsnOrDriver	:= SQLState['IM007'];
    constant DialgFailed		:= SQLState['IM008'];
    constant LoadingTranslationFailed	:= SQLState['IM009'];
    constant DataSourceNameTooLong	:= SQLState['IM010'];
    constant DriverNameTooLong		:= SQLState['IM011'];
    constant DriverKeywordSyntaxError	:= SQLState['IM012'];
    constant TraceFileError		:= SQLState['IM013'];
    constant InvalidNameOrFileDSN	:= SQLState['IM014'];
    constant CorruptFileDataSource	:= SQLState['IM015'];
}

# vi:ft=perl6:ts=8:sw=4
