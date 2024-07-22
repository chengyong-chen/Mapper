#pragma once


#include "FillComplex.h"


namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CGeom;
class CFillCompact;
class CPoly;
class CViewinfo;
class CPsboard;
class BinaryStream;

class __declspec(dllexport) CFillSymbol : public CFillComplex
{
protected:
	DECLARE_SERIAL(CFillSymbol);

public:
	CFillSymbol();

	~CFillSymbol() override;

public:
	WORD m_libId;
	WORD m_symId;

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
	FILLTYPE Gettype() const override { return FILLTYPE::Symbol; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file, const CLibrary& library) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const override;

	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CGeom* pGeom) const override
	{
	};
	void Decrease(CLibrary& library) const override;
};
