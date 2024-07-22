#pragma once

#include "Line.h"
#include "Midtable.hpp"
#include "SymbolMid.hpp"

class CGeom;
class CSymbolLine;

class __declspec(dllexport) CLineSymbol : public CLine
{
protected:
	DECLARE_SERIAL(CLineSymbol);

public:
	CLineSymbol();

	~CLineSymbol() override;

public:
	WORD m_libId;
	WORD m_symId;
	unsigned short m_nScale;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Pen* GetPen(const float& fZoom, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const override;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const override;
public:
	BOOL operator ==(const CLine& line) const override;
	void CopyTo(CLine* pDen) const override;
	bool IsSolid() const override { return false; };
	LINETYPE Gettype() const override { return LINETYPE::Symbol; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void SetWidth(const CMidtable<CSymbolMid<CSymbolLine>>& tagtable) override;
public:
	void ExportPs3(FILE* file, const CLibrary& library) const override;

	void ExportPs3(FILE* file, CPsboard& aiKey) const override
	{
	};
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath) const override;
	void Decrease(CLibrary& library) const override;
};
