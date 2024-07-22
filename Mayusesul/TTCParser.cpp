#include "StdAfx.h"
#include "TTCParser.h"
#include "MemPtrReader.h"
#include "../Public/EndianSwap.h"

TTCParser::TTCParser(void)
{
	m_pFile = nullptr;
}

TTCParser::~TTCParser(void)
{
	Clear();
}

bool TTCParser::Parse(const std::wstring& szFilePath)
{
	Clear();
	m_szFilePath = szFilePath;
	m_pFile = _wfopen(szFilePath.c_str(), L"rb");
	if(nullptr==m_pFile)
		return false;
	return ParseFont();
}

bool TTCParser::Parse(BYTE* pFile, size_t size)
{
	return Parse(pFile, size, 0);
}

bool TTCParser::Parse(BYTE* pFile, size_t size, size_t offset)
{
	Clear();
	if(pFile==nullptr)
		return false;
	return ParseFontMem(pFile, size, offset);
}

bool TTCParser::ParseFontMem(BYTE* pFile, size_t size, size_t offset)
{
	stTTCHeader TTCHeader;
	memset(&TTCHeader, 0, sizeof TTCHeader);
	MemPtrReader memPtrReader(pFile, size, offset);
	memPtrReader.Read(&TTCHeader, sizeof TTCHeader, 1);
	EndianSwap::Switch(TTCHeader.numFonts);
	std::string strTag = "";
	strTag += TTCHeader.tag[0];
	strTag += TTCHeader.tag[1];
	strTag += TTCHeader.tag[2];
	strTag += TTCHeader.tag[3];
	if(strTag!="ttcf")
		return false;
	std::vector<UINT> vecOffset;
	for(size_t i = 0; i<TTCHeader.numFonts; ++i)
	{
		UINT nOffset = 0;
		memset(&nOffset, 0, sizeof nOffset);
		memPtrReader.Read(&nOffset, sizeof nOffset, 1);
		EndianSwap::Switch(nOffset);
		vecOffset.push_back(nOffset);
	}
	for(size_t i = 0; i<vecOffset.size(); ++i)
	{
		TTFParser* pTtf = new TTFParser();
		memPtrReader.Seek(vecOffset.at(i),SEEK_SET);
		bool b = pTtf->Parse(memPtrReader.GetPtr(), memPtrReader.Size(), vecOffset.at(i));
		if(b)
		{
			m_vecTtfPtr.push_back(pTtf);
		}
		else
		{
			delete pTtf;
			pTtf = nullptr;
		}
	}
	if(TTCHeader.numFonts>0)
		return true;
	return false;
}

bool TTCParser::ParseFont()
{
	stTTCHeader TTCHeader;
	memset(&TTCHeader, 0, sizeof TTCHeader);
	fread(&TTCHeader, sizeof TTCHeader, 1, m_pFile);
	EndianSwap::Switch(TTCHeader.numFonts);
	std::string strTag = "";
	strTag += TTCHeader.tag[0];
	strTag += TTCHeader.tag[1];
	strTag += TTCHeader.tag[2];
	strTag += TTCHeader.tag[3];
	if(strTag!="ttcf")
		return false;
	std::vector<UINT> vecOffset;
	for(size_t i = 0; i<TTCHeader.numFonts; ++i)
	{
		UINT nOffset = 0;
		memset(&nOffset, 0, sizeof nOffset);
		fread(&nOffset, sizeof nOffset, 1, m_pFile);
		EndianSwap::Switch(nOffset);
		vecOffset.push_back(nOffset);
	}
	for(size_t i = 0; i<vecOffset.size(); ++i)
	{
		TTFParser* pTtf = new TTFParser();
		fseek(m_pFile, vecOffset.at(i),SEEK_SET);
		bool b = pTtf->Parse(m_pFile, vecOffset.at(i));
		if(b)
		{
			m_vecTtfPtr.push_back(pTtf);
		}
		else
		{
			delete pTtf;
			pTtf = nullptr;
		}
	}
	if(TTCHeader.numFonts>0)
		return true;
	return false;
}

void TTCParser::Clear()
{
	if(m_pFile)
		fclose(m_pFile);
	m_pFile = nullptr;
	m_szFilePath = L"";
	for(size_t i = 0; i<m_vecTtfPtr.size(); ++i)
	{
		if(m_vecTtfPtr.at(i))
		{
			delete m_vecTtfPtr.at(i);
			m_vecTtfPtr.at(i) = nullptr;
		}
	}
	m_vecTtfPtr.clear();
}

std::wstring TTCParser::NarrowToWide(const std::string& szNarrow)
{
	return TTFParser::NarrowToWide(szNarrow);
}

std::string TTCParser::WideToNarrow(const std::wstring& szWide)
{
	return TTFParser::WideToNarrow(szWide);
}
