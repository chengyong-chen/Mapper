#pragma once

#include "PRect.h"
#include "Global.h"
#include "../Public/Enumeration.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace json {
		class object;
	}
};
class CLibrary;
class CFillGeneral;
class CDatainfo;
class CViewinfo;
struct Context;
enum ANCHOR : BYTE;

class AFX_EXT_CLASS CText : public CPRect
{
protected:
	DECLARE_SERIAL(CText);
	CText();
	CText(CPoint origin);

public:
	CPoint m_Origin;
	TEXT_ALIGN m_Align;
	TEXT_ANCHOR m_Anchor;

public:
	bool IsArea() const  override { return true; };
	bool IsValid() const override;
	bool Tagable() const override { return false; };
	bool IsOrphan() const override;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const override;

public:
	CPoint GetOrigin() const override{ return m_Origin; };
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override { return 0.f; }
	void GatherFonts(std::list<CStringA>& fontlist) const override;
	CPoint GetGeogroid() const override { return CGeom::GetGeogroid(); }
	CPoint GetLabeloid() const override { return CGeom::GetLabeloid(); }

public:	
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override
	{
	}
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	static void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, Gdiplus::Font* font, Gdiplus::Brush* textBrush, Gdiplus::Pen* rimPen, Gdiplus::Brush* rimBrush, bool roundRect)
	{
	}
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot, const bool& bOblige) const override
	{
	}
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void DrawAnchors(CDC* pDC, const CViewinfo& viewinfo) const override;	

public:
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override;
	void SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign) override {}
	void SetStyle(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo) override
	{
	}
	void SetType(CWnd* pWnd, const CViewinfo& viewinfo) override;

	static void SetHint(CWnd* pWnd)
	{
	}
	bool CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& oblist) override { return false; }
	void CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* m_pSpot, const CType* pType) const override;
	bool HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override
	{
		return PickIn(point);
	}
	unsigned int PickAnchor(const CDatainfo& datainfo, const CViewinfo& viewinfo, const Gdiplus::Point& cliPoint) const override
	{
		return 0;
	}
	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	Gdiplus::RectF GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void CalCorner(CType* pType, float fPointToDoc, const float& ratio);
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override { return false; }
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void Rectify(CTin& tin) override;
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
	bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;

public:
	friend class CTextTool;
	static void AnalyzeString(const CString& string, CObArray& array);
public:
	class CWord : public CObject
	{
	public:
		CWord()
		{
			bCJK = true;
		}

	public:
		CString str;
		bool bCJK;
	};
};