#pragma once

#include "Path.h"


using namespace std;
using namespace Clipper2Lib;
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class BinaryStream;
class CActionStack;
struct IMatrix2;

class AFX_EXT_CLASS CPRect : public CPath
{
protected:
	DECLARE_SERIAL(CPRect);
	CPRect();
	CPRect(const CRect& rect, CLine* pLine, CFillCompact* pFill);

public:
	int m_originX;
	int m_originY;
	int m_Width;
	int m_Height;

	bool m_bRegular;
	CPoint* m_pPoints;
	IMatrix2* m_pIMatrix2 = nullptr;

public:
	double GetSin() const;
	double GetCos() const;
	double GetTg() const;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
public:
	void RecalRect() override;
	CPoint& GetAnchor(const unsigned int& nAnchor) const override;
	bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const override;
	CPoint GetPoint(const unsigned int& nAnchor, const double& t) const override;
	double GetAngle(const unsigned int& nAnchor, const double& t) const override;
	virtual CPoint* GetPoints() const;
	CPoly* GetPoly() const override;	
	CPoly* Polylize(const unsigned int& tolerance) const override;
public:
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const override;

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& delta, const bool& bMatch) override;	
	void ChangeAnchor(const unsigned int& nAnchor, const CSize& delta, const bool& bMatch, Undoredo::CActionStack* pActionstack) override;	
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool PickIn(const CPoly& polygon) const override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override;

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	double Shortcut(const CPoint& point, CPoint& trend) const override;

	void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo) override;
public:
	~CPRect() override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
	void ReleaseWeb(CFile& file, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(BinaryStream& stream, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;

	bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const override;
	void ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const override;
	void ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const override;
	void ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const override;
	void ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;
	friend class CPRectTool;
};
