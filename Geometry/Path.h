#pragma once

#include "Geom.h"
#include <list>
#include <optional>
#include <functional>

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

using namespace std;
class CViewinfo;
struct Context;

class AFX_EXT_CLASS CPath abstract : public CGeom
{
protected:
	DECLARE_DYNAMIC(CPath);
	CPath();
	CPath(CLine* pLine, CFillCompact* pFill);
	CPath(const CRect& rect, CLine* pLine, CFillCompact* pFill);
	~CPath() override;

public:
	std::list<CPath*> m_holes;
	mutable	bool m_bMeridianCrossing = true;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const override;
public:
	virtual CPoly* GetPoly() const
	{
		return nullptr;
	};
	virtual CPoly* Polylize(const unsigned int& tolerance) const
	{
		return nullptr;
	};
	void GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const override;
	void Purify() const override;

public:
	using CGeom::Preoccupy;
	void Draw(Gdiplus::Graphics& g, CLine* pLine, const CViewinfo& viewinfo, const float& ratio) const;
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const override;
	void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const override;	
	virtual void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const = 0;
	virtual void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const = 0;
	void Preoccupy(Gdiplus::Graphics& g, CLine* pLine, const CViewinfo& viewinfo, const float& ratio) const;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
public:
	CPoint* m_geogroid = nullptr;
	CPoint* m_labeloid = nullptr;
	CPoint m_centroid = CPoint(0, 0);//it is actually calculated labeloid, becase centriod is not usefule, we treat labeloid as centriod 
	virtual CPoint FindCentroid() const { return m_Rect.CenterPoint(); };
	void RecalCentroid() override;
	CPoint GetGeogroid() const override;
	CPoint GetLabeloid() const override;
	void SetGeogroid(const CPoint& point) override;
	void SetLabeloid(const CPoint& point) override;
	void ResetGeogroid() override;
	void ResetLabeloid() override;
	Gdiplus::PointF GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const override;

public:
	using CGeom::Normalize;
	virtual void Normalize(bool bHole) {};
	virtual void Replace(std::vector<int>& coords, Undoredo::CActionStack& actionstack) {};
public:
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CLine* pLine, const CViewinfo& viewinfo, CPsboard& aiKey) const;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
public:
	static CGeom* Convert(Clipper2Lib::Paths64& paths, bool closed);
	struct Cell
	{
		int centerx;// cell center	
		int centery;// cell center
		int h;		// half the cell size
		long dis;	// distance from cell center to polygon
		long max;	// max distance to polygon within a cell

		Cell(int x, int y, int half, CGeom* polygon)
			:centerx(x), centery(y), h(half)
		{
			CPoint center(centerx, centery);
			dis = polygon->Distance1(center);
			max = dis+half*sqrt(2);
		}
	};
};
