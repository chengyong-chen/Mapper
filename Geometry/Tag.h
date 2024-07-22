#pragma once

#include "Global.h"
#include "../Dataview/viewinfo.h"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

class CHint;
class CDatainfo;
class CDatainfo;
class CViewinfo;
struct Context;

class AFX_EXT_CLASS CTag
{
public:
	CTag(CString& str, ANCHOR wAnchor);
	virtual ~CTag();

private:
	ANCHOR m_anchor;
	HALIGN m_hAlign;
	VALIGN m_vAlign;

public:
	CString& m_string;

public:
	bool m_bModified;
public:
	virtual void SetAnchor(ANCHOR anchor);
	virtual ANCHOR GetAnchor() const { return m_anchor; };
	virtual BYTE GetShort() const
	{
		BYTE byte = m_hAlign;
		byte |= (m_vAlign<<2) &0B00001100;
		byte |= (m_anchor<<4) &0B11110000;
		return byte;
	}
public:
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const Gdiplus::PointF& point) const;
	virtual void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const Gdiplus::PointF& point, const CHint* pHint) const;

public:
	template<typename T1, typename T2>
	T2 GetAnchorPoint(const CViewinfo& viewinfo, CRect docRect) const	
	{
		T1 rect = viewinfo.DocToClient<T1>(docRect);
		return CTag::GetAnchorPoint<T1, T2>(rect, m_anchor);
	};

public:
	virtual CTag* Clone(CString& str) const;

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual DWORD ReleaseCE(CFile& file, const BYTE& type) const;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const Gdiplus::PointF& origin, const CHint* pHint) const;
	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const Gdiplus::PointF& origin, const CHint* pHint) const;

public:
	static void GetDefaultAlign(ANCHOR anchor, HALIGN& hAlign, VALIGN& vAlign);

	template<typename T1, typename T2>
	static T2 GetAnchorPoint(T1 rect, ANCHOR anchor)
	{
		T2 point;
		switch(anchor)
		{
		case ANCHOR_1:
			point.X = rect.GetLeft();
			point.Y = rect.GetTop();
			break;
		case ANCHOR_2:
			point.X = rect.GetLeft()+rect.Width/2;
			point.Y = rect.GetTop();
			break;
		case ANCHOR_3:
			point.X = rect.GetRight();
			point.Y = rect.GetTop();
			break;
		case ANCHOR_4:
			point.X = rect.GetLeft();
			point.Y = rect.GetTop()+rect.Height/2;
			break;
		case ANCHOR_5:
			point.X = rect.GetLeft()+rect.Width/2;
			point.Y = rect.GetTop()+rect.Height/2;
			break;
		case ANCHOR_6:
			point.X = rect.GetRight();
			point.Y = rect.GetTop()+rect.Height/2;
			break;
		case ANCHOR_7:
			point.X = rect.GetLeft();
			point.Y = rect.GetBottom();
			break;
		case ANCHOR_8:
			point.X = rect.GetLeft()+rect.Width/2;
			point.Y = rect.GetBottom();
			break;
		case ANCHOR_9:
			point.X = rect.GetRight();
			point.Y = rect.GetBottom();
			break;
		}

		return point;
	}
};
