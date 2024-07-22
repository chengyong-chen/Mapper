#include "stdafx.h"
#include "Ruler.h"
#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CRuler::CRuler()
{
	m_bVisible = false;
	m_bUnit = true;

	m_nClass = 0;
	m_nXpixels = 0;
	m_nYpixels = 0;
}

CRuler::~CRuler()
{
}

bool CRuler::Reset(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_bVisible==false)
		return false;

	CRect client;
	pWnd->GetClientRect(client);
	CPoint cPoint = client.CenterPoint();
	cPoint.x = client.left+client.Width()/2;
	cPoint.y = client.top+client.Height()/2;
	const CPoint lPoint = CPoint(client.left, cPoint.y);
	const CPoint rPoint = CPoint(client.right, cPoint.y);
	const CPoint tPoint = CPoint(cPoint.x, client.top);
	const CPoint bPoint = CPoint(cPoint.x, client.bottom);

	CPoint points[2];
	points[0] = lPoint;
	points[1] = rPoint;
	double hlength = viewinfo.m_datainfo.CalArea(points, 2);
	if(hlength==0.0f)
	{
		return false;
	}

	points[0] = tPoint;
	points[1] = bPoint;
	double vlength = viewinfo.m_datainfo.CalArea(points, 2);
	if(vlength==0.0f)
	{
		return false;
	}
	if(hlength<1||vlength<1)
	{
		hlength *= 1000;
		vlength *= 1000;
		m_bUnit = false;
	}
	else
	{
		m_bUnit = true;
	}

	pWnd->GetClientRect(client);
	const double fXpixels = (double)client.Width()/hlength;
	const double fYpixels = (double)client.Height()/vlength;

	int nXClass = 1;
	int nYClass = 1;
	while(fXpixels*nXClass<40)
		nXClass *= 10;
	while(fYpixels*nYClass<40)
		nYClass *= 10;

	m_nClass = max(nXClass, nYClass);
	m_nXpixels = (unsigned short)(fXpixels*m_nClass);
	m_nYpixels = (unsigned short)(fYpixels*m_nClass);
	return true;
}

void CRuler::DrawH(CView* pView, CMDIChildWnd* pChildfrm, CPoint start) const
{
	if(m_bVisible==false)
		return;

	CClientDC dc(pChildfrm);

	CPen* OldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
	CRect client;
	pView->GetClientRect(client);
	dc.Rectangle(13, 0, client.right+13, 13);
	dc.SelectObject(OldPen);

	CFont font;
	font.CreatePointFont(55, _T("Arial"), nullptr);

	CFont* OldFont = (CFont*)dc.SelectObject(&font);
	const UINT oldAlign = dc.SetTextAlign(TA_LEFT|TA_TOP);
	dc.SetBkMode(TRANSPARENT);

	long xnum = 0;
	start.x += 13;
	while(start.x<0)
	{
		start.x += m_nXpixels;
		xnum++;
	}
	while(start.x>0)
	{
		start.x -= m_nXpixels;
		xnum--;
	}

	OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	for(short x = start.x; x<client.right+13; x += m_nXpixels)
	{
		dc.MoveTo(x, 11);
		dc.LineTo(x, 0);
		TCHAR string[10];
		_itot(abs(xnum*m_nClass), string, 10);
		dc.TextOut(x+2, 0, string, lstrlen(string));

		for(BYTE i = 1; i<10; i++)
		{
			const unsigned short Δ = (long)(m_nXpixels*i)/10;

			dc.MoveTo(x+Δ, 11);
			if(i==5)
				dc.LineTo(x+Δ, 3);
			else
				dc.LineTo(x+Δ, 6);
		}
		xnum++;
	}
	dc.MoveTo(13, 11);
	dc.LineTo(13, 0);

	dc.SelectObject(OldPen);
	dc.SetTextAlign(oldAlign);
	dc.SelectObject(OldFont);
	dc.SelectObject(OldBrush);
	font.DeleteObject();

	font.CreatePointFont(90, _T("Arial"), nullptr);
	OldFont = (CFont*)dc.SelectObject(&font);
	OldPen = (CPen*)dc.SelectStockObject(NULL_PEN);

	dc.Rectangle(0, 0, 13, 13);
	if(m_bUnit==true)
		dc.TextOut(1, 1, _T("Km"), 2);
	else
		dc.TextOut(3, 1, _T("m"), 2);

	dc.SelectObject(OldFont);
	font.DeleteObject();
}

void CRuler::DrawV(CView* pView, CMDIChildWnd* pChildfrm, CPoint start) const
{
	if(m_bVisible==false)
		return;

	CClientDC dc(pChildfrm);

	CPen* OldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
	CRect client;
	pView->GetClientRect(client);
	dc.Rectangle(0, 13, 13, client.bottom+13);
	dc.SelectObject(OldPen);

	CFont font;
	font.CreatePointFont(55, _T("Arial"), nullptr);

	CFont* OldFont = (CFont*)dc.SelectObject(&font);
	const UINT oldAlign = dc.SetTextAlign(TA_LEFT|TA_TOP);
	dc.SetBkMode(TRANSPARENT);

	long ynum = 0;
	start.y += 13;
	while(start.y<0)
	{
		start.y += m_nYpixels;
		ynum--;
	}
	while(start.y>0)
	{
		start.y -= m_nYpixels;
		ynum++;
	}

	OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	for(short y = start.y; y<client.bottom+13; y += m_nYpixels)
	{
		dc.MoveTo(11, y);
		dc.LineTo(0, y);
		TCHAR string[10];
		_itot(abs(ynum*m_nClass), string, 10);
		dc.TextOut(0, y+2, string, lstrlen(string));

		for(BYTE i = 1; i<10; i++)
		{
			const long Δ = (long)(m_nYpixels*i)/10;

			dc.MoveTo(11, y+Δ);
			if(i==5)
				dc.LineTo(3, y+Δ);
			else
				dc.LineTo(6, y+Δ);
		}
		ynum--;
	}
	dc.MoveTo(0, 13);
	dc.LineTo(12, 13);

	dc.SelectObject(OldPen);
	dc.SetTextAlign(oldAlign);
	dc.SelectObject(OldFont);
	dc.SelectObject(OldBrush);
	font.DeleteObject();

	font.CreatePointFont(90, _T("Arial"), nullptr);
	OldFont = (CFont*)dc.SelectObject(&font);
	OldPen = (CPen*)dc.SelectStockObject(NULL_PEN);

	dc.Rectangle(0, 0, 13, 13);
	if(m_bUnit==true)
		dc.TextOut(1, 1, _T("Km"), 2);
	else
		dc.TextOut(3, 1, _T("m"), 2);

	dc.SelectObject(OldFont);
	font.DeleteObject();
}

void CRuler::ShowSwitch(CView* pView, const CViewinfo& viewinfo)
{
	if(m_bVisible==true)
	{
		m_bVisible = false;

		CRect rect;
		pView->GetWindowRect(rect);
		pView->MoveWindow(0, 0, rect.Width()+12, rect.Height()+12, TRUE);
	}
	else
	{
		m_bVisible = true;

		CRect rect;
		pView->GetWindowRect(rect);
		pView->MoveWindow(12, 12, rect.Width()-12, rect.Height()-12, TRUE);

		if(Reset(pView, viewinfo)==false)
		{
			pView->MoveWindow(0, 0, rect.Width()+12, rect.Height()+12, TRUE);
			m_bVisible = false;
		}
	}
}
