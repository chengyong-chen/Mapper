#include "stdafx.h"
#include "LegendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg dialog

CLegendDlg::CLegendDlg(CWnd* pParent, CString strHtml)
	: CDialog(CLegendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLegendDlg)

	//}}AFX_DATA_INIT
	m_strHtml = strHtml;
}

void CLegendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLegendDlg)
	DDX_Control(pDX, IDC_EXPLORER, m_browser);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLegendDlg, CDialog)
	//{{AFX_MSG_MAP(CLegendDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg message handlers

BOOL CLegendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/*	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),m_strImage,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		CBitmap bmp;
		BITMAP  bi;
		bmp.Attach(hBmp);
		bmp.GetBitmap(&bi);

		SetWindowPos(&wndTop, 0, 0, bi.bmWidth,bi.bmHeight, SWP_SHOWWINDOW);
		CRect rect1;
		GetClientRect(rect1);
		CWnd* pWnd = GetDlgItem(IDOK);
		if(pWnd != nullptr)
		{
			CRect rect2;
			pWnd->GetClientRect(rect2);
			pWnd->SetWindowPos(&wndTop, rect1.right-rect2.Width(),rect1.bottom-rect2.Height(),0,0, SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		bmp.DeleteObject();
	*/
	CenterWindow();
	m_browser.Navigate(m_strHtml, nullptr, nullptr, nullptr, nullptr);
	return TRUE;
}

/*
void CLegendDlg::OnPaint()
{
	CPaintDC dc(this);

	if(_taccess(m_strImage,0) != -1)
	{
		CRect rect1;
		CRect rect2;
		GetClientRect(rect1);
		CWnd* pWnd = GetDlgItem(IDOK);
		if(pWnd != nullptr)
		{
			pWnd->GetClientRect(rect2);
			rect2.OffsetRect(rect1.right-rect2.Width(),rect1.bottom-rect2.Height());
		}
		else
			rect2.SetRectEmpty();

		CRgn rgn1;
		CRgn rgn2;
		rgn1.CreateRectRgnIndirect(rect1);
		rgn2.CreateRectRgnIndirect(rect2);
		rgn1.CombineRgn(&rgn1,&rgn2,RGN_XOR);
		dc.SelectClipRgn(&rgn1);
		rgn1.DeleteObject();
		rgn2.DeleteObject();

		dc.FillSolidRect(rect1, GetSysColor(COLOR_3DFACE));

		HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),m_strImage,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		CBitmap bmp;
		bmp.Attach(hBmp);

		CDC bmDC;
		bmDC.CreateCompatibleDC(&dc);

		CBitmap *pOldbmp = bmDC.SelectObject(&bmp);
		BITMAP  bi;
		bmp.GetBitmap(&bi);
		dc.BitBlt(0,0,bi.bmWidth,bi.bmHeight,&bmDC,0,0,SRCCOPY);

		bmDC.SelectObject(pOldbmp);
		bmDC.DeleteDC();

		bmp.DeleteObject();
	}
}
*/
void CLegendDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_browser.m_hWnd!=nullptr)
	{
		m_browser.SetWindowPos(&wndTop, 0, 0, cx, cy-30, SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	CWnd* pWnd = GetDlgItem(IDOK);
	if(pWnd!=nullptr)
	{
		pWnd->SetWindowPos(&wndTop, cx-75, cy-27, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}
