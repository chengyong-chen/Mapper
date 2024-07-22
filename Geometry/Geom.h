#pragma once

#include <gdiplus.h>
#include <vector>
#include <list>
#include <optional>
#include "../Public/Enumeration.h"
#include <boost/compute/detail/sha1.hpp>

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace boost {
	namespace json {
		class object;
	}
};
namespace Undoredo {
	class CActionStack;
};
namespace Clipper2Lib
{
	template <typename T>
	struct Point;
	template <typename T>
	using Path = std::vector<Point<T>>;

	using Path64 = Path<int64_t>;;
	using Paths64 = std::vector< Path64>;
	enum class ClipType;
	enum class JoinType;
	enum class EndType;
}
class CPoly;
class CBezier;

class CLine;
class CFillGeneral;
class CFillCompact;
class CType;
class CHint;
class CFillRadial;
class CFillLinear;
class LinearBrush;
class RadialBrush;
class CTin;
class CTag;

class CDatainfo;
class CProjection;

class CSpot;
class CLibrary;
class CVectorTile;
class CPsboard;
class CODBCRecordset;

class CLine;
class CFill;
class CViewinfo;
struct Context;
class BinaryStream;
enum Interpolation : BYTE;
enum ANCHOR : BYTE;

#include "../Filter/FilterType.h"
#include "../Public/ValueCounter.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

class AFX_EXT_CLASS CGeom abstract : public CObject
{
	DECLARE_DYNAMIC(CGeom);
protected:
	CGeom();
	CGeom(CLine* pLine, CFillCompact* pFill);
	explicit CGeom(const CRect& rect, CLine* pLine, CFillCompact* pFill);

public:
	CLine* m_pLine = nullptr;
	CFillCompact* m_pFill = nullptr;
	CType* m_pType = nullptr;
	CHint* m_pHint = nullptr;

public:
	DWORD m_geoId = 0B00000000000000000000000000000000;
	DWORD m_attId = 0XFFFFFFFF;
	CStringA m_geoCode;
	CRect m_Rect;

	bool m_bLocked = false;
	bool m_bClosed = true;

	bool m_bGroup = false;

	BYTE m_bType = 0;
	CString m_strName;
	CTag* m_pTag = nullptr;
	
	FilterType m_filter = FilterType::None;
public:
	mutable bool m_bModified = false;
	mutable bool m_bActive = false;
	mutable float m_levelObj = 25.5;
	mutable float m_levelTag = 25.5;
	mutable BYTE m_bTag = 0X00;

public:
	virtual BYTE Gettype() const { return m_bType; };
	virtual CPoint GetOrigin() const { return m_Rect.TopLeft(); };
	virtual unsigned int GetAnchors() const;
	virtual CPoint& GetAnchor(const unsigned int& nAnchor) const;
	virtual bool GetAandT(unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const;
	virtual CPoint GetPoint(const unsigned int& nAnchor, const double& t) const;
	virtual double GetAngle(const unsigned int& nAnchor, const double& t) const;
	virtual bool IsValid() const;
	virtual bool IsArea() const { return false; };
	virtual bool IsOrphan() const { return false; };
	virtual unsigned long GetLength(const double& tolerance) const;
	virtual double GetLength(const CDatainfo& datainfo, const double& tolerance) const;
	virtual double GetArea() const;
	virtual double GetArea(const CDatainfo& datainfo) const;
	virtual CRect GetRect() const { return m_Rect; };
	virtual void Purify() const {};
	virtual void CleanOrphan() {};	
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;	
	virtual void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const;
	virtual void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const {};
	virtual bool IsVisible(float level) const 
	{
		return true;
	};

public:
	virtual void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const = 0;
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const = 0;
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const = 0;
	virtual void Mono(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const;
	virtual void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const;
	virtual void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const;
	virtual void DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const;
	virtual void DrawPath(CDC* pDC, const CViewinfo& viewinfo) const;
	virtual void DrawAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const;
	virtual void DrawPentip(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const { return DrawAnchor(pDC, viewinfo, nAnchor); };
	virtual void DrawAnchors(CDC* pDC, const CViewinfo& viewinfo) const;
	virtual void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const {};
	virtual Gdiplus::PathGradientBrush* SetGradientBrush(RadialBrush* brush, const CViewinfo& viewinfo, Gdiplus::GraphicsPath* path) const;
	virtual Gdiplus::PathGradientBrush* SetGradientBrush(RadialBrush* brush, const Gdiplus::Matrix& matrix, Gdiplus::GraphicsPath* path) const;
	virtual Gdiplus::LinearGradientBrush* SetGradientBrush(LinearBrush* brush, const CViewinfo& viewinfo) const;
	virtual Gdiplus::LinearGradientBrush* SetGradientBrush(LinearBrush* brush, const Gdiplus::Matrix& matrix) const;
	virtual void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const;
	virtual void DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const;
	virtual void Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const;
	virtual void Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const;
	virtual void Preoccupy(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const{};
	virtual void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const{};
	virtual void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const;
	
	virtual void PositeTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, bool reset = false) const {};

public:
	virtual bool Close(bool pForce, const unsigned int& tolerance, Undoredo::CActionStack* pActionstack = nullptr, bool ignore = false) { return FALSE; };
	virtual void ChangeAnchor(const unsigned int& nAnchor, const CSize& delta, const bool& bMatch, Undoredo::CActionStack* pActionstack);	
	virtual void Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) {};
	virtual void Simplify(const unsigned int& tolerance) {};
	virtual void Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms) {};
	
	virtual void Normalize() {};

public:
	virtual void ResetGeogroid() {};
	virtual void ResetLabeloid() {};
	virtual void RecalCentroid() {};
	virtual void SetGeogroid(const CPoint& point) {};
	virtual void SetLabeloid(const CPoint& point) {};
	virtual CPoint GetGeogroid() const;
	virtual CPoint GetLabeloid() const;	
	virtual void GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const {};
	virtual Gdiplus::PointF GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const;
	virtual unsigned long long Distance2(const CPoint& point) const{
		return 1;
	};
	virtual long Distance1(const CPoint& point) const
	{
		const unsigned long long min = Distance2(point);
		return PickIn(point) ? sqrt(min) : -sqrt(min);
	}
	virtual bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) { return false; };
	virtual bool Letitgo(CTypedPtrList<CObList, CGeom*>& geomlist) { return false; };
			
	virtual bool CreateName(CWnd* pWnd, const float& fontSize, const float& zoomPointToDoc, CObList& oblist);
	virtual void CreateTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign);
	virtual void CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect, const int& sizeFont);
	virtual void SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign);
	virtual void DeleteTag();
	virtual void Information(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint);

	virtual void Attribute(CWnd* pWnd, const CViewinfo& viewinfo);

	virtual void Open(CWnd* pWnd) const {};
public:
	virtual void SetStyle(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo);

	virtual void SetType(CWnd* pWnd, const CViewinfo& viewinfo) {};
	virtual void SetHint(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual bool IsDissident() const;
	virtual void Regress(CWnd* pWnd, CLibrary& library);
	virtual void CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* m_pSpot, const CType* pType) const;
	virtual bool HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const;
	

	virtual void RecalRect();
	virtual float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const;
	virtual Gdiplus::RectF GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const;
	virtual void Invalid(CDocument& document) const;
	virtual void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const;
	virtual void InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const;
	virtual void InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& nAnchor, const unsigned long& inflate) const;
	virtual void InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const;
	virtual void Move(const int& dx, const int& dy);
	virtual void Move(const CSize& delta);
	virtual void MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& delta, const bool& bMatch);	
	virtual void MoveContrl(CDC* pDC, const CViewinfo& viewinfo, std::pair<unsigned int, BYTE> nCtrle, const CSize& delta) {};	
	virtual bool ChangeContrl(std::pair<unsigned int, BYTE> handle, const CSize& delta) { return TRUE; };

	virtual bool PickOn(const CPoint& point, const unsigned long& gap) const;
	virtual bool PickIn(const CPoint& point) const;

	virtual bool PickIn(const CRect& rect) const;
	virtual bool PickIn(const CPoint& center, const unsigned long& radius) const;
	virtual bool PickIn(const CPoly& polygon) const;
	virtual bool IsIn(const CViewinfo& viewinfo, const Gdiplus::Rect& vewRect) const;
	

	virtual unsigned int PickAnchor(const CDatainfo& datainfo, const CViewinfo& viewinfo, const Gdiplus::Point& cliPoint) const;

	virtual void Reverse() {};
	virtual bool Junction(const CPoint& point1, const CPoint& point2);
	virtual BOOL operator ==(const CGeom& geom) const;
	virtual BOOL operator !=(const CGeom& geom) const;
	virtual CGeom* Clone() const;
	virtual void CopyTo(CGeom* pGeom, bool ignore = false) const;
	virtual void Inherit(CGeom* pGeom) const;
	virtual void Swap(CGeom* pGeom);
	virtual std::list<CGeom*> Split(const CPoint& point, const unsigned long& gap) const { return std::list<CGeom*>(); };
	virtual CGeom* Siderize(const CDatainfo& datainfo) { return this; };

	virtual void InsertAnchor(const unsigned nAnchor, const CPoint& point) {};

	virtual void RemoveAnchor(const unsigned int& nAnchor) {};
	virtual bool AddAnchor(const CPoint& point, bool pTail = true, bool ignore = false) { return false; };
	virtual double Shortcut(const CPoint& point, CPoint& trend) const;

	virtual void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32);
	virtual void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32);
	virtual void Transform(const CViewinfo& viewinfo);
	virtual void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing);
	virtual CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance);
	virtual void Rectify(CTin& tin);
	virtual CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke);
	virtual std::list<std::tuple<int, int, CGeom*>> Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const;

public:
	~CGeom() override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag);
	virtual bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const;

	virtual bool IntoDatabase(CODBCRecordset& rs, const CDatainfo& datainfo) const;
	virtual bool BeforeDispose(CDatabase* pDatabase, CString strTable, const CDatainfo& datainfo);

	virtual void GatherFonts(std::list<CStringA>& fontlist, bool bEnglish) const;

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual DWORD ReleaseCE(CFile& file, const BYTE& type) const;
	virtual void ReleaseWeb(CFile& file, CSize offset = CSize(0, 0)) const;

	virtual void ReleaseWeb(BinaryStream& stream, CSize offset = CSize(0, 0)) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, CSize& offset, CDatabase* pDatabase, CString& strSQL, CString& strIdField) const;
	virtual void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const;
	virtual void ReleaseWeb(pbf::writer& writer, CDatabase* pDatabase, CString& strSQL, CString& strIdField) const;

	virtual void ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const {};
	virtual void ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const {};
	virtual void ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const {};
	virtual void ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const {};
	virtual void ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const;
	virtual void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const =0;
	virtual void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const = 0;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const = 0;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const = 0;
	virtual void ExportPdfTag(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CHint* pHint) const;
	virtual void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
	{
	};
	virtual void ExportMid(FILE* fileMid) const;

public:
	virtual bool HasTag() const;
	virtual bool Tagable() const;
	virtual bool HasGeoCode() const {
		return !m_geoCode.IsEmpty();
	};

public:
	static CGeom* Apply(BYTE type);
	static CGeom* Load(CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag);

	static Gdiplus::PointF* Transform(const Gdiplus::Matrix& matrix, CPoint* pPoints, int count);
	static Gdiplus::Rect Transform(const Gdiplus::Matrix& matrix, CRect rect);
	static CPoint Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& angle, const CPoint& point);
	static void Serializelist(CArchive& ar, const DWORD dwVersion, CObList& oblist);
	static CRect RegulizeRect(CPoint point1, CPoint point2, bool bSquare, bool bCentral);

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
public:
	virtual bool Query(CString strKey);

	static bool QueryRecursion(const CString& string, CString strKey);
};