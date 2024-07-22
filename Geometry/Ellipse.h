#pragma once
#include "PRect.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

class CViewinfo;
class AFX_EXT_CLASS CEllipse sealed : public CPRect
{
protected:
	DECLARE_SERIAL(CEllipse);
	CEllipse();
	CEllipse(const CRect& rect, CLine* pLine, CFillCompact* pFill);

public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;

public:
	bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const override;
	CPoint GetPoint(const unsigned int& nAnchor, const double& t) const override;
	double GetAngle(const unsigned int& nAnchor, const double& t) const override;

	CPoint* GetPoints() const override;
	CPoly* GetPoly() const override;		
	CPoly* Polylize(const unsigned int& tolerance) const override;

	// Implementation
public:
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool PickIn(const CPoly& polygon) const override;	
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override
	{
	};
	double Shortcut(const CPoint& point, CPoint& trend) const override;

public:
	BOOL operator ==(const CGeom& geom) const override;
	void ReleaseCE(CArchive& ar) const override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
	void ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const override;
	void ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const override;
	void ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const override;
	void ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
public:
	friend class CEllipseTool;
};
