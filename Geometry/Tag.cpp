#include "stdafx.h"
#include "Tag.h"
#include "Global.h"
#include "../Style/Hint.h"

#include "../Dataview/Viewinfo.h"

#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTag::CTag(CString& str, ANCHOR anchor)
	: m_string(str), m_anchor(anchor)
{
	CTag::GetDefaultAlign(anchor, m_hAlign, m_vAlign);

	m_bModified = false;
}
CTag::~CTag()
{
}
void CTag::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	BYTE byte=0X00;
	if(ar.IsStoring())
	{
		ar<<m_hAlign;
		ar<<m_vAlign;
		ar<<m_anchor;
		ar<<byte;
	}
	else
	{	
		ar>>m_hAlign;
		ar>>m_vAlign;
		ar>>m_anchor;
		ar>>byte;
		CTag::GetDefaultAlign(m_anchor, m_hAlign, m_vAlign);
	}
}
void CTag::SetAnchor(ANCHOR anchor)
{
	if(anchor!=m_anchor)
	{
		m_anchor = anchor;
		CTag::GetDefaultAlign(anchor, m_hAlign, m_vAlign);
		m_bModified = true;
	}
}
DWORD CTag::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = 0;
	if(pFile!=nullptr)
	{
		pFile->Write(&m_hAlign, sizeof(BYTE));
		size += sizeof(BYTE);
		pFile->Write(&m_vAlign, sizeof(BYTE));
		size += sizeof(BYTE);

		pFile->Write(&m_anchor, sizeof(BYTE));
		size += sizeof(BYTE);

		return size;
	}
	else
	{
		m_hAlign = (HALIGN)*(BYTE*)bytes;
		bytes += sizeof(BYTE);
		m_vAlign = (VALIGN)*(BYTE*)bytes;
		bytes += sizeof(BYTE);

		m_anchor = (ANCHOR)*bytes;
		bytes += sizeof(BYTE);
		return 0;
	}
}
DWORD CTag::ReleaseCE(CFile& file, const BYTE& type) const
{
	const DWORD size = 0;

	return size;
}
void CTag::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const Gdiplus::PointF& point) const
{
	if(m_string.IsEmpty()==TRUE)
		return;
	if(pHint==nullptr)
		return;

	pHint->DrawString(g, viewinfo, m_string, point, 0, m_hAlign, m_vAlign);
}
void CTag::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const Gdiplus::PointF& point, const CHint* pHint) const
{
	if(pWnd==nullptr)
		return;
	if(pHint==nullptr)
		return;

	CDC* pDC = pWnd->GetDC();
	Gdiplus::Graphics g(pDC->m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	
	const Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, m_string, 0);
	const Gdiplus::RectF tagRect = ::GetTagRect(point,textSize, m_hAlign, m_vAlign);
	pWnd->InvalidateRect(CRect(tagRect.GetLeft(), tagRect.GetTop(), tagRect.GetRight(), tagRect.GetBottom()), FALSE);

	g.ReleaseHDC(pDC->m_hDC);
	pWnd->ReleaseDC(pDC);
}
CTag* CTag::Clone(CString& str) const
{
	CTag* pClone = new CTag(str, m_anchor);
	pClone->m_anchor = m_anchor;
	pClone->m_hAlign = m_hAlign;
	pClone->m_vAlign = m_vAlign;
	return pClone;
}
void CTag::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const Gdiplus::PointF& origin, const CHint* pHint) const
{
	if(m_string.IsEmpty()==TRUE)
		return;

	pHint->ExportTag(file, viewinfo, aiKey, origin, 0, m_hAlign, m_vAlign, m_string);
}
void CTag::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const Gdiplus::PointF& origin, const CHint* pHint) const
{
	if(m_string.IsEmpty()==TRUE)
		return;

	if(pHint==nullptr)
		return;

	pHint->ExportPdf(pdf, page);
	pHint->ExportTag(pdf, page, viewinfo, origin, 0, m_hAlign, m_vAlign, m_string);
}
void CTag::GetDefaultAlign(ANCHOR anchor, HALIGN& hAlign, VALIGN& vAlign)
{
	switch(anchor)
	{
	case ANCHOR_1:
		hAlign = HALIGN::HA_RIGHT;
		vAlign = VALIGN::VA_BOTTOM;
		break;
	case ANCHOR_2:
		hAlign = HALIGN::HA_CENTER;
		vAlign = VALIGN::VA_BOTTOM;
		break;
	case ANCHOR_3:
		hAlign = HALIGN::HA_LEFT;
		vAlign = VALIGN::VA_BOTTOM;
		break;
	case ANCHOR_4:
		hAlign = HALIGN::HA_RIGHT;
		vAlign = VALIGN::VA_CENTER;
		break;
	case ANCHOR_5:
		hAlign = HALIGN::HA_CENTER;
		vAlign = VALIGN::VA_CENTER;
		break;
	case ANCHOR_6:
		hAlign = HALIGN::HA_LEFT;
		vAlign = VALIGN::VA_CENTER;
		break;
	case ANCHOR_7:
		hAlign = HALIGN::HA_RIGHT;
		vAlign = VALIGN::VA_TOP;
		break;
	case ANCHOR_8:
		hAlign = HALIGN::HA_CENTER;
		vAlign = VALIGN::VA_TOP;
		break;
	case ANCHOR_9:
		hAlign = HALIGN::HA_LEFT;
		vAlign = VALIGN::VA_TOP;
		break;
	}
}