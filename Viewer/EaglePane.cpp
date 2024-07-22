#include "stdafx.h"
#include "Viewer.h"
#include "EaglePane.h"

#include "..\Mapper\Global.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

// CEaglePane
IMPLEMENT_DYNAMIC(CEaglePane, CDockablePane)

CEaglePane::CEaglePane()
{
	m_fHScale = 1.0;	
	m_fVScale = 1.0;	
	m_vRect = CRect(0, 0, 0, 0);
	m_Image = nullptr;
	m_bDown = FALSE;
	m_pView = nullptr;
}

CEaglePane::~CEaglePane()
{
	delete m_Image;
	
	m_pView = nullptr;
}


BEGIN_MESSAGE_MAP(CEaglePane, CDockablePane)
	//{{AFX_MSG_MAP(CEaglePane)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
// CEaglePane message handlers

void CEaglePane::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(m_Image != nullptr)
	{
		CPen* oldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
		CBrush* oldBrush = (CBrush*)dc.SelectStockObject(LTGRAY_BRUSH);

		CRect rect;
		GetClientRect(&rect);
		rect.InflateRect(1,1);
		dc.Rectangle(rect);

		dc.SelectObject(oldBrush);
		dc.SelectObject(oldPen);
	
		CRect bmpRect;
		CRect vRect = m_vRect;
		
		CSize size = CSize(m_Image->GetWidth(),m_Image->GetHeight());		
		int x = (rect.Width()  - size.cx)/2;
		int y = (rect.Height() - size.cy)/2;

		bmpRect = CRect(CPoint(x,y),size);
		
		Graphics g(dc.m_hDC);
		g.DrawImage(m_Image,x, y,size.cx,size.cy);
		g.ReleaseHDC(dc.m_hDC);

		vRect.OffsetRect(x,y);

		CPen pen(PS_SOLID, 1, (COLORREF)RGB(255, 0, 0));
		oldPen = dc.SelectObject(&pen);
		oldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

		dc.IntersectClipRect(bmpRect);

		dc.Rectangle(vRect);

		CPoint center = vRect.CenterPoint();
		dc.MoveTo(center.x,vRect.top);
		dc.LineTo(center.x,vRect.top-4);
		dc.MoveTo(center.x,vRect.bottom);
		dc.LineTo(center.x,vRect.bottom+3);

		dc.MoveTo(vRect.left,   center.y);
		dc.LineTo(vRect.left-4, center.y);
		dc.MoveTo(vRect.right,  center.y);
		dc.LineTo(vRect.right+3,center.y);

		dc.SelectObject(oldBrush);
		dc.SelectObject(oldPen);
		pen.DeleteObject();	
	}
}

void CEaglePane::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CWnd::OnLButtonDown(nFlags, point);

	if(m_Image != nullptr)
	{
		CRect vRect = m_vRect; 
	
		CRect rect;
		GetClientRect(&rect);

		CSize size = CSize(m_Image->GetWidth(),m_Image->GetHeight());		
		int x = (rect.Width()  - size.cx)/2;
		int y = (rect.Height() - size.cy)/2;
		CRect bmpRect = CRect(CPoint(x,y),size);

		if(bmpRect.PtInRect(point) == false)
		{
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}

		vRect.OffsetRect(x,y);
	
		m_bDown = TRUE;
		vRect.InflateRect(2, 2);

		if(!vRect.PtInRect(point))
		{	
			m_ptDown = vRect.CenterPoint();
			OnMouseMove(nFlags,point);
		}
		else
			m_ptDown = point;

		SetCapture();
	}
}

void CEaglePane::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CWnd::OnMouseMove(nFlags, point);

	if(m_Image != nullptr && m_bDown == TRUE)
	{
		CRect rect;
		GetClientRect(&rect);
		CRect bmpRect = rect;
		CRect vRect = m_vRect;

		CSize size = CSize(m_Image->GetWidth(),m_Image->GetHeight());		
		int x = (rect.Width()  - size.cx)/2;
		int y = (rect.Height() - size.cy)/2;
		bmpRect = CRect(CPoint(x,y),size);
		
		vRect.OffsetRect(x,y);
		
		CSize ? = point - m_ptDown;
			
		if(vRect.left + ?.cx < bmpRect.left)
			?.cx = bmpRect.left - vRect.left;
		if(vRect.right + ?.cx > bmpRect.right)
			?.cx = bmpRect.right-vRect.right;

		if(vRect.top + ?.cy < bmpRect.top)
			?.cy = bmpRect.top - vRect.top;
		if(vRect.bottom + ?.cy > bmpRect.bottom)
			?.cy = bmpRect.bottom-vRect.bottom;

		m_vRect.OffsetRect(?);
		m_ptDown.Offset(?);
			
		Invalidate();		
	}
}

void CEaglePane::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDown == false || GetCapture() != this || m_pView == nullptr)
	{
		ReleaseCapture();
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	if(m_Image != nullptr && m_bDown == TRUE)
	{
		ReleaseCapture();	

		point = m_vRect.CenterPoint();

		point.x = (long)(point.x/m_fHScale);
		point.y = (long)(point.y/m_fVScale);
		point.y = m_docSize.cy - point.y;

		if(m_pView != nullptr && m_pView->m_hWnd != nullptr)
		{
			CRect cliRect;
			m_pView->GetClientRect(&cliRect);
			CRect docRect = ((CGrfView*)m_pView)->m_Viewinfo.ClientToDoc(cliRect);

			CPoint cPoint = docRect.CenterPoint();
			CSize ? = point - cPoint;
			
			CPoint delta1(0,0);
			CPoint delta2(?.cx,?.cy);
			matrixDoctoCli.TransformPoints(&delta1);
			matrixDoctoCli.TransformPoints(&delta2);
			?.cx = delta2.x - delta1.x;
			?.cy = delta2.y - delta1.y;

			m_pView->OnScrollBy(?);
			m_bDown = FALSE;
		}
		else
		{
			m_pView = nullptr;
		}
	}

	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CEaglePane::SetRect(CView* pView,CRect cliRect)
{	
	if(m_Image != nullptr && pView == m_pView)
	{
		m_vRect = cliRect;
		
		m_vRect.left   = (long)(m_vRect.left  *m_fHScale);
		m_vRect.top    = (long)(m_vRect.top   *m_fVScale);
		m_vRect.right  = (long)(m_vRect.right *m_fHScale);
		m_vRect.bottom = (long)(m_vRect.bottom*m_fVScale);

		Invalidate();
	}
}
              
void CEaglePane::OpenBmp(CView* pView,CString strBmp,CSize docSize)
{
	m_pView = pView;
	m_docSize = docSize;

	if(strBmp != m_strBmp)
	{
		m_strBmp.Empty();
		delete m_Image;
		m_Image = nullptr;
		
		if(_taccess(strBmp,00) != -1)
		{
			_bstr_t btrBmp(strBmp);
			m_Image = Image::FromFile(btrBmp);
			this->ShowWindow(SW_SHOW);
		}
		else
		{
			this->ShowWindow(SW_HIDE);
		}

		m_strBmp = strBmp;
	}
			
	CSize bmpSize;
	if(m_Image != nullptr)
	{
		bmpSize.cx = m_Image->GetWidth();
		bmpSize.cy = m_Image->GetHeight();

		m_fHScale = (double)bmpSize.cx/docSize.cx;
		m_fVScale = (double)bmpSize.cy/docSize.cy;	
	}		

	CPaintDC dc(this);
	CPen* oldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
	CBrush* oldBrush = (CBrush*)dc.SelectStockObject(LTGRAY_BRUSH);
	CRect cliRect;
	GetClientRect(&cliRect);
	
	dc.Rectangle(cliRect);

	dc.SelectObject(oldBrush);
	dc.SelectObject(oldPen);
}