// GetFontFile.h

// Copyright (C) 2001 Hans Dietrich

// This software is released into the public domain.  
// You are free to use it in any way you like.

// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 

///////////////////////////////////////////////////////////////////////////////

#pragma once

typedef struct _tagFONT_PROPERTIES
{
	CStringA realName;
	CString faceName;
	CString csCopyright;
	CString csTrademark;
	CString csFamily;
} FONT_PROPERTIES,*LPFONT_PROPERTIES;

typedef struct _tagTTF_OFFSET_TABLE
{
	USHORT uMajorVersion;
	USHORT uMinorVersion;
	USHORT uNumOfTables;
	USHORT uSearchRange;
	USHORT uEntrySelector;
	USHORT uRangeShift;
} TTF_OFFSET_TABLE;

typedef struct _tagTTF_TABLE_DIRECTORY
{
	char szTag[4]; //table name
	ULONG uCheckSum; //Check sum
	ULONG uOffset; //Offset from beginning of file
	ULONG uLength; //length of the table in bytes
} TTF_TABLE_DIRECTORY;

typedef struct _tagTTF_NAME_TABLE_HEADER
{
	USHORT uFSelector; //format selector. Always 0
	USHORT uNRCount; //Name Records count
	USHORT uStorageOffset; //Offset for strings storage, from start of the table
} TTF_NAME_TABLE_HEADER;

typedef struct _tagTTF_NAME_RECORD
{
	USHORT uPlatformId;
	USHORT uEncodingId;
	USHORT uLanguageId;
	USHORT uNameId;
	USHORT uStringLength;
	USHORT uStringOffset; //from start of storage area
} TTF_NAME_RECORD;

typedef struct _tagTTC_HEADER
{
	CHAR tag[4];
	UINT Version;
	ULONG numFonts;
} TTC_HEADER;

__declspec(dllexport) CStringA GetFontType(LPCTSTR lpszFilePath);
__declspec(dllexport) int GetTTCFontCount(LPCTSTR lpszFilePath);
__declspec(dllexport) CStringA GetTTCFontName(LPCTSTR lpszFilePath, int index);

__declspec(dllexport) BOOL GetTTFFontProperties(LPCTSTR lpszFilePath, FONT_PROPERTIES& FontProps);
__declspec(dllexport) BOOL GetTTCFontProperties(LPCTSTR lpszFilePath, FONT_PROPERTIES& FontProps, int index);
__declspec(dllexport) BOOL GetTTFontProperties(CFile& file, FONT_PROPERTIES& FontProps);
