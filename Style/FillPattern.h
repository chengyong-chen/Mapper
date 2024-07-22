#pragma once

#include "FillComplex.h"

class CPoly;

class __declspec(dllexport) CFillPattern : public CFillComplex
{
protected:
	DECLARE_SERIAL(CFillPattern);

public:
	CFillPattern();
public:
	~CFillPattern() override;

public:
	BYTE m_libId;
	BYTE m_nIndex;
	unsigned short m_nScale;
	unsigned short m_nAngle;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const override;
	Gdiplus::Brush* GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const override;

public:
	BOOL operator ==(const CFill& fill) const override;
	void CopyTo(CFill* pDen) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Pattern; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;

public:
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
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const override;

	void ExportPdf(HPDF_Page page) const override
	{
	};

	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CGeom* pGeom) const override
	{
	};
	void Decrease(CLibrary& library) const override;
};
