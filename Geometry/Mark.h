#pragma once
#include "Geom.h"
#include "../Public/Enumeration.h"

namespace boost {
	namespace json {
		class object;
	}
};
class CSpot;

class CPole;
class CLibrary;
class CFillGeneral;
class CDatainfo;
class CViewinfo;
class BinaryStream;
enum Interpolation : BYTE;
struct IMatrix2;

class AFX_EXT_CLASS CMark : public CGeom
{
protected:
	DECLARE_SERIAL(CMark);
	CMark();
	CMark(const CPoint& origin);

	~CMark() override;

public:
	CPoint m_Origin;
	CSpot* m_pSpot;

public:
	IMatrix2* m_pIMatrix2 = nullptr;

private:
	mutable Gdiplus::RectF m_cliMark;
	// Implementation

public:
	CPoint GetOrigin() const override { return m_Origin; };
	bool IsArea() const  override { return true; };
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override { return 0.f; };
	Gdiplus::PointF GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const override;
	bool IsVisible(float level) const override
	{
		return level>=m_levelObj;
	};
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const override;

public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override
	{
	};
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override
	{
	};
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const BYTE& bTag) const;	
	void DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;	
	void Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const override;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override{};
	void PositeTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, bool reset = false) const override;
private:
	void FillTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const BYTE& bTag) const;

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override { return PickIn(point); }

	unsigned int PickAnchor(const CDatainfo& datainfo, const CViewinfo& viewinfo, const Gdiplus::Point& cliPoint) const override
	{
		return 0;
	};

	void SetStyle(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo) override
	{
	};
	void SetSpot(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo);
	bool IsDissident() const override;
	void Regress(CWnd* pWnd, CLibrary& library) override;
	void CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* m_pSpot, const CType* m_pType) const override;
	bool HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const override;
	Gdiplus::RectF GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void CalCorner(const CSpot* pSpot, const CDatainfo& datainfo, const CLibrary& library);
	void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void Rectify(CTin& tin) override;
	void Transform(const CViewinfo& viewinfo) override;
	double Shortcut(const CPoint& point, CPoint& trend) const override;
	bool CreateName(CWnd* pWnd, const float& fontSize, const  float& zoomPointToDoc, CObList& oblist) override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override { return false; };

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;
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
	
	virtual void Tile(CFile** matrixFiles, int countX, int countY, const CViewinfo& viewinfo, const bool& bStroke, const bool& bFill)
	{
	}
	bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const override;
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override {};
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override {};
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;

	friend class CMarkTool;
};
