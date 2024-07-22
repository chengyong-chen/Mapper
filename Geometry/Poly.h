#pragma once

#include "Path.h"


namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace json {
		class object;
	}
};
namespace Clipper2Lib
{
	enum class JoinType;
	enum class EndType;
};
class LLine;
class CSpot;
class CTin;
class CDatainfo;
class CProjection;
class CBezier;
class BinaryStream;
enum Interpolation : BYTE;

#include <vector>

#include "../Action/ActionStack.h"

using namespace std;
using namespace Clipper2Lib;

class AFX_EXT_CLASS CPoly : public CPath
{
protected:
	DECLARE_SERIAL(CPoly);
	CPoly();
	CPoly(CLine* pLine, CFillCompact* pFill);
	CPoly(const CRect& rect, CLine* pLine, CFillCompact* pFill);
	CPoly(const std::vector<CPoint>& points, bool bCLosed = false);
	CPoly(CPoint* pPoints, unsigned int anchors, bool bCLosed = false);

public:
	bool IsValid() const override;
	bool IsOrphan() const override;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
public:
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override;
public:
	unsigned int GetAnchors() const override;
	CPoint& GetAnchor(const unsigned int& nAnchor) const override;
	bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const override;
	CPoint GetPoint(const unsigned int& nAnchor, const double& t) const override;
	double GetAngle(const unsigned int& nAnchor, const double& t) const override;	
	virtual CRect GetSegRect(const unsigned int& nAnchor) const;
	virtual CPoint GetSample(const short& direction, const CPoint& center, const long& radius) const;
	unsigned long GetLength(const double& tolerance) const override;
	double GetLength(const CDatainfo& datainfo, const double& tolerance) const override;
	double GetArea() const override;
	double GetArea(const CDatainfo& datainfo) const override;
	void Purify() const override;
	virtual LLine* GetSegment(const unsigned int& nAnchor) const;
	virtual CPoly* GetByFHTToTHT(unsigned int fAnchor, double st, unsigned int tAnchor, double et) const;

	void RecalRect() override;
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const override;
	void InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const override;
	void InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const override;
	void InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const override;
	bool AddAnchor(const CPoint& point, bool pTail = true, bool ignore = false) override;
	void InsertAnchor(unsigned nAnchor, const CPoint& point) override;
	bool Junction(const CPoint& point1, const CPoint& point2) override { return false; };
	bool Close(bool pForce, const unsigned int& tolerance, Undoredo::CActionStack* pActionstack = nullptr, bool ignore = false) override;
	virtual CPoly* Join(const CPoly* pPoly, const BYTE& mode) const;
	void GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const override;
	std::list<CGeom*> Split(const CPoint& point, const unsigned long& gap) const override;
	bool CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& geomlist) override;
public:
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawPentip(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const override;	
	void DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const override;
	void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;
	virtual void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix, const bool& reverse) const;
	virtual void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance, const bool& reverse) const;

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& delta, const bool& bMatch) override;	
	void ChangeAnchor(const unsigned int& nAnchor, const CSize& delta, const bool& bMatch, Undoredo::CActionStack* pActionstack) override;
	void Reverse() override;
	CPoint* Reverted() const;
	virtual CGeom* Buffer(const unsigned int& distance, const unsigned int& tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype) const;
	CPoly* GetPoly() const override
	{
		return (CPoly*)Clone();
	};
	virtual void Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack);
	void Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override;
	void Simplify(const unsigned int& tolerance) override;
	void Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms) override;
	void Normalize() override;
	void Normalize(bool bHole) override;
	void Replace(std::vector<int>& coords, Undoredo::CActionStack& actionstack) override;
	virtual bool Add(const CPoly* poly);
	double Shortcut(const CPoint& point, CPoint& trend) const override;
	virtual bool GetOrthocentre(const CPoint& point, unsigned int& nAnchor, double& t) const;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	virtual unsigned int PickOn(const CPoint& point, const unsigned long& gap, double& t) const;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool PickIn(const CPoly& polygon) const override;

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore=true) const override;
	void CopyPoints(CPoint* points, bool direction, unsigned int& current);
	CPoint* ClonePoints(bool direction);
	void Swap(CGeom* pGeom) override;
	virtual CBezier* Bezierize(const float scale) const;
	void RemoveAnchor(const unsigned int& nAnchor) override;
	void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void Rectify(CTin& tin) override;
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;

	static std::vector<std::pair<unsigned int, CPoint*>> Clipin(CRect rect, const CPoint* points, unsigned int nAnchors);
	std::list<std::tuple<int, int, CGeom*>> Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const override;
	
public:
	~CPoly() override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
	void ReleaseWeb(CFile& file, CSize offset = CSize(0, 0)) const override;
	void ReleaseWeb(BinaryStream& stream, CSize offset = CSize(0, 0)) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
	virtual void ReleasePoints(pbf::writer& writer, const CSize offset = CSize(0, 0));
	bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const override;
	void ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const override;
	void ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const override;
	void ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;

private:
	static CPoint BezierCtrlPoint(const CPoint& prev, const CPoint& point, const CPoint& next, const float& scale);

public:
	static void GetIntersectinon(const CPoly* poly1, const CPoly* poly2, CArray<CPoint, CPoint>& pointarray);
	static unsigned short Joinable(const CPoly* pPoly1, const CPoly* pPoly2, const unsigned int& tolerance);

public:
	unsigned long long Distance2(const CPoint& point) const override;
	CPoint FindCentroid() const override;
	void Diagnose() const;

public:
	unsigned int m_nAnchors = 0;
	unsigned int m_nAllocs = 0;
	CPoint* m_pPoints = nullptr;

public://temporary variables
	mutable	optional<bool> m_southpolin;

public:
	friend class CPolyTool;
	friend struct Cell;

private:
	static std::list<std::vector<CPoint>> CloseSegment(const CRect& clipper, std::list<std::vector<CPoint>*> polylist, bool clockwise);
public:
	static void DrawSquareAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point);
	static void DrawSquareAnchor(CDC* pDC, const CViewinfo& viewinfo, const CPoint& point);
	static void DrawBlackAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point);
	static void DrawLineTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint docPoint1, CPoint docPoint2);
	static long Length(Gdiplus::Point* points, int nAnchors);
	static Gdiplus::Point GetPoint(const Gdiplus::Point* pPoints, int nAnchors, const unsigned int& nAnchor, const double& t);
	static bool GetAandT(const Gdiplus::Point* pPoints, int nAnchors, unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et);
	template<class T>
	static bool PointInPolygon(const T& point, T* pPoints, const unsigned int& nAnchors)
	{
		long odd = 0;
		for(unsigned int i = 0; i < nAnchors - 1; i++)
		{
			if(point == pPoints[i])
				return true;
			else if(point.x > max(pPoints[i].x, pPoints[i + 1].x))
				continue;
			else if(point.y > max(pPoints[i].y, pPoints[i + 1].y))
				continue;
			else if(point.y < min(pPoints[i].y, pPoints[i + 1].y))
				continue;
			else if(pPoints[i].y == pPoints[i + 1].y)
				continue;
			else if(point.y == pPoints[i].y && pPoints[i].y < pPoints[i + 1].y)//excludes the starting point for a downward edge 
				continue;
			else if(point.y == pPoints[i + 1].y && pPoints[i].y > pPoints[i + 1].y)//excludes the ending point for a upward edge
				continue;
			else
			{
				long xtemp = round(pPoints[i].x + (double)(point.y - pPoints[i].y)*(pPoints[i + 1].x - pPoints[i].x)/(double)(pPoints[i + 1].y - pPoints[i].y));
				if(xtemp == point.x)
					return true;
				else if(xtemp > point.x)
					odd++;
			}
		}
		return odd % 2 == 1 ? true : false;
	};
};

typedef CTypedPtrList<CObList, CPoly*> CPolyList;