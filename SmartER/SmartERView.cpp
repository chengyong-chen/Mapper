#include "stdafx.h"
#include "SmartER.h"


#include "SmartERDoc.h"
#include "SmartERView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmartERView

IMPLEMENT_GWDYNCREATE(CSmartERView, CScrollView)

BEGIN_MESSAGE_MAP(CSmartERView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSmartERView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSmartERView construction/destruction

CSmartERView::CSmartERView(CObject& parameter)
	:m_document((CSmartERDoc&)parameter),m_viewMonitor(m_document.m_Datainfo),m_viewPrinter(m_document.m_Datainfo)
{
	// TODO: add construction code here

		m_viewMonitor.m_bViewer = false;
		m_viewPrinter.m_bViewer = false;
}

CSmartERView::~CSmartERView()
{
}

BOOL CSmartERView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return CScrollView::PreCreateWindow(cs);
}
void CSmartERView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();	

	CRectF mapRect = m_viewMonitor.m_Data.GetMapRect();
	m_viewMonitor.ScaleAtTo(this,m_document.m_Datainfo.m_scaleSource,mapRect.CenterPoint());	
}
// CSmartERView drawing

void CSmartERView::OnDraw(CDC* pDC)
{
	if(pDC->IsPrinting() == FALSE)
	{
		CRect clipBox; 
		pDC->GetClipBox(clipBox);
		clipBox.NormalizeRect();
		if(clipBox.IsRectNull())
			return;

		m_viewMonitor.m_sizeDPI = CSize(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSY));		
		

		Bitmap bitmap(clipBox.Width(),clipBox.Height(),PixelFormat32bppARGB);
		Graphics g1(&bitmap);		
		g1.SetPageUnit(Unit::UnitPixel);

		HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
		LONG nStyle = ::GetWindowLong(hWnd,GWL_EXSTYLE);
		if((nStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		{
			g1.Clear(Color(255, 255, 255));
			g1.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);
			g1.SetInterpolationMode(InterpolationMode::InterpolationModeHighQualityBicubic);
			g1.SetPixelOffsetMode(PixelOffsetMode::PixelOffsetModeHighQuality);
			g1.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);
		}
		else
		{
			g1.Clear(Color(255, 255, 254));
		}
		g1.TranslateTransform(-clipBox.left,-clipBox.top);

		CRect inRect = CRect(-m_viewMonitor.m_ptViewPos,m_viewMonitor.m_sizeView);
		inRect.IntersectRect(inRect,clipBox);
		inRect = m_viewMonitor.ClientToDoc(inRect);         

		m_viewMonitor.Draw(g1);
		m_document.Draw(this,m_viewMonitor,g1,inRect);			
		
			
		Graphics g2(pDC->m_hDC);	
		g2.DrawImage(&bitmap,clipBox.left,clipBox.top,clipBox.Width(),clipBox.Height());
		g2.ReleaseHDC(pDC->m_hDC);
	}
	else
	{
		CRect clipBox; 
		pDC->GetClipBox(&clipBox);
		CRect inRect = m_viewPrinter.ClientToDoc(clipBox);         
		if(inRect.IsRectEmpty() == TRUE)
			return;		

		Graphics g(pDC->m_hDC);
		g.SetPageUnit(Unit::UnitPixel);
		g.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);
		g.SetInterpolationMode(InterpolationMode::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(PixelOffsetMode::PixelOffsetModeHighQuality);
		g.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);
	//	g.SetClip(Rect(inRect.left,inRect.top,inRect.Width(),inRect.Height()));//��Ҫ�ָ�
		m_viewPrinter.ResetProjection(m_viewMonitor.m_pProjection, true);
		m_viewPrinter.m_sizeDPI = CSize(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSY));		
		m_document.Draw(this,m_viewPrinter,g,inRect);
		m_viewPrinter.m_pProjection = nullptr;
		g.ReleaseHDC(pDC->m_hDC);
	}

	// TODO: add draw code for native data here
}

// CSmartERView printing


void CSmartERView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSmartERView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSmartERView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSmartERView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSmartERView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSmartERView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSmartERView diagnostics

#ifdef _DEBUG
void CSmartERView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSmartERView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG


// CSmartERView message handlers
