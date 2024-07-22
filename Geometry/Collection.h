#pragma once

#include "Geom.h"
#include "../Public/Enumeration.h"
using namespace std;
using namespace Clipper2Lib;

class CGeom;
class CViewinfo;
struct Context;
enum Interpolation : BYTE;

template<class T>
class CCollection abstract : public CGeom
{
protected:
	CCollection();
	CCollection(CLine* pLine, CFillCompact* pFill);

public:
	CTypedPtrList<CObList, T*> m_geomlist;

public:
	bool IsDissident() const override;
	bool IsOrphan() const override;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const override;

public:
	void RecalCentroid() override;
	void ResetGeogroid() override;
	void ResetLabeloid() override;
public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const override;
	void Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const override;
	void Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const override; 
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
public:
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override;
	T* GetBiggest() const;
	void OrderByArea();

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool PickIn(const CPoly& polygon) const override;

	void Regress(CWnd* pWnd, CLibrary& library) override;
	virtual void Fleefrom(CGeom* pGeom) const;
	void CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* m_pSpot, const CType* pType) const override;
	bool HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const override;
	void RecalRect() override;	
	void Purify() const override;
	void Clear();
	void CleanOrphan() override;

	virtual void AddMember(CGeom* pGeom);
	virtual void AddMembers(std::list<CGeom*>& geoms);
	virtual void AddMembers(std::list<CPoly*>& polys);
	virtual bool RemoveMember(T* pGeom);
	virtual void Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack);
	void Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack) override;
	void Simplify(const unsigned int& tolerance) override;
	void Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms) override;
	void Normalize() override;

	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override;	
	void ChangeAnchor(const unsigned int& nAnchor, const CSize& delta, const bool& bMatch, Undoredo::CActionStack* pActionstack) override;	
	void Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Transform(const CViewinfo& viewinfo) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void Rectify(CTin& tin) override;

public:
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;
	std::list<std::tuple<int, int, CGeom*>> Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const override;

public:
	void GatherFonts(std::list<CStringA>& fontlist, bool bEnglish) const override;
public:
	~CCollection() override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(CFile& file, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(BinaryStream& stream, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
	void ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const override;
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	void ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const override;
	void ExportMid(FILE* fileMid) const override;

public:
	bool HasTag() const override;
	bool HasGeoCode() const override;
};

#include "Collection.cpp"