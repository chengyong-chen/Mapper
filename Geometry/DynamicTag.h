#pragma once
//#include "Global.h"

class CPoly;
class CGeom;
class CHint;
class CViewinfo;
enum ANCHOR : BYTE;

class AFX_EXT_CLASS CDynamicTag abstract : public CObject
{
public:
	CDynamicTag();

	~CDynamicTag() override;

public:
	CString m_string;

public:
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
	{
	};
};

class AFX_EXT_CLASS CPointTag : public CDynamicTag
{
public:
	Gdiplus::PointF m_point;
public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const override;
};

class AFX_EXT_CLASS CFixedTag : public CDynamicTag
{
public:
	Gdiplus::PointF m_point;
public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const override;
};

class AFX_EXT_CLASS CRectTag : public CDynamicTag
{
public:
	CRectTag();

public:
	Gdiplus::RectF m_rect;
	ANCHOR defAnchor;

public:
	CRect GetRect(const CRect& spotRect, const CSize& rimSize, const CSize& size, const BYTE& wAlign, const BYTE& wAnchor) const;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const override;
};

class AFX_EXT_CLASS CGroupTag : public CDynamicTag
{
public:
	CGroupTag();

	~CGroupTag() override;

public:
	CObList m_geomlist;
	bool m_bDelete;

public:
	static CGroupTag* GetGroup(CObList& list, const CGeom* pGeom);

public:
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot, const bool& bOblige);

	class CWord : public CObject
	{
	public:
		CWord()
		{
			length = 0;
			bEnglish = false;
		}

	public:
		CString str;
		int length;
		bool bEnglish;
	};
};