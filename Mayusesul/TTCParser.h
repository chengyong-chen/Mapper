#pragma once
#include <vector>
#include <string>
#include "TTFParser.h"
class TTCParser
{
public:
	TTCParser(void);
	~TTCParser(void);
	struct stTTCHeader
	{
		CHAR tag[4];
		UINT Version;
		ULONG numFonts;
	};
	bool Parse(const std::wstring& szFilePath);
	bool Parse(BYTE* pFile, size_t size);
	bool Parse(BYTE* pFile, size_t size, size_t offset);

	size_t Size() const
	{
		return m_vecTtfPtr.size();
	}

	void Clear();

	std::wstring GetFontName(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetFontName();
	};

	std::wstring GetCopyright(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetCopyright();
	};

	std::wstring GetFontFamilyName(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetFontFamilyName();
	};

	std::wstring GetFontSubFamilyName(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetFontSubFamilyName();
	};

	std::wstring GetFontID(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetFontID();
	};

	std::wstring GetVersion(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetVersion();
	};

	std::wstring GetPostScriptName(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetPostScriptName();
	};

	std::wstring GetTrademark(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->GetTrademark();
	};

	bool IsBold(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->IsBold();
	};

	bool IsItalic(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->IsItalic();
	};

	bool IsRegular(size_t nIndex)
	{
		return m_vecTtfPtr.at(nIndex)->IsRegular();
	};
	static std::string WideToNarrow(const std::wstring& szWide);
	static std::wstring NarrowToWide(const std::string& szNarrow);
private:
	bool ParseFont();
	bool ParseFontMem(BYTE* pFile, size_t size, size_t offset);
	FILE* m_pFile;
	std::wstring m_szFilePath;
	std::vector<TTFParser*> m_vecTtfPtr;
};
