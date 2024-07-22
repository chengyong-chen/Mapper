#pragma once

// Define constants and enums that can safely be included in mixed mode C++ classes, 
// without pulling a lot of other C++ classes.
namespace AGR
{
	enum AGR_RDBMS
	{
		SQLSERVER,
		ORACLE
	};

	enum EValueTyp
	{
		eLong,
		eDouble,
		eBool,
		eCString,
		eTCounter,
		eTDimValue,
		eTAttId,
		eTDate,
		eTCHAR,
		eNULL,
		eVoid,
		eGuid,
		eTGuid,
		eTBlob
	};

	enum DatabaseType
	{
		Other,
		Oracle,
		MSSql
	};

	//Driver types (matches managed enum DriverType in Driver.Database)
	enum DatabaseDriverType
	{
		//Unknown
		Unknown = 0,
		//Implemented in managed code using the managed driver API
		Managed = 1,
		//Implemented in native code using the client driver API
		NativeClient = 2,
		//Implemented in native code using the server driver API
		NativeServer = 3
	};
}

// Move to the AGR namespace in the future.
enum AGR_StopType
{
	STOPTYPE_UNINITIALIZED = 0,
	NO_ROWS = 2,
	FUNC_ERROR = 3,
	TECH_ERROR = 4
};

enum AGR_ExceptionStatus
{
	STATUS_UNINITIALIZED,
	ERROR_PROCESSED,
	ERROR_NOT_PROCESSED,
	ERROR_DB_NOCONNECTION,
	ERROR_DB_CONFIG
};
