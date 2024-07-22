// Label.cpp : implementation file
//

#include "stdafx.h"
#include "Label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabel

CLabel::CLabel()
{
	m_crText = GetSysColor(COLOR_WINDOWTEXT);
	m_crTextLink = GetSysColor(COLOR_WINDOWTEXT);
	m_hBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	m_hBrushLink = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lf), &m_lf);
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lfLink), &m_lfLink);

	m_font.CreateFontIndirect(&m_lf);
	m_fontLink.CreateFontIndirect(&m_lfLink);
	m_bTimer = FALSE;
	m_bState = FALSE;
	m_bLink = TRUE;
	m_hCursor = nullptr;
	m_Type = None;

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	m_bMouseOver = FALSE;
}

CLabel::~CLabel()
{
	if(m_font.GetSafeHandle()!=nullptr)
		m_font.DeleteObject();
	if(m_fontLink.GetSafeHandle()!=nullptr)
		m_fontLink.DeleteObject();

	::DeleteObject(m_hBrush);
	::DeleteObject(m_hwndBrush);
}

CLabel& CLabel::SetText(const CString& strText)
{
	SetWindowText(strText);
	return *this;
}

CLabel& CLabel::SetTextColor(COLORREF crText, BOOL bIsLink)
{
	if(!bIsLink)
		m_crText = crText;
	else
		m_crTextLink = crText;
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontBold(BOOL bBold, BOOL bIsLink)
{
	if(!bIsLink)
		m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	else
		m_lfLink.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont(bIsLink);
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontUnderline(BOOL bSet, BOOL bIsLink)
{
	if(!bIsLink)
		m_lf.lfUnderline = bSet;
	else
		m_lfLink.lfUnderline = bSet;
	ReconstructFont(bIsLink);
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontItalic(BOOL bSet, BOOL bIsLink)
{
	if(!bIsLink)
		m_lf.lfItalic = bSet;
	else
		m_lfLink.lfItalic = bSet;
	ReconstructFont(bIsLink);
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontSize(int nSize, BOOL bIsLink)
{
	nSize *= -1;
	if(!bIsLink)
		m_lf.lfHeight = nSize;
	else
		m_lfLink.lfHeight = nSize;
	ReconstructFont(bIsLink);
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetFontName(const CString& strFont, BOOL bIsLink)
{
	if(!bIsLink)
		_tcscpy(m_lf.lfFaceName, strFont);
	else
		_tcscpy(m_lfLink.lfFaceName, strFont);
	ReconstructFont(bIsLink);
	RedrawWindow();
	return *this;
}

CLabel& CLabel::SetSunken(BOOL bSet)
{
	if(!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
	else
		ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);

	return *this;
}

CLabel& CLabel::SetBorder(BOOL bSet)
{
	if(!bSet)
		ModifyStyle(WS_BORDER, 0, SWP_DRAWFRAME);
	else
		ModifyStyle(0, WS_BORDER, SWP_DRAWFRAME);

	return *this;
}

CLabel& CLabel::SetBkColor(COLORREF crBkgnd, BOOL bIsLink)
{
	if(!bIsLink)
	{
		::DeleteObject(m_hBrush);
		m_hBrush = ::CreateSolidBrush(crBkgnd);
	}
	else
	{
		::DeleteObject(m_hBrushLink);
		m_hBrushLink = ::CreateSolidBrush(crBkgnd);
	}

	return *this;
}

BEGIN_MESSAGE_MAP(CLabel, CStatic)
	//{{AFX_MSG_MAP(CLabel)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabel message handlers

HBRUSH CLabel::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if(CTLCOLOR_STATIC==nCtlColor)
	{
		if(!m_bMouseOver)
		{
			pDC->SelectObject(&m_font);
			pDC->SetTextColor(m_crText);
		}
		else
		{
			pDC->SelectObject(&m_fontLink);
			pDC->SetTextColor(m_crTextLink);
		}
		pDC->SetBkMode(TRANSPARENT);
	}

	if(m_Type==Background)
	{
		if(!m_bState)
			return m_hwndBrush;
	}

	if(!m_bMouseOver)
		return m_hBrush;
	else
		return m_hBrushLink;
}

void CLabel::ReconstructFont(BOOL bIsLink)
{
	if(!bIsLink)
	{
		m_font.DeleteObject();
		VERIFY(m_font.CreateFontIndirect(&m_lf));
	}
	else
	{
		m_fontLink.DeleteObject();
		VERIFY(m_fontLink.CreateFontIndirect(&m_lfLink));
	}
}

CLabel& CLabel::FlashText(BOOL bActivate)
{
	if(m_bTimer)
	{
		SetWindowText(m_strText);
		this->KillTimer(1);
	}

	if(bActivate)
	{
		GetWindowText(m_strText);
		m_bState = FALSE;

		m_bTimer = TRUE;
		this->SetTimer(1, 500, nullptr);
		m_Type = Text;
	}

	return *this;
}

CLabel& CLabel::FlashBackground(BOOL bActivate)
{
	if(m_bTimer)
		this->KillTimer(1);

	if(bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;
		this->SetTimer(1, 500, nullptr);

		m_Type = Background;
	}

	return *this;
}

void CLabel::OnTimer(UINT nIDEvent)
{
	m_bState = !m_bState;

	switch(m_Type)
	{
	case Text:
		if(m_bState)
			SetWindowText(_T(""));
		else
			SetWindowText(m_strText);
		break;

	case Background:
		InvalidateRect(nullptr, FALSE);
		UpdateWindow();
		break;
	}

	CStatic::OnTimer(nIDEvent);
}

CLabel& CLabel::SetLink(BOOL bLink, LPCTSTR strLinkTarget)
{
	m_bLink = bLink;

	if(bLink)
	{
		ModifyStyle(0, SS_NOTIFY);
		m_strLinkTarget = strLinkTarget;
	}
	else
		ModifyStyle(SS_NOTIFY, 0);

	return *this;
}

void CLabel::OnLButtonDown(UINT nFlags, CPoint point)
{
	CString strTarget, strMailTo = _T("mailto:");
	HWND hWndDesktop;
	int iLength;

	m_bMouseOver = FALSE;
	::ReleaseCapture();
	RedrawWindow();

	hWndDesktop = ::GetDesktopWindow();
	strTarget = m_strLinkTarget;
	if(strTarget==_T(""))
	{
		GetWindowText(strTarget);
		iLength = strMailTo.GetLength();
		if(strTarget.Find(_T('@'))!=-1&&strTarget.Left(iLength)!=strMailTo)
			strTarget = strMailTo+strTarget;
	}
	::ShellExecute(hWndDesktop, _T("open"), strTarget, nullptr, _T(""), SW_SHOW);
}

BOOL CLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

CLabel& CLabel::SetLinkCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
	return *this;
}

void CLabel::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bPreviousState;

	CRect rect;
	GetClientRect(&rect);
	bPreviousState = m_bMouseOver;
	if(rect.PtInRect(point))
	{
		m_bMouseOver = TRUE;
		::SetCursor(m_hCursor);
		SetCapture();
	}
	else
	{
		m_bMouseOver = FALSE;
		::ReleaseCapture();
	}

	if(m_bMouseOver!=bPreviousState)
		RedrawWindow();

	CStatic::OnMouseMove(nFlags, point);
}
