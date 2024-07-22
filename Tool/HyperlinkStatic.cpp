#include "stdafx.h"
#include "HyperlinkStatic.h"
#include "../Viewer/Global.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic

CHyperlinkStatic::CHyperlinkStatic()
{
	m_bMouseIn = false;
}

CHyperlinkStatic::~CHyperlinkStatic()
{
}

BEGIN_MESSAGE_MAP(CHyperlinkStatic, CStatic)
	//{{AFX_MSG_MAP(CHyperlinkStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic message handlers

void CHyperlinkStatic::OnLButtonDown(UINT nFlags, CPoint docPoint)
{
	CStatic::OnLButtonDown(nFlags, docPoint);

	if(m_strURL.IsEmpty()==FALSE)
	{
		const CString strRegistryKey = AfxGetApp()->m_pszRegistryKey;
		if(strRegistryKey.IsEmpty()==FALSE)
		{
			const CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, strRegistryKey, _T("Html"), _T("Path"), _T(""));
			CString strHtml = strPath+m_strURL;
			AfxGetMainWnd()->SendMessage(WM_BROWSEHTML, (UINT)&strHtml, 1);
		}
	}
}

void CHyperlinkStatic::OnPaint()
{
	CPaintDC dc(this);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	LOGFONT LogFont;
	memcpy(&LogFont, &(ncm.lfStatusFont), sizeof(LOGFONT));
	LogFont.lfUnderline = TRUE;

	CFont font;
	font.CreateFontIndirect(&LogFont);
	CFont* OldFont = (CFont*)dc.SelectObject((CFont*)&font);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 255));
	dc.TextOut(0, 0, _T("View the link"));

	dc.SelectObject(OldFont);
	font.DeleteObject();
}

void CHyperlinkStatic::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY, TRUE);

	CStatic::PreSubclassWindow();
}

LRESULT CHyperlinkStatic::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);;
	}

	m_bMouseIn = false;
	return 0;
}

void CHyperlinkStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_bMouseIn==false)
	{
		//Track the mouse leave event
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = GetSafeHwnd();
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bMouseIn = true;
	}
	else
	{
		const HCURSOR hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND));
		if(hCursor!=nullptr)
		{
			::SetCursor(hCursor);;
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}
