#pragma once

#include "Symbol.h"
#include "SymbolLine.h"
#include "SymbolFill.h"
#include "SymbolSpot.h"

#include "PatternMid.h"
#include "FlashMid.h"
#include "SymbolMid.hpp"
#include "Midtable.hpp"

#include "../Pbf/writer.hpp"

#include "Icmm.h"

class __declspec(dllexport) CLibrary : public CObject
{
	DECLARE_SERIAL(CLibrary);
public:
	CLibrary();

public:
	~CLibrary() override;

public:
	CIcmm m_Icmm;

public:
	CMidtable<CSymbolMid<CSymbolLine>> m_libLineSymbol;
	CMidtable<CSymbolMid<CSymbolFill>> m_libFillSymbol;
	CMidtable<CPatternMid> m_libFillPattern;
	CMidtable<CFlashMid> m_libFillFlash;
	CMidtable<CSymbolMid<CSymbolSpot>> m_libSpotSymbol;
	CMidtable<CPatternMid> m_libSpotPattern;
	CMidtable<CFlashMid> m_libSpotFlash;

public:
	void Initialize();
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
};
