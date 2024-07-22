#pragma once

#include "Line.h"

class CGeom;
class CLineNormal;
class CLibrary;

class __declspec(dllexport) CLineRiver : public CLine
{
protected:
	DECLARE_SERIAL(CLineRiver);

public:
	CLineRiver();

	~CLineRiver() override;

public:
	WORD m_wColorId;
	WORD m_wDashId;

	unsigned short m_nFWidth;
	unsigned short m_nTWidth;
	unsigned short m_nTrend;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const override;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const override;
public:
	BOOL operator ==(const CLine& line) const override;
	void CopyTo(CLine* pDen) const override;
	bool IsSolid() const override { return false; };
	LINETYPE Gettype() const override { return LINETYPE::River; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
public:
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};

	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath) const override
	{
	};
};
