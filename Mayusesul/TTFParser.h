#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include "MemPtrReader.h"
class TTFParser
{
public:
	TTFParser(void);
	~TTFParser(void);
	struct stOffsetTable
	{
		UINT version;
		USHORT numTables;
		USHORT searchRange;
		USHORT entrySelector;
		USHORT rangeShift;
	};
	struct stTableDirectory
	{
		CHAR tag[4];
		ULONG checkSum;
		ULONG offset;
		ULONG length;
	};
	struct stNamingTable
	{
		USHORT FormatSelector;
		USHORT NumberOfNameRecords;
		USHORT OffsetStartOfStringStorage;
	};
	struct stNameRecord
	{
		USHORT PlatformID;
		USHORT EncodingID;
		USHORT LanguageID;
		USHORT NameID;
		USHORT StringLength;
		USHORT OffsetFromStorageArea;
	};
	bool Parse(const std::wstring& szFilePath);
	bool Parse(FILE* pFile, size_t nOffsetFromStart);
	bool Parse(BYTE* pFile, size_t size);
	bool Parse(BYTE* pFile, size_t size, size_t offset);
	void Clear();

	std::wstring GetFontName() const
	{
		return m_szFontName;
	};

	std::wstring GetCopyright() const
	{
		return m_szCopyright;
	};

	std::wstring GetFontFamilyName() const
	{
		return m_szFontFamilyName;
	};

	std::wstring GetFontSubFamilyName() const
	{
		return m_szFontSubFamilyName;
	};

	std::wstring GetFontID() const
	{
		return m_szFontID;
	};

	std::wstring GetVersion() const
	{
		return m_szVersion;
	};

	std::wstring GetPostScriptName() const
	{
		return m_szPostScriptName;
	};

	std::wstring GetTrademark() const
	{
		return m_szTrademark;
	};
	bool IsBold() const;
	bool IsItalic() const;
	bool IsRegular() const;
	static std::string WideToNarrow(const std::wstring& szWide);
	static std::wstring NarrowToWide(const std::string& szNarrow);
private:
	bool ParseOffsetTable();
	bool ParseOffsetTableMem();
	bool ParseName(stTableDirectory& TableDirectory);
	bool ParseNameMem(stTableDirectory& TableDirectory);
	void ParseStyles();
	static std::wstring GetNameID(USHORT uNameID);
	void SetNameIDValue(USHORT uNameID, const std::wstring& str);
	void SetNameIDValue(USHORT uNameID, const std::string& str);
	static std::wstring ToLower(const std::wstring& szNormal);
	static std::string ToLower(const std::string& szNormal);
	std::FILE* m_pFile;
	std::wstring m_szFilePath;
	std::wstring m_szFontName;
	std::wstring m_szCopyright;
	std::wstring m_szFontFamilyName;
	std::wstring m_szFontSubFamilyName;
	std::wstring m_szFontID;
	std::wstring m_szVersion;
	std::wstring m_szPostScriptName;
	std::wstring m_szTrademark;
	BYTE m_nInfo;
	bool m_bExternalFilePtr;
	MemPtrReader* m_pMemPtrReader;
	bool m_bBold;
	bool m_bRegular;
	bool m_bItalic;
};
