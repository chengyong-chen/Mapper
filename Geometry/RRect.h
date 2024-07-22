#pragma once

#include "PRect.h"
using namespace std;
using namespace Clipper2Lib;

class AFX_EXT_CLASS CRRect sealed : public CPRect
{
protected:
	DECLARE_SERIAL(CRRect);
	CRRect();
	CRRect(const CRect& rect, CLine* pLine, CFillCompact* pFill);

public:
	CSize m_nRadius;

public:
	bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const override;
	CPoint GetPoint(const unsigned int& nAnchor, const double& t) const override;
	double GetAngle(const unsigned int& nAnchor, const double& t) const override;
	CPoint* GetPoints() const override;
	CPoly* GetPoly() const override;
	CPoly* Polylize(const unsigned int& tolerance) const override;
	// Implementation
public:
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const override;
	void DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;

public:
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoly& polygon) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override;

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
public:
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
	void ReleaseWeb(CFile& file, CSize offset = CSize(0, 0)) const override;
	void ReleaseWeb(BinaryStream& stream, CSize offset = CSize(0, 0)) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
	void ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const override;
	void ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const override;
	void ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const override;
	void ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	friend class CRRectTool;
};
