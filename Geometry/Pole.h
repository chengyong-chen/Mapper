#pragma once

#include "Geom.h"
class AFX_EXT_CLASS CPole sealed : public CGeom
{
protected:
	DECLARE_SERIAL(CPole);

public:
	CPoint m_Center;
	bool m_bDirection;

public:
	CTypedPtrList<CObList, CGeom*> m_geomlist;

public:
	CPole();
	CPole(const CRect& rect);

	~CPole() override;
public:
	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;	
	void Preoccupy(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const;
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override {};
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override {};
};
