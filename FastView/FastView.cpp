
#include "stdafx.h"
#include "Fast.h"
#include "FastDoc.h"
#include "..\Image\Dib.h"

#include "FastView.h"
#include "mainfrm.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
 

/////////////////////////////////////////////////////////////////////////////
// CFastView

IMPLEMENT_DYNCREATE(CFastView, CScrollView)

BEGIN_MESSAGE_MAP(CFastView, CScrollView)
	//{{AFX_MSG_MAP(CFastView)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastView construction/destruction


CFastView::CFastView()
{
	m_bCenter=TRUE;
    WindowSize=CSize(500,500);	
	m_Zoom=1;
	m_nMapMode=MM_TEXT;
}

CFastView::~CFastView()
{
}

void CFastView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();	    
	
	m_Zoom=1;
	m_nMapMode=MM_TEXT;

	SetViewWindowSize(0,0);
}

void CFastView::SetViewWindowSize(CPoint docpoint,CPoint devipoint)
{
	SIZE      ViewSize;
	CClientDC dc(nullptr);	
		
	CFastDoc* TheDoc=GetDocument();
	CDib *TheDib=TheDoc->GetDib();
	if(TheDib!=nullptr)
	{
		WindowSize =TheDib->GetSize();
	
		ViewSize.cx=WindowSize.cx*m_Zoom;
		ViewSize.cy=WindowSize.cy*m_Zoom;
		
		SetScrollSizes(MM_TEXT, ViewSize,CSize(80,80),CSize(10,10));
		SetScrollPos(SB_HORZ,docpoint.x*ViewSize.cx/WindowSize.cx-devipoint.x);
		SetScrollPos(SB_VERT,docpoint.y*ViewSize.cy/WindowSize.cy-devipoint.y);
	}
}

void CFastView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo); 

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportExt(int(72*m_Zoom),int(72*m_Zoom));
	pDC->SetWindowExt(72,-72);	

	CFastDoc* TheDoc=GetDocument();
	CDib *TheDib=TheDoc->GetDib();
	if(TheDib!=nullptr)
	{
		WindowSize =TheDib->GetSize();
		pDC->SetWindowOrg(0,WindowSize.cy);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFastView drawing

void CFastView::OnDraw(CDC* pDC)
{
	CFastDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//���ڵ�ɫ���д������о�

	pDoc->Draw(pDC,this);
}


void CFastView::ClientToDoc(const CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, nullptr);
	dc.DPtoLP(&point);
}

void CFastView::ClientToDoc(CRect& rect)
{
	int diversion = rect.bottom;
	rect.bottom   = rect.top;
	rect.top      = diversion;

	CClientDC dc(this);
	OnPrepareDC(&dc, nullptr);
	dc.DPtoLP(rect);
	ASSERT(rect.left <= rect.right);
	ASSERT(rect.bottom >= rect.top);
}

void CFastView::DocToClient(const CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, nullptr);
	dc.LPtoDP(&point);
}


void CFastView::DocToClient(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, nullptr);
	dc.LPtoDP(rect);
	rect.NormalizeRect();
}




/////////////////////////////////////////////////////////////////////////////
// CFastView message handlers



void CFastView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default


	CPoint local = point;
	ClientToDoc(local);
	float zoommuti=m_Zoom/2;
	if(zoommuti<0.25)
		return;

	m_Zoom=zoommuti;
	SetViewWindowSize(local,point);
	Invalidate();
	CScrollView::OnRButtonDown(nFlags, point);
}
void CFastView::OnLButtonDown(UINT nFlags, CPoint point)
{

	// TODO: Add your message handler code here and/or call default
	CPoint local = point;
	ClientToDoc(local);
	float zoommuti=m_Zoom*2;
	if(zoommuti>8)
		return;

	m_Zoom=zoommuti;
	SetViewWindowSize(local,point);
	Invalidate();
	CScrollView::OnLButtonDown(nFlags, point);
}




/////////////////////////////////////////////////////////////////////////////
// CFastView diagnostics

#ifdef _DEBUG
void CFastView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CFastView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

BOOL CFastView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CFastView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CScrollView::OnBeginPrinting(pDC, pInfo);
}


void CFastView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrint(pDC, pInfo);
}


void CFastView::SetScrollSizes(int nMapMode, SIZE sizeTotal,const SIZE& sizePage, const SIZE& sizeLine)
{
	ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
	ASSERT(nMapMode > 0);
	ASSERT(nMapMode != MM_ISOTROPIC && nMapMode != MM_ANISOTROPIC);

	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;

	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(nullptr);
		ASSERT(m_nMapMode > 0);
		dc.SetMapMode(m_nMapMode);

		// total size
		m_totalDev = m_totalLog;
//		dc.LPtoDP((LPPOINT)&m_totalDev); this line is very important!
		m_pageDev = sizePage;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = sizeLine;
		dc.LPtoDP((LPPOINT)&m_lineDev);
		if(m_totalDev.cy < 0)
			m_totalDev.cy = -m_totalDev.cy;
		if(m_pageDev.cy < 0)
			m_pageDev.cy = -m_pageDev.cy;
		if(m_lineDev.cy < 0)
			m_lineDev.cy = -m_lineDev.cy;
	} // release DC here

	// now adjust device specific sizes
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if(m_pageDev.cx == 0)
		m_pageDev.cx = m_totalDev.cx/10;
	if(m_pageDev.cy == 0)
		m_pageDev.cy = m_totalDev.cy/10;
	if(m_lineDev.cx == 0)
		m_lineDev.cx = m_pageDev.cx/10;
	if(m_lineDev.cy == 0)
		m_lineDev.cy = m_pageDev.cy/10;

	if(m_hWnd != nullptr)
	{
		// window has been created, invalidate now
		UpdateBars();
		if(nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
}


