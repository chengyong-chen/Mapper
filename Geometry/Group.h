#pragma once

#include "Collection.h"

using namespace std;
using namespace Clipper2Lib;

class CGeom;
class CLine;
class CFill;
enum ANCHOR : BYTE;

class AFX_EXT_CLASS CGroup sealed : public CCollection<CGeom>
{
protected:
	DECLARE_SERIAL(CGroup)
	CGroup();
	CGroup(CLine* pLine, CFillCompact* pFill);

public:
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot, const bool& bOblige) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const override;
	void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const override;

public:
	void CreateTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign) override;
	void CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect, const int& sizeFont) override;
	void SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign) override;
	void DeleteTag() override;

public:
	void InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const override;
public:
	bool Tagable() const override;
public:
	~CGroup() override;
};
