#include "stdafx.h"
#include "EagleWnd.h"
#include "../Image/Dib.h"
#include  <io.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEagleWnd

CEagleWnd::CEagleWnd()
{
	const HINSTANCE hInst = AfxGetInstanceHandle();
	const CString wndclass = _T("EagleWnd");
	WNDCLASS windowclass;
	if(::GetClassInfo(hInst, wndclass, &windowclass)==0)
	{
		windowclass.style = CS_DBLCLKS;// | CS_HREDRAW | CS_VREDRAW;
		windowclass.lpfnWndProc = ::DefWindowProc;
		windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
		windowclass.hInstance = hInst;
		windowclass.hIcon = nullptr;
		windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
		windowclass.lpszMenuName = nullptr;
		windowclass.lpszClassName = wndclass;
		if(!AfxRegisterClass(&windowclass))
		{
			AfxThrowResourceException();
		}
	}

	m_fHScale = 1.0;
	m_fVScale = 1.0;
	m_eyeRect = CRect(0, 0, 0, 0);
	m_Image = nullptr;
	m_bDown = FALSE;
	m_pView = nullptr;
}

CEagleWnd::~CEagleWnd()
{
	delete m_Image;

	m_pView = nullptr;
}


BEGIN_MESSAGE_MAP(CEagleWnd, CWnd)
	//{{AFX_MSG_MAP(CEagleWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEagleWnd message handlers
void CEagleWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if(m_Image!=nullptr)
	{
		CPen* oldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
		CBrush* oldBrush = (CBrush*)dc.SelectStockObject(LTGRAY_BRUSH);

		CRect rect;
		GetClientRect(&rect);
		rect.InflateRect(1, 1);
		dc.Rectangle(rect);

		dc.SelectObject(oldBrush);
		dc.SelectObject(oldPen);

		CRect bmpRect;
		CRect eyeRect = m_eyeRect;
		const CSize size = CSize(m_Image->GetWidth(), m_Image->GetHeight());
		const int x = (rect.Width()-size.cx)/2;
		const int y = (rect.Height()-size.cy)/2;

		bmpRect = CRect(CPoint(x, y), size);
		eyeRect.OffsetRect(x, y);

		Gdiplus::Graphics g(dc.m_hDC);
		g.DrawImage(m_Image, x, y, size.cx, size.cy);
		g.ReleaseHDC(dc.m_hDC);

		CPen pen(PS_SOLID, 1, (COLORREF)RGB(255, 0, 0));
		oldPen = dc.SelectObject(&pen);
		oldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

		dc.IntersectClipRect(bmpRect);

		dc.Rectangle(eyeRect);
		const CPoint center = eyeRect.CenterPoint();
		dc.MoveTo(center.x, eyeRect.top);
		dc.LineTo(center.x, eyeRect.top-4);
		dc.MoveTo(center.x, eyeRect.bottom);
		dc.LineTo(center.x, eyeRect.bottom+3);

		dc.MoveTo(eyeRect.left, center.y);
		dc.LineTo(eyeRect.left-4, center.y);
		dc.MoveTo(eyeRect.right, center.y);
		dc.LineTo(eyeRect.right+3, center.y);

		dc.SelectObject(oldBrush);
		dc.SelectObject(oldPen);
		pen.DeleteObject();
	}
}

void CEagleWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	if(m_Image!=nullptr)
	{
		CRect eyeRect = m_eyeRect;

		CRect rect;
		GetClientRect(&rect);
		const CSize size = CSize(m_Image->GetWidth(), m_Image->GetHeight());
		const int x = (rect.Width()-size.cx)/2;
		const int y = (rect.Height()-size.cy)/2;
		const CRect bmpRect = CRect(CPoint(x, y), size);
		eyeRect.OffsetRect(x, y);
		eyeRect.InflateRect(2, 2);

		if(bmpRect.PtInRect(point)==false)
		{
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}


		m_bDown = TRUE;
		if(!eyeRect.PtInRect(point))
		{
			m_ptDown = eyeRect.CenterPoint();
			OnMouseMove(nFlags, point);
		}
		else
			m_ptDown = point;

		SetCapture();
	}
}

void CEagleWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if(m_Image!=nullptr&&m_bDown==TRUE)
	{
		CRect rect;
		GetClientRect(&rect);
		CRect bmpRect = rect;
		CRect eyeRect = m_eyeRect;
		const CSize size = CSize(m_Image->GetWidth(), m_Image->GetHeight());
		const int x = (rect.Width()-size.cx)/2;
		const int y = (rect.Height()-size.cy)/2;
		bmpRect = CRect(CPoint(x, y), size);

		eyeRect.OffsetRect(x, y);

		CSize Δ = point-m_ptDown;

		if(eyeRect.left+Δ.cx<bmpRect.left)
			Δ.cx = bmpRect.left-eyeRect.left;
		if(eyeRect.right+Δ.cx>bmpRect.right)
			Δ.cx = bmpRect.right-eyeRect.right;

		if(eyeRect.top+Δ.cy<bmpRect.top)
			Δ.cy = bmpRect.top-eyeRect.top;
		if(eyeRect.bottom+Δ.cy>bmpRect.bottom)
			Δ.cy = bmpRect.bottom-eyeRect.bottom;

		m_eyeRect.OffsetRect(Δ);
		m_ptDown.Offset(Δ);

		Invalidate();
	}
}

void CEagleWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDown==false||GetCapture()!=this||m_pView==nullptr)
	{
		ReleaseCapture();
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	//ccy	if(m_Image != nullptr && m_bDown == TRUE)
	//ccy	{
	//ccy		ReleaseCapture();	
	//ccy
	//ccy		CPoint pt = m_eyeRect.CenterPoint();
	//ccy		pt.x = (long)(pt.x/m_fHScale);
	//ccy		pt.y = (long)(pt.y/m_fVScale);
	//ccy		pt.x = m_docRect.left   + pt.x;
	//ccy		pt.y = m_docRect.bottom - pt.y;
	//ccy		if(m_pView != nullptr && m_pView->m_hWnd != nullptr)
	//ccy		{
	//ccy			CRect cliRect;
	//ccy			m_pView->GetClientRect(&cliRect);
	//ccy			m_pView->SendMessage(WM_CLITODOC,(UINT)(&cliRect),3);
	//ccy
	//ccy			long cx = pt.x - (cliRect.left+cliRect.Width()/2);
	//ccy			long cy = pt.y - (cliRect.top +cliRect.Height()/2);
	//ccy			
	//ccy			Matrix matrixDoctoCli;
	//ccy			m_pView->SendMessage(WM_SETMATRIX,(UINT)&matrixDoctoCli,0);
	//ccy			CPoint delta1(0,0);
	//ccy			CPoint delta2(cx,cy);
	//ccy			matrixDoctoCli.TransformPoints(&delta1);
	//ccy			matrixDoctoCli.TransformPoints(&delta2);
	//ccy			cx = delta2.x - delta1.x;
	//ccy			cy = delta2.y - delta1.y;
	//ccy
	//ccy			m_pView->OnScrollBy(CSize(cx,cy));
	//ccy			m_bDown = FALSE;
	//ccy		}
	//ccy		else
	//ccy		{
	//ccy			m_pView = nullptr;
	//ccy		}
	//ccy	}

	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CEagleWnd::SetRect(CView* pView, CRect cliRect)
{
	if(m_Image!=nullptr&&pView==m_pView)
	{
		cliRect.OffsetRect(-m_docRect.left, -m_docRect.top);

		m_eyeRect.left = cliRect.left;
		m_eyeRect.right = cliRect.right;
		m_eyeRect.top = m_docRect.Height()-cliRect.bottom;
		m_eyeRect.bottom = m_docRect.Height()-cliRect.top;

		m_eyeRect.left = (long)(m_eyeRect.left*m_fHScale);
		m_eyeRect.top = (long)(m_eyeRect.top*m_fVScale);
		m_eyeRect.right = (long)(m_eyeRect.right*m_fHScale);
		m_eyeRect.bottom = (long)(m_eyeRect.bottom*m_fVScale);

		Invalidate();
	}
}

void CEagleWnd::OpenEagleBmp(CView* pView, CString strBmp, CRect docRect)
{
	m_pView = pView;
	m_docRect = docRect;

	if(strBmp!=m_strBmp)
	{
		m_strBmp.Empty();

		delete m_Image;
		m_Image = nullptr;
		if(_taccess(strBmp, 00)!=-1)
		{
			const _bstr_t btrBmp(strBmp);
			m_Image = Gdiplus::Image::FromFile(btrBmp);
			this->ShowWindow(SW_SHOW);
		}
		else
		{
			this->ShowWindow(SW_HIDE);
		}

		m_strBmp = strBmp;
	}

	CSize bmpSize;
	if(m_Image!=nullptr)
	{
		bmpSize.cx = m_Image->GetWidth();
		bmpSize.cy = m_Image->GetHeight();

		m_fHScale = (double)bmpSize.cx/docRect.Width();
		m_fVScale = (double)bmpSize.cy/docRect.Height();
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

void CEagleWnd::Clear()
{
	delete m_Image;
	m_Image = nullptr;

	m_pView = nullptr;
}