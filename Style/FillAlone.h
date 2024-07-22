#pragma once

#include "FillCompact.h"



class CGeom;
class CColor;
class CPoly;
class CLibrary;
class CPsboard;
class CViewinfo;

class __declspec(dllexport) CFillAlone : public CFillCompact
{
protected:
	DECLARE_SERIAL(CFillAlone);
	CFillAlone();
	CFillAlone(CColor* pColor);
public:
	~CFillAlone() override;

public:
	CColor* m_pColor;
	bool m_bOverprint;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const override;
	Gdiplus::Brush* GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const override;

public:
	BOOL operator ==(const CFill& fill) const override;
	void CopyTo(CFill* pDen) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Alone; };

	CColor* GetColor() const override
	{
		return m_pColor;
	};
	void SetColor(CColor* pColor) override
	{
		m_pColor = pColor;
	};

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;

	void ExportPs3(FILE* file, const CLibrary& library) const override
	{
	};
	void ExportPs3(FILE* file, CPsboard& aiKey) const override;

	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const override
	{
	};
	void ExportPdf(HPDF_Page page) const override;

	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CGeom* pGeom) const override
	{
	};
};
