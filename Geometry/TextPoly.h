#pragma once
#include "Poly.h"

namespace boost {
	namespace json {
		class object;
	}
};
class CLibrary;
class CViewinfo;
struct Context;
enum ANCHOR : BYTE;


// (0 ~ 3 bit)
#define	ALIGN_BASELINE	 0X0000L
#define	ALIGN_CENTER 	 0X0001L
#define	ALIGN_DECENT	 0X0002L
//(4 ~ 7 bit)
#define	ORIENT_VERTICAL  0X0000L
#define	ORIENT_PARALLEL	 0X0010L

//(8 ~ 11 bit)
#define	SPACE_ANCHOR     0X0000L
#define	SPACE_AVERAGE	 0X0100L

class AFX_EXT_CLASS CTextPoly sealed : public CPoly
{
protected:
	DECLARE_SERIAL(CTextPoly);
	CTextPoly();
	CTextPoly(const CRect& rect);

public:
	DWORD m_dwStyle;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const override;
public:
	bool Tagable() const override { return false; };
public:
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override;
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const override;
	void InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot, const bool& bOblige) const override
	{
	};	
public:
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override;
	void SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign) override {};

	void SetStyle(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo) override
	{
	};
	void SetType(CWnd* pWnd, const CViewinfo& viewinfo) override;

	virtual void SetHint(CWnd* pWnd)
	{
	};
	bool CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& oblist) override { return 0; };
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;

	void Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override	{};
	void Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override {};
	void Simplify(const unsigned int& tolerance) override {};
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;

public:
	void GatherFonts(std::list<CStringA>& fontlist) const override;

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
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;
	void ExportMid(FILE* fileMid) const override;
};
