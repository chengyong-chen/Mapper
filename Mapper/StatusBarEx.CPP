#include "stdafx.h"
#include "StatusBarEx.h"
#include "resource.h"
#include "../Dataview/Datainfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,
	IDS_VIEWLEVEL,
	IDS_VIEWSCALE,
	IDS_GEOMNAME,
	IDS_GEOCODE,
	IDS_GEOMID,
	IDS_POINTCOUNT,
	IDS_HANDLEXY,
	IDS_COORDINATE,
	IDS_UNITCOMBO,
};

BEGIN_MESSAGE_MAP(CStatusBarEx, CMFCStatusBar)
	//{{AFX_MSG_MAP(CStatusBarEx)	
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CStatusBarEx::CStatusBarEx()
{
}

CStatusBarEx::~CStatusBarEx()
{
	if(m_LevelBar.m_hWnd != nullptr)
	{
		m_LevelBar.PostMessage(WM_DESTROY, 0, 0);
		m_LevelBar.DestroyWindow();
	}
	if(m_ScaleBar.m_hWnd!=nullptr)
	{
		m_ScaleBar.PostMessage(WM_DESTROY, 0, 0);
		m_ScaleBar.DestroyWindow();
	}
}

BOOL CStatusBarEx::Create(CWnd* wnd)
{
	if(CMFCStatusBar::Create(wnd)==FALSE)
	{
		TRACE0("Failed to create status bar\n");
		return -1; // fail to create
	}

	if(CMFCStatusBar::SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))==FALSE)
	{
		TRACE0("Failed to create status bar\n");
		return -1; // fail to create
	}

	CMFCStatusBar::SetPaneWidth(0, 50);
	CMFCStatusBar::SetPaneWidth(indexVIEWLEVEL, 120);
	CMFCStatusBar::SetPaneWidth(indexVIEWSCALE, 120);
	CMFCStatusBar::SetPaneWidth(indexGEOMNAME, 200);
	CMFCStatusBar::SetPaneWidth(indexGEOCODE, 100);
	CMFCStatusBar::SetPaneWidth(indexGEOMID, 70);
	CMFCStatusBar::SetPaneWidth(indexPOINTCOUNT, 70);
	CMFCStatusBar::SetPaneWidth(indexHANDLEXY, 150);
	CMFCStatusBar::SetPaneWidth(indexCOORDINATE, 150);
	CMFCStatusBar::SetPaneWidth(indexUNITCOMBO, 70);

	CMFCStatusBar::SetPaneStyle(0, SBPS_STRETCH);

	CMFCStatusBar::SetTipText(indexGEOMNAME, _T(""));
	CMFCStatusBar::SetTipText(indexGEOCODE, _T(""));
	CMFCStatusBar::SetTipText(indexGEOMID, _T(""));
	CMFCStatusBar::SetTipText(indexPOINTCOUNT, _T(""));
	CMFCStatusBar::SetTipText(indexHANDLEXY, _T(""));
	CMFCStatusBar::SetTipText(indexCOORDINATE, _T(""));
	CMFCStatusBar::SetTipText(indexUNITCOMBO, _T(""));

	if(m_LevelBar.Create(IDD_LEVELBAR, this)==TRUE)
	{
		CRect rect;
		CMFCStatusBar::GetItemRect(indexVIEWLEVEL, &rect);
		m_LevelBar.MoveWindow(rect);
		m_LevelBar.ShowWindow(SW_SHOW);
	}
	if(m_ScaleBar.Create(IDD_SCALEBAR, this) == TRUE)
	{
		CRect rect;
		CMFCStatusBar::GetItemRect(indexVIEWSCALE, &rect);
		m_ScaleBar.MoveWindow(rect);
		m_ScaleBar.ShowWindow(SW_SHOW);
	}

	if(m_StatusCombo.m_hWnd==nullptr)
	{
		CRect rect;
		CMFCStatusBar::GetItemRect(indexUNITCOMBO, &rect);
		m_StatusCombo.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|CBS_DROPDOWNLIST, rect, this, indexUNITCOMBO);
		m_StatusCombo.MoveWindow(rect);
		m_StatusCombo.ShowWindow(SW_SHOW);
		m_StatusCombo.EnableWindow(FALSE);

		m_StatusCombo.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	}
	return TRUE;
}

void CStatusBarEx::ViewChanged(const CDatainfo& datainfo)
{
	m_StatusCombo.ResetContent();
	m_StatusCombo.EnableWindow(TRUE);
	if(datainfo.m_pProjection==nullptr)
	{
		m_StatusCombo.AddString(_T("Map"));
	}
	else
	{
		m_StatusCombo.AddString(_T("Map"));
		m_StatusCombo.AddString(_T("Geography"));
	}
	m_StatusCombo.SetCurSel(0);
}

void CStatusBarEx::SetViewBackground(int nProvider, int nVariety)
{
}

void CStatusBarEx::OnSize(UINT nType, int cx, int cy)
{
	CMFCStatusBar::OnSize(nType, cx, cy);

	if(m_LevelBar.m_hWnd != nullptr)
	{
		CRect rect;
		this->GetItemRect(indexVIEWLEVEL, &rect);
		m_LevelBar.SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}
	if(m_ScaleBar.m_hWnd!=nullptr)
	{
		CRect rect;
		this->GetItemRect(indexVIEWSCALE, &rect);
		m_ScaleBar.SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}

	if(m_StatusCombo.m_hWnd!=nullptr)
	{
		CRect rect;
		this->GetItemRect(indexUNITCOMBO, &rect);
		m_StatusCombo.SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}
}
