#include "stdafx.h"
#include <math.h>

#include "Ruler.h"
#include "../Public/Global.h"
#include "../Public/Function.h"

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRuler::CRuler()
{
	m_bVisible = FALSE;
	m_last = CPoint(-100, -100);
	m_origin = CPointF(0, 0);
}

CRuler::~CRuler()
{
}

void CRuler::Reset(CView* pView, CPointF origin, float factorMapToView)
{
	m_origin = origin;
	const double fClass = log10(30.f/factorMapToView);
	m_nClass = ceil(fClass);
	m_pixels = factorMapToView*pow(10.f, m_nClass);
}

void CRuler::DrawH(CView* pView, CWnd* pParent, Gdiplus::Point start)
{
	if(!m_bVisible)
		return;
	if(pParent==nullptr)
		return;
	if(pParent->m_hWnd==nullptr)
		return;
	if(pView==nullptr)
		return;
	if(pView->m_hWnd==nullptr)
		return;
	if(m_pixels==0)
		return;

	CClientDC dc(pParent);

	CFont font;
	font.CreatePointFont(60, _T("Arial"), nullptr);
	CGdiObject* OldFont = dc.SelectObject(&font);
	CGdiObject* OldPen = dc.SelectStockObject(NULL_PEN);
	CGdiObject* OldBrush = dc.SelectStockObject(WHITE_BRUSH);

	CRect rect;
	pView->GetWindowRect(rect);
	pParent->ScreenToClient(&rect);
	dc.Rectangle(rect.left-13, rect.top-13, rect.right, rect.top);
	dc.IntersectClipRect(rect.left-13, rect.top-13, rect.right, rect.top);

	long xnum = 0;
	while(start.X<0)
	{
		start.X += m_pixels;
		xnum++;
	}
	while(start.X>0)
	{
		start.X -= m_pixels;
		xnum--;
	}

	dc.SetBkMode(TRANSPARENT);
	const UINT oldAlign = dc.SetTextAlign(TA_LEFT|TA_TOP);
	dc.SelectObject(OldPen);
	OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	for(long x = start.X+rect.left; x<rect.right; x += m_pixels)
	{
		if(x>=rect.left)
		{
			dc.MoveTo(x, rect.top);
			dc.LineTo(x, rect.top-11);
			CString str;
			str.Format(_T("%g"), xnum*pow(10.f, m_nClass));
			dc.TextOut(x+2, rect.top-15, str);
		}
		for(long i = 1; i<10; i++)
		{
			const long Δ = round(m_pixels*i/10.f);
			if(x+Δ<rect.left)
				continue;
			dc.MoveTo(x+Δ, rect.top);
			if(i==5)
				dc.LineTo(x+Δ, rect.top-9);
			else
				dc.LineTo(x+Δ, rect.top-7);
		}
		xnum++;
	}
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.left, -1);

	dc.MoveTo(rect.left, rect.top-1);
	dc.LineTo(rect.right, rect.top-1);

	dc.SelectObject(OldPen);
	dc.SetTextAlign(oldAlign);
	dc.SelectObject(OldFont);
	dc.SelectObject(OldBrush);
	font.DeleteObject();

	DrawMouse(dc, m_last);
}

void CRuler::DrawV(CView* pView, CWnd* pParent, Gdiplus::Point start)
{
	if(!m_bVisible)
		return;
	if(pParent==nullptr)
		return;
	if(pParent->m_hWnd==nullptr)
		return;
	if(pView==nullptr)
		return;
	if(pView->m_hWnd==nullptr)
		return;
	if(m_pixels==0)
		return;

	CClientDC dc(pParent);
	CFont font;
	font.CreateFont(9, 0, 2700, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Arial"));
	CGdiObject* OldFont = dc.SelectObject(&font);
	CGdiObject* OldPen = dc.SelectStockObject(NULL_PEN);
	CGdiObject* OldBrush = dc.SelectStockObject(WHITE_BRUSH);

	CRect rect;
	pView->GetWindowRect(rect);
	pParent->ScreenToClient(&rect);
	dc.Rectangle(rect.left-13, rect.top-13, rect.left, rect.bottom);
	dc.IntersectClipRect(rect.left-13, rect.top-13, rect.left, rect.bottom);

	long ynum = 0;
	while(start.Y<0)
	{
		start.Y += m_pixels;
		ynum--;
	}
	while(start.Y>0)
	{
		start.Y -= m_pixels;
		ynum++;
	}

	dc.SetBkMode(TRANSPARENT);
	//	UINT oldAlign = dc.SetTextAlign(TA_LEFT |TA_TOP);

	dc.SelectObject(OldPen);
	OldPen = dc.SelectStockObject(BLACK_PEN);
	for(long y = start.Y+rect.top; y<rect.bottom; y += m_pixels)
	{
		if(y>=rect.top)
		{
			dc.MoveTo(rect.left, y);
			dc.LineTo(rect.left-11, y);
			CString str;
			str.Format(_T("%g"), ynum*pow(10.f, m_nClass));
			dc.TextOut(rect.left-5, y+2, str);
		}
		for(long i = 1; i<10; i++)
		{
			const long Δ = round(m_pixels*i/10.f);
			if(y+Δ<rect.top)
				continue;
			dc.MoveTo(rect.left, y+Δ);
			if(i==5)
				dc.LineTo(rect.left-9, y+Δ);
			else
				dc.LineTo(rect.left-7, y+Δ);
		}
		ynum--;
	}
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(-1, rect.top);

	dc.MoveTo(rect.left-1, rect.top);
	dc.LineTo(rect.left-1, rect.bottom);

	dc.SelectObject(OldPen);
	//	dc.SetTextAlign(oldAlign);
	dc.SelectObject(OldBrush);
	dc.SelectObject(OldFont);
	font.DeleteObject();

	DrawMouse(dc, m_last);
}

void CRuler::DrawMouse(CClientDC& dc, const CPoint& point) const
{
	if(!m_bVisible)
		return;

	CGdiObject* OldPen = dc.SelectStockObject(BLACK_PEN);
	const int oldROP2 = dc.SetROP2(R2_NOTXORPEN);

	dc.MoveTo(12, point.y+13);
	dc.LineTo(0, point.y+13);

	dc.MoveTo(point.x+13, 12);
	dc.LineTo(point.x+13, 0);

	dc.SetROP2(oldROP2);
	dc.SelectObject(OldPen);
}

void CRuler::ShowSwitch(CWnd* pParent, CView* pView)
{
	CRect rect;
	pView->GetWindowRect(rect);
	pParent->ScreenToClient(rect);

	if(m_bVisible)
	{
		pView->MoveWindow(rect.left-13, rect.top-13, rect.Width()+13, rect.Height()+13, TRUE);
	}
	else
	{
		pView->MoveWindow(rect.left+13, rect.top+13, rect.Width()-13, rect.Height()-13, TRUE);
	}

	m_bVisible = 1-m_bVisible;
}

void CRuler::OnMouseMove(CWnd* pParent, CPoint point)
{
	if(!m_bVisible)
		return;
	if(pParent==nullptr)
		return;
	if(pParent->m_hWnd==nullptr)
		return;

	CClientDC dc(pParent);
	const int oldMapMode = dc.SetMapMode(MM_TEXT);
	const int oldBkMode = dc.SetBkMode(TRANSPARENT);

	DrawMouse(dc, m_last);
	DrawMouse(dc, point);

	m_last = point;

	dc.SetBkMode(oldBkMode);
	dc.SetMapMode(oldMapMode);
}
