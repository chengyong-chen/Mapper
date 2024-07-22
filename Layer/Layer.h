#pragma once

#include <list>
#include <gdiplus.h>
#include "../Action/ActionStack.h"

#include "../Public/Global.h"
#include "../Public/ValueCounter.h"
#include "../Public/Enumeration.h"
#include "../Public/TreeNode.h"
#include "../Public/Tree.h"

#include "../Rectify/Tin.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"


namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace Clipper2Lib
{
	enum class JoinType;
	enum class EndType;
};
class CLayerTree;
class CRectF;
class CViewinfo;
class CPointTag;
class CFixedTag;
class CRectTag;
class CGroupTag;
class CPath;
class CATTDatasource;
class CTag;
class CSpot;
class CLine;
class CFill;
class CFillGeneral;
class CType;
class CHint;
class CVectorTile;
class CDatainfo;
class Chashtable;
class CLibrary;
class CLayex;
class CProjection;
class CGeom;
class CMark;
class CTag;
class CPoly;
class CTheme;
class CODBCDatabase;
class CATTHeaderProfile;
class BinaryStream;
enum Interpolation : BYTE;
enum ANCHOR : BYTE;

#define SpotDynamic 0X01
#define LineDynamic 0X02
#define FillDynamic 0X04
#define TypeDynamic 0X08
#define HintDynamic 0X10

class AFX_EXT_CLASS CLayer : public CTreeNode<CLayer>, public CObject
{
public:
	CLayer(CTree<CLayer>& tree);
	CLayer(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);
	CLayer(CTree<CLayer>& tree, CLayer* pParent, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);
	~CLayer() override;

public:
	WORD m_wId=0;
	BYTE m_bViewLevel = 0;
	BYTE m_bReserved = 0;
	CStringA m_strGeoCatogery;
	CString m_strName = _T("New");

	float m_minLevelObj;
	float m_maxLevelObj;
	float m_minLevelTag;
	float m_maxLevelTag;

public:
	bool m_bVisible = true;
	bool m_bShowGeom = true;
	bool m_bShowTag = true;
	bool m_bKeyQuery = true;
	bool m_bLocked = false;
	bool m_bDetour = false;
	bool m_bHide = false;
	bool m_bPivot = false;
	bool m_bAttribute = false;
	bool m_bCentroid = true;
	BYTE m_bDynamic = 0XFF&(~HintDynamic);
	mutable unsigned long m_nActiveCount = 0;
	BYTE ​m_bTagResistance = 0X00;

public:
	CLayerTree& m_tree;

public:
	CSpot* m_pSpot = nullptr;
	CLine* m_pLine = nullptr;
	CFillGeneral* m_pFill = nullptr;
	CType* m_pType = nullptr;
	CHint* m_pHint = nullptr;
	CTheme* m_pThem = nullptr;

private:
	CATTDatasource* m_attDatasource = nullptr;
public:
	void EnableAttribute(CString database);
	void DisableAttribute();
	CATTDatasource* GetAttDatasource() const;

public:
	CTypedPtrList<CObList, CGeom*> m_geomlist;

public:
	virtual BYTE Gettype() const { return 1; }
	virtual bool IsInfertile() const { return false; }
	virtual bool IsVisible(float levelCurrent) const;
	virtual bool IsControllable() const { return true; }
	virtual bool CanDrawDirect(const CGeom* pGeom) const;
	virtual bool Copyable() const;
	virtual bool SuportGeoCode() const;
	virtual bool CanActivateGeom(const CGeom* pGeom) const { return true; }

public:
	CSpot* GetSpot() const;
	CLine* GetLine() const;
	CFillGeneral* GetFill() const;
	CType* GetType() const;
	CHint* GetHint() const;
	CTheme* GetTheme() const;

public:
	virtual void Purify() const;
	virtual void Wipeout();
	virtual void CleanOrphan();

public:
	virtual CTypedPtrList<CObList, CGeom*>& GetGeomList() { return m_geomlist; }
	virtual int GetGeomCount() const;
	virtual CGeom* GetGeom(const DWORD& dwId) const;
	virtual CRect GetRect() const;
	virtual CRect GetActiveRect() const;
	virtual Gdiplus::Rect GetActiveArea(const CViewinfo& viewinfo) const;
	virtual std::list<CGeom*> GetActivatedGeoms() const;
	virtual void CountActiveStyles(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types) const;

	virtual void ActivateAll(CWnd* pWnd, const ACTIVEALL& activeal, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const;
	virtual void ActivateAll(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, CString& strName) const;
	virtual void ActivateSameStyle(CWnd* pWnd, const ACTIVEALL& activeal, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const CLine* pLine, const CFillGeneral* pFill, const CSpot* pSpot, const CType* pType) const;
	virtual void ActivateSameStyle(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const unsigned int target[5]) const;
	virtual void ActivateIdentical(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const unsigned int target1[5], const unsigned int target2[5]) const;
	virtual void ActivateInverse(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const;
	virtual void InactivateAll(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const;

	virtual void GetEditableActives(std::list<CGeom*>& geoms) const;
	virtual CGeom* Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point, const bool& bEnlock) const;
	virtual void Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CRect& rect, const bool& bEnlock) const;
	virtual void Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& center, const unsigned long& radius, const bool& bEnlock) const;
	virtual void Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoly& poly, const bool& bEnlock) const;
	virtual CGeom* Look(const CViewinfo& viewinfo, const CPoint& point) const;

	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const;
	virtual void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, const bool showCreated, const bool showDynamic) const;
	virtual void DrawAnchors(CDC* pDC, const CViewinfo& viewinfo, const CRect& inRect) const;
	virtual void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const CRect& inRect, const bool& labeloid, const bool& geogroid) const;
	virtual void Capture(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const;	
	virtual void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const;
	virtual void Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const;
	virtual void Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const;
	virtual void PositeTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, bool reset = false) const;

	virtual void Invalid(CDocument& document) const;
	virtual void Invalidate(CDocument& document, const CGeom* pGeom) const;
	virtual void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom) const;
	virtual void InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom, const unsigned int& nAnchor) const;
	virtual void InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom, const unsigned int& nAnchor) const;

	virtual void SetupAnno(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void ClearAnno(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void GeomAttribute(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void GeomName(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo);
	virtual void CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect);
	virtual void CreateTag(CWnd* pWnd, const CViewinfo& viewinfo, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign);
	virtual void SetupTag(CWnd* pWnd, const CViewinfo& viewinfo, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign);
	virtual void DeleteTag(CWnd* pWnd, const CViewinfo& viewinfo);

	virtual void ResetGeogroid(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void ResetLabeloid(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void PlantGeogroid(CWnd* pWnd, const CViewinfo& viewinfo, std::list<CMark*>& seeds);
	virtual void PlantLabeloid(CWnd* pWnd, const CViewinfo& viewinfo, std::list<CMark*>& seeds);

	virtual void AddHead(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack);
	virtual void AddTail(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack);
	virtual void CopyTo(CLayer* layer) const;
	virtual void Migrate(CLayer* layer);
	virtual bool CopyGeomTo(CLayer* layer, function<void(CString)> dbcopied) const;

public://modifying
	virtual bool NewGeom(CWnd* pWnd, const CDatainfo& datainfo, CGeom* pGeom, Undoredo::CActionStack& actionstack);	
	virtual void Duplicate(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CSize& delta, Undoredo::CActionStack& actionstack);
	virtual bool Replace(CWnd* pWnd, const CViewinfo& viewinfo, CGeom* pGeom, std::list<CGeom*> polys, Undoredo::CActionStack& actionstack);
	virtual void ReplaceWithMark(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual void Group(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual void UnGroup(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual bool Join(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack, std::function<bool(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joinable, std::function<void(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joined);
	virtual bool Combine(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual bool Disband(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual void Letitgo(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual bool Split(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& local, Undoredo::CActionStack& actionstack, std::function<void(CLayer* pLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred)>& splitted);
	virtual void BezierToPoly(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack);
	virtual void PolyToBezier(CWnd* pWnd, const CViewinfo& viewinfo, const float& scale, Undoredo::CActionStack& actionstack);
	virtual bool Clear(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual void Move(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, const CSize& delta);
	virtual void Align(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, CRect rect, ALIGN align);
	virtual void Arrange(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, ARRANGE arrange);
	virtual void Lock(Undoredo::CActionStack& actionstack);
	virtual void UnLock(Undoredo::CActionStack& actionstack);
	virtual void Transform(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32);
	virtual bool Clip(CWnd* pWnd, const CPath* pMask, bool bIn, int tolerance, Undoredo::CActionStack& actionstack);
	virtual void Densify(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack);
	virtual void Simplify(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack);	
	virtual void Normalize();
	virtual void PathReverse(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);
	virtual void PathBuffer(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int distance, const unsigned int tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype, Undoredo::CActionStack& actionstack);
	virtual void Close(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerence, Undoredo::CActionStack& actionstack);

public:
	virtual void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32);
	virtual void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32);
	
	virtual void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing);
	virtual void Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance);
	virtual void Rectify(CTin& tin);
	
	virtual void SetAttribute(CDocument& document);
	virtual void Regress(CWnd* pWnd, const CViewinfo& viewinfo);
	virtual void Relieve();
	virtual void RecalRect();
	virtual void RecalRect(const CDatainfo& datainfo);
	
public:
	virtual CSize GetDocInflation(const CViewinfo& viewinfo, const CGeom* pGeom) const;
	Gdiplus::SizeF GetVewInflation(const CViewinfo& viewinfo, const CGeom* pGeom) const;	
	CRect GetGeomDocBoundary(const CViewinfo& viewinfo, const CGeom* pGeom) const;
	void  ExtremizeScale(const float& oldMinimum, const float& oldMaximum, const float& newMinimum, const float& newMaximum);

public:
	virtual void SetModifiedFlag(bool bFlag)
	{
	}
public:
	void Assign(CODBCDatabase* pDatabase, const CATTHeaderProfile& profile);
	void ApplyName(CDatabase* pDatabase, CATTHeaderProfile& profile, CString strField);
	void ApplyCode(CDatabase* pDatabase, CATTHeaderProfile& profile, CString strField);

public:
	virtual void Tilize(CVectorTile** matrixTiles, int minRow, int maxRow, int minCol, int maxCol, const CViewinfo& viewinfo) const;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms=false);
	virtual void Serializelist(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseCap(CFile& file, const CDatainfo& datainfo) const;
	virtual void ReleaseCap(BinaryStream& stream, const CDatainfo& datainfo) const;
	virtual void ReleaseCap(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel) const;
	virtual void ReleaseCap(pbf::writer& writer, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool noFollowing) const;
	virtual void ReleaseWeb(CFile& file, const CDatainfo& datainfo) const;
	virtual void ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo) const;
	virtual void ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, CDatabase* pDatabase, CString strSQL, CString strIdField) const;
	virtual void ReleaseWeb(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, CDatabase* pDatabase, CString strSQL, CString strIdField) const;
	virtual void ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const;
	virtual void ExportGeomPs3(FILE* file, const CViewinfo& viewinfo) const;
	virtual void ExportTagPs3(FILE* file, const CViewinfo& viewinfo) const;
	virtual void ExportGeomPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo) const;
	virtual void ExportTagPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo) const;
	virtual void ExportMapInfoTab(const CDatainfo& datainfo, LPCTSTR lpszPath, const bool& bTransGeo, const unsigned int& tolerance) const;
	virtual void ExportMapInfoMif(const CDatainfo& datainfo, LPCTSTR lpszPath, const bool& bTransGeo, const unsigned int& tolerance) const;
public:
	virtual void SpotChanged(CWnd* pView, const CViewinfo& viewinfo) const;
	virtual void StyleChanged(CWnd* pView, const CViewinfo& viewinfo) const;
	virtual void TypeChanged(CWnd* pView, const CViewinfo& viewinfo) const;
	virtual void HintChanged(CWnd* pView, const CViewinfo& viewinfo) const;
	virtual void PreRemoveGeom(CGeom* pGeom) const;
	virtual void PreInactivateGeom(const CDatainfo& datainfo, CGeom* pGeom) const
	{
	}
	virtual void NewGeomDrew(CGeom* pGeom)
	{
	}
	virtual void NewGeomBred(CGeom* pGeom)
	{
	}
	virtual void GeomActivated(const CGeom* pGeom) const
	{
	}
	virtual void Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect)
	{
	}

public:
	static CLayer* Apply(CLayerTree& tree, BYTE type);

	 mutable CTypedPtrList<CObList, CPointTag*> m_pointTaglist;
	 mutable CTypedPtrList<CObList, CFixedTag*> m_fixedTaglist;
	 mutable CTypedPtrList<CObList, CRectTag*> m_rectTaglist;
	 mutable CTypedPtrList<CObList, CGroupTag*> m_groupTaglist;

protected:
	mutable CSize m_tmpLineInflation;
	mutable CSize m_tmpTypeInflation;
	virtual void RecalculateTemperyVariables(const CViewinfo& viewinfo) const;

	friend class CLayerTreeCtrl;
};
