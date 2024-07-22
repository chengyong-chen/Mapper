#pragma once

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

class CGeom;
class COleItem;

class AFX_EXT_CLASS COleObj : public CGeom
{
protected:
	DECLARE_SERIAL(COleObj);
	COleObj();

public:
	COleObj(const CRect& rect);

	~COleObj() override;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
public:
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override { return 0.f; };
	bool IsArea() const  override { return true; };
public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override {};
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	
public:
	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void Open(CWnd* pWnd) const override;
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	CSize m_extent;
	COleItem* m_pOleItem;


public:
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override {};
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override {};
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override {};
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override {};
};