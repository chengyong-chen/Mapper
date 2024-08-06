// SJBitmapButton.cpp : implementation file
//

#include "stdafx.h"
#include "GfxBitmapButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGfxBitmapButton

CGfxBitmapButton::CGfxBitmapButton()
{
	m_colTextColor = ::GetSysColor(COLOR_BTNTEXT);
	m_colBkColor   = ::GetSysColor(COLOR_BTNFACE);
	m_colFrameColor= ::GetSysColor(COLOR_BTNSHADOW);
	m_colDisableTextColor = ::GetSysColor(COLOR_GRAYTEXT);
	m_colFocusTextColor = m_colTextColor;

	m_bMouseTracking = FALSE;
	m_bFlat = FALSE;
}

CGfxBitmapButton::~CGfxBitmapButton()
{
}


BEGIN_MESSAGE_MAP(CGfxBitmapButton, CButton)
	//{{AFX_MSG_MAP(CGfxBitmapButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGfxBitmapButton message handlers
void CGfxBitmapButton::SetBkColor(COLORREF colBkColor)
{
	m_colBkColor = colBkColor;
}

void CGfxBitmapButton::SetTextColor(COLORREF colTextColor)
{
	m_colTextColor = colTextColor;
	m_colFocusTextColor = m_colTextColor;
	m_colDisableTextColor = m_colTextColor;
}

void CGfxBitmapButton::SetFrameColor(COLORREF colFrameColor)
{
	m_colFrameColor = colFrameColor;
}

void CGfxBitmapButton::SetTextColor(COLORREF colTextColor, COLORREF colFocusTextColor, 
	COLORREF colDisableTextColor)
{
	m_colTextColor = colTextColor;
	m_colFocusTextColor = colFocusTextColor;
	m_colDisableTextColor = colDisableTextColor;
}

BOOL CGfxBitmapButton::SetImageList(CImageList* imagelist, int nType)
{
	if(!nType)
	{
		m_cImageBk.DeleteImageList();
		return m_cImageBk.Create(imagelist);
	}
	else
	{
		m_cImageMark.DeleteImageList();
		return m_cImageMark.Create(imagelist);
	}
}

// Draw the appropriate bitmap
void CGfxBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != nullptr);
	CRect rcItem = lpDIS->rcItem;
	UINT  state  = lpDIS->itemState;
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	if(dc.CreateCompatibleDC(pDC))
	{
		if(bitmap.CreateCompatibleBitmap(pDC, rcItem.Width(), rcItem.Height()))
		{
			pDrawDC = &dc;
			pOldBitmap = dc.SelectObject(&bitmap);
		}
	}

	int nSaveDC = pDrawDC->SaveDC();
	pDrawDC->FillSolidRect(&rcItem, m_colBkColor);

	DWORD style = GetStyle();
	int nImageCount = 0;
	if(m_cImageBk.m_hImageList)
		nImageCount = m_cImageBk.GetImageCount();

	// ������λͼ
	int nBitmapId = 0;
	if(nImageCount > 1)
	{
		if((state & ODS_DISABLED) && m_cImageBk.GetImageCount() > 1)
			nBitmapId = 1;
		else if(((state & ODS_FOCUS) || m_bMouseTracking) && m_cImageBk.GetImageCount() > 2)
			nBitmapId = 2;
		else if((state & ODS_SELECTED) && m_cImageBk.GetImageCount() > 3)
			nBitmapId = 3;

		m_cImageBk.Draw(pDrawDC, nBitmapId, CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
	}
	else if(nImageCount == 1)
		m_cImageBk.Draw(pDrawDC, nBitmapId, CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);

	CString rText;
	GetWindowText(rText);
	
	// �����
	CPoint point;
	pDrawDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	CSize size = pDrawDC->GetTextExtent(_T(" "), 2);
	size.cx = size.cx/2;
	size.cy = size.cy/2;
	point.x = rcItem.left;
	point.y = rcItem.right;

	CSize sizeImage(0, 0);
	if(m_cImageMark.m_hImageList)
	{
		IMAGEINFO imageinfo;
		m_cImageMark.GetImageInfo(0, &imageinfo);
		sizeImage.cx = imageinfo.rcImage.right;
		sizeImage.cy = imageinfo.rcImage.bottom;
		sizeImage.cx += size.cx;
		sizeImage.cy += size.cy;
	}

	UINT dt;
	if(style & BS_MULTILINE)
		dt = 0;
	else
		dt = DT_SINGLELINE;
	if((style & BS_CENTER) == BS_CENTER)
	{
		dt |= DT_CENTER;
		point.x = rcItem.CenterPoint().x - (sizeImage.cx - size.cx)/2;
	}
	else if((style & BS_RIGHT) == BS_RIGHT)
	{
		dt |= DT_RIGHT;
		point.x = rcItem.right - size.cx;
		rcItem.right = point.x - sizeImage.cx;
	}
	else
	{
		dt |= DT_LEFT;
		point.x = rcItem.left + size.cx;
		rcItem.left = point.x + sizeImage.cx;
	}
	if((style & BS_VCENTER) == BS_VCENTER)
	{
		dt |= DT_VCENTER;
		point.y = rcItem.CenterPoint().y - (sizeImage.cy - size.cy)/2;
	}
	else if((style & BS_BOTTOM) == BS_BOTTOM)
	{
		dt |= DT_BOTTOM;
		point.y = rcItem.bottom - size.cy;
		rcItem.bottom = point.y - sizeImage.cy;
	}
	else
	{
		dt |= DT_TOP;
		point.y = rcItem.top + size.cy;
		rcItem.top = point.y + sizeImage.cy;
	}

	if(m_cImageMark.m_hImageList)
		m_cImageMark.Draw(pDrawDC, 0, point, ILD_TRANSPARENT);

	// ����Χ�ı߿�
	CPen pen(PS_SOLID, 0, m_colFrameColor);
	if(m_bMouseTracking && !(state & ODS_DISABLED))
		pDrawDC->Draw3dRect(&(lpDIS->rcItem), GetSysColor(COLOR_BTNHILIGHT), m_colFrameColor);
	else
	{
		pDrawDC->SelectObject(&pen);
		pDrawDC->SelectObject(GetStockObject(NULL_BRUSH));
		pDrawDC->Rectangle(&(lpDIS->rcItem));
	}

	// д����
	if(rText.GetLength())
	{
		if(state & ODS_DISABLED)
			pDrawDC->SetTextColor(m_colDisableTextColor);
		else if(state & ODS_FOCUS || state & ODS_SELECTED ||
			m_bMouseTracking)
			pDrawDC->SetTextColor(m_colFocusTextColor);
		else
			pDrawDC->SetTextColor(m_colTextColor);

		pDrawDC->SetBkMode(TRANSPARENT);
		pDrawDC->DrawText(rText, &rcItem, dt);
	}
	pDrawDC->RestoreDC(nSaveDC);

	rcItem = lpDIS->rcItem;
	if(pDrawDC != pDC)
	{
		pDC->BitBlt(0, 0, rcItem.Width(), rcItem.Height(), &dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
	}
}

void CGfxBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);
}

void CGfxBitmapButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_bFlat) return;

	CRect rect;
	GetClientRect(&rect);

	if(rect.PtInRect(point))
	{
		if(GetCapture() == this) return;
		SetCapture();
		Invalidate();
		m_bMouseTracking = TRUE;
	}
	else
	{
		ReleaseCapture();
		
		m_bMouseTracking = FALSE;
		Invalidate();
	}

	CButton::OnMouseMove(nFlags, point);
}

BOOL CGfxBitmapButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
