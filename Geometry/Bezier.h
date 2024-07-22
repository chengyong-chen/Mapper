#pragma once

#include "Poly.h"

class LBezier;
class CTin;
class CActionStack;
class pbf::writer;
enum Interpolation : BYTE;

class AFX_EXT_CLASS CBezier : public CPoly
{
protected:
	DECLARE_SERIAL(CBezier);
	CBezier();
	CBezier(const CRect& rect, CLine* pLine, CFillCompact* pFill);
	CBezier(const std::vector<CPoint>& points, int anchors, bool bCLosed);

	// Operations
	void RecalRect() override;
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const override;
	bool AddAnchor(const CPoint& point, bool pTail = true, bool ignore = false) override;
	virtual bool AddAnchor(const CPoint points[], bool pTail = true, bool ignore = false);
	void InsertAnchor(unsigned int nAnchor, const CPoint& point) override;
	virtual void InsertAnchor(unsigned int nAnchor, const CPoint& fctrol, const CPoint points[], const CPoint& tctrol);
	bool Close(bool pForce, const unsigned int& tolerance, Undoredo::CActionStack* pActionstack = nullptr, bool ignore = false) override;
	CPoly* Join(const CPoly* pPoly, const BYTE& mode) const override;
	std::list<CGeom*> Split(const CPoint& local, const unsigned long& gap) const override;	
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;

public:
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;
	void DrawPentip(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const override;
	void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const override;

	// Implementation
public:
	CPoint& GetAnchor(const unsigned int& nAnchor) const override;
	CPoint GetPoint(const unsigned int& nAnchor, const double& t) const override;
	double GetAngle(const unsigned int& nAnchor, const double& t) const override;
	CRect GetSegRect(const unsigned int& nhandle) const override;
	CPoint GetSample(const short& direction, const CPoint& center, const long& radius) const override;
	unsigned long GetLength(const double& tolerance) const override;
	double GetLength(const CDatainfo& datainfo, const double& tolerance) const override;
	double GetArea() const override;
	double GetArea(const CDatainfo& datainfo) const override;
	bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const override;
	LLine* GetSegment(const unsigned int& nAnchor) const override;
	CPoly* GetByFHTToTHT(unsigned int fAnchor, double st, unsigned int tAnchor, double et) const override;
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& delta, const bool& bMatch) override;	
	void ChangeAnchor(const unsigned int& nAnchor, const CSize& delta, const bool& bMatch, Undoredo::CActionStack* pActionstack) override;	
	void MoveContrl(CDC* pDC, const CViewinfo& viewinfo, std::pair<unsigned int, BYTE> nCtrle, const CSize& delta) override;
	bool ChangeContrl(std::pair<unsigned int, BYTE> handle, const CSize& delta) override;
	void SetContrl(const unsigned int& nAnchor, const BYTE& nCtrle, const CPoint& point, bool ignore);
	void Reverse() override;
	CGeom* Buffer(const unsigned int& distance, const unsigned int& tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype) const override;
	void Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override;
	void Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override;
	void Simplify(const unsigned int& tolerance) override;
	void Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms) override;
	bool Add(const CPoly* poly) override;
	double Shortcut(const CPoint& point, CPoint& trend) const override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoly& polygon) const override;
	unsigned int PickOn(const CPoint& point, const unsigned long& gap, double& t) const override;

	bool Junction(const CPoint& point1, const CPoint& point2) override { return false; }
	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void RemoveAnchor(const unsigned int& nAnchor) override;
	void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void Rectify(CTin& tin) override;
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;
	std::list<std::tuple<int, int, CGeom*>> Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const override;

public:
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
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;

	CPoly* Polylize(const unsigned int& tolerance) const override;
	void GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const override;
public:
	static void DrawSquareAnchor(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point[]);
	static void DrawBezierTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point1[], CPoint point2[]);
	static void DrawCross(CDC* pDC, Gdiplus::Point point);
	static bool PointInBeziergon(const CPoint& point, CPoint* points, const unsigned int& ptCount);

public:
	unsigned long long Distance2(const CPoint& point) const override;
	CPoint FindCentroid() const override;
public:
	friend class CBezierTool;
};