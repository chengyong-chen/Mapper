// GetFontFile.cpp
//
// Copyright (C) 2001 Hans Dietrich
//
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GetFontFile.h"
#include <string.h>
#include <string>

#include "../Public/EndianSwap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SWAPWORD(x)	MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)	MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

CStringA GetFontType(LPCTSTR lpszFilePath)
{
	CFile file;
	if(file.Open(lpszFilePath, CFile::modeRead|CFile::shareDenyWrite))
	{
		TTF_OFFSET_TABLE ttOffsetTable;
		file.Read(&ttOffsetTable, sizeof(TTF_OFFSET_TABLE));
		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if(ttOffsetTable.uMajorVersion == 1 && ttOffsetTable.uMinorVersion == 0)
		{
			file.Close();
			return "TTF";
		}
		else
		{
			file.SeekToBegin();
			TTC_HEADER TTCHeader;
			memset(&TTCHeader, 0, sizeof(TTC_HEADER));
			file.Read(&TTCHeader, sizeof(TTC_HEADER));
			file.Close();

			std::string strTag = "";
			strTag += TTCHeader.tag[0];
			strTag += TTCHeader.tag[1];
			strTag += TTCHeader.tag[2];
			strTag += TTCHeader.tag[3];
			if(strTag == "ttcf")
				return "TTC";
			else
				return "";
		}
	}
	else
		return "";
}

int GetTTCFontCount(LPCTSTR lpszFilePath)
{
	if(GetFontType(lpszFilePath)=="TTC")
	{
		CFile file;
		if(file.Open(lpszFilePath, CFile::modeRead|CFile::shareDenyWrite))
		{
			TTC_HEADER TTCHeader;
			memset(&TTCHeader, 0, sizeof(TTC_HEADER));
			file.Read(&TTCHeader, sizeof(TTC_HEADER));
			EndianSwap::Switch(TTCHeader.numFonts);
			file.Close();
			return TTCHeader.numFonts;
		}
		else
			return 0;
	}
	else
		return 0;
}

CStringA GetTTCFontName(LPCTSTR lpszFilePath, int index)
{
	FONT_PROPERTIES FontProps;
	GetTTCFontProperties(lpszFilePath, FontProps, index);
	return FontProps.realName;
}

BOOL GetTTFFontProperties(LPCTSTR lpszFilePath, FONT_PROPERTIES& FontProps)
{
	CFile file;
	if(file.Open(lpszFilePath, CFile::modeRead|CFile::shareDenyWrite))
	{
		const BOOL ret = GetTTFontProperties(file, FontProps);
		file.Close();
		return ret;
	}
	else
		return FALSE;
}

BOOL GetTTCFontProperties(LPCTSTR lpszFilePath, FONT_PROPERTIES& FontProps, int index)
{
	const int count = GetTTCFontCount(lpszFilePath);
	if(count==0)
		return FALSE;
	if(index>=count)
		return FALSE;

	CFile file;
	if(file.Open(lpszFilePath, CFile::modeRead|CFile::shareDenyWrite))
	{
		file.Seek(sizeof(TTC_HEADER), CFile::begin);
		file.Seek(index * sizeof(UINT), CFile::current);
		UINT nOffset = 0;
		file.Read(&nOffset, sizeof(UINT));
		EndianSwap::Switch(nOffset);
		file.Seek(nOffset, SEEK_SET);
		const BOOL ret = GetTTFontProperties(file, FontProps);
		file.Close();
		return ret;
	}
	else
		return FALSE;
}

BOOL GetTTFontProperties(CFile& file, FONT_PROPERTIES& FontProps)
{
	if(file.m_hFile == nullptr)
		return FALSE;

	const DWORD language1 = GetSystemDefaultUILanguage();	//	区域选项(location)       display 语言         高级(non=unicode)
	const DWORD language2 = GetUserDefaultUILanguage();		//		改变				       改变					不变
	const DWORD language3 = GetSystemDefaultLangID();		//		不变					   不变					改变
	const DWORD language4 = GetUserDefaultLangID();			//		改变					   不变					不变 

	TTF_OFFSET_TABLE ttOffsetTable;
	file.Read(&ttOffsetTable, sizeof(TTF_OFFSET_TABLE));
	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	//check is this is a true type font and the version is 1.0
	if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		return FALSE;

	for(int i = 0; i < ttOffsetTable.uNumOfTables; i++)
	{
		TTF_TABLE_DIRECTORY tblDir;
		file.Read(&tblDir, sizeof(TTF_TABLE_DIRECTORY));
		tblDir.szTag[4] = '\0';
		tblDir.uLength = SWAPLONG(tblDir.uLength);
		tblDir.uOffset = SWAPLONG(tblDir.uOffset);
		if(strcmp(tblDir.szTag, "name") == 0)
		{
			file.Seek(tblDir.uOffset, CFile::begin);
			TTF_NAME_TABLE_HEADER ttNTHeader;
			file.Read(&ttNTHeader, sizeof(TTF_NAME_TABLE_HEADER));
			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);

			for(int i = 0; i < min(ttNTHeader.uNRCount, 100); i++)
			{
				TTF_NAME_RECORD ttRecord;
				file.Read(&ttRecord, sizeof(TTF_NAME_RECORD));
				ttRecord.uPlatformId = SWAPWORD(ttRecord.uPlatformId);
				ttRecord.uEncodingId = SWAPWORD(ttRecord.uEncodingId);
				ttRecord.uNameId = SWAPWORD(ttRecord.uNameId);
				ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
				ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);
				ttRecord.uLanguageId = SWAPWORD(ttRecord.uLanguageId);
				if(ttRecord.uStringLength == 0)
					continue;
				const int nPos = file.GetPosition();
				file.Seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, CFile::begin);

				BYTE* buf = new BYTE[ttRecord.uStringLength + 2];
				ZeroMemory(buf, ttRecord.uStringLength + 2);
				file.Read(buf, ttRecord.uStringLength);
				buf[ttRecord.uStringLength] = '\0';
				CString strName;
				strName = buf;
				if(ttRecord.uPlatformId == 3)//microsoft
				{
					switch(ttRecord.uEncodingId)
					{
						case 1: //unicode BMP(UCS-2)
						case 3: //PRC
						case 4: //BIG5
						case 10: //unicode BMP(UCS-2)
							for(int index = 0; index < ttRecord.uStringLength / 2; index++)
							{
								const BYTE c = buf[index * 2];
								buf[index * 2] = buf[index * 2 + 1];
								buf[index * 2 + 1] = c;
							}
							strName = (WCHAR*)buf;
					}
				}
				delete[] buf;
				if(strName != _T(""))
				{
					switch(ttRecord.uNameId)
					{
						case 0:
							if(FontProps.csCopyright.IsEmpty())
								FontProps.csCopyright = strName;
							break;
						case 1:
							if(FontProps.csFamily.IsEmpty())
								FontProps.csFamily = strName;
							break;
						case 4: //Full name
							//ttRecord.uLanguageId
							//1041://Japanese 
							//1042://Korean 
							//2052://Simplified Chinese
							//1028://Traditional Chinese						
							if(ttRecord.uLanguageId == language1)
								FontProps.faceName = strName;
							else if(ttRecord.uLanguageId == language2)
								FontProps.faceName = strName;
							else if(ttRecord.uLanguageId == language3)
								FontProps.faceName = strName;
							else if(ttRecord.uLanguageId == language4)
								FontProps.faceName = strName;
							else if(FontProps.faceName.IsEmpty() == TRUE)
								FontProps.faceName = strName;
							break;
						case 6: //PostScript 
							if(FontProps.realName.IsEmpty())
								FontProps.realName = strName;
							break;
						case 7:
							if(FontProps.csTrademark.IsEmpty())
								FontProps.csTrademark = strName;
							break;
						case 20: //PostScript CId findfont name
							break;
						default:
							break;
					}
				}

				file.Seek(nPos, CFile::begin);
			}
		}
		else if(strcmp(tblDir.szTag, "glyf") == 0)
		{
		}
		else if(strcmp(tblDir.szTag, "loca") == 0)
		{
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFontEnglishName
//
// Note:  This is *not* a foolproof method for finding the name of a font file.
//        If a font has been installed in a normal manner, and if it is in
//        the Windows "Font" directory, then this method will probably work.
//        It will probably work for most screen fonts and TrueType fonts.
//        However, this method might not work for fonts that are created 
//        or installed dynamically, or that are specific to a particular
//        device, or that are not installed into the font directory.
