#include "stdafx.h"
#include "Global.h"
#include "FillAlone.h"
#include "FillAloneDlg.h"
#include "Color.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillAloneDlg dialog

CFillAloneDlg::CFillAloneDlg(CWnd* pParent /*=nullptr*/, CFillAlone* pfill)
	: CDialog(CFillAloneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFillAloneDlg)

	//}}AFX_DATA_INIT
	d_pFill = pfill;
}

void CFillAloneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillAloneDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillAloneDlg, CDialog)
	//{{AFX_MSG_MAP(CFillAloneDlg)
	ON_WM_PAINT()
	ON_BN_DOUBLECLICKED(IDC_COLOR, OnDCColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillAloneDlg message handlers

void CFillAloneDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_COLOR);
	if(pWnd!=nullptr)
	{
		CRect rect;
		pWnd->GetClientRect(rect);
		rect.InflateRect(-1, -1);

		CClientDC dc(pWnd);

		if(d_pFill!=nullptr)
		{
			CColor* color = d_pFill->m_pColor;
			if(color!=nullptr)
				dc.FillSolidRect(rect, color->GetMonitorRGB());
			else
				dc.FillSolidRect(rect, RGB(255, 255, 255));
		}
		else
		{
			dc.FillSolidRect(rect, RGB(255, 255, 255));
		}
	}
	GetParent()->SendMessage(WM_REDRAWPREVIEW);
}

BOOL CFillAloneDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFillAloneDlg::OnDCColor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CColorDlg dlg(this, d_pFill->m_pColor, RGB(255, 255, 255));
	if(dlg.DoModal()==IDOK)
	{
		delete d_pFill->m_pColor;
		d_pFill->m_pColor = dlg.d_pColor;
		dlg.d_pColor = nullptr;

		GetDlgItem(IDC_COLOR)->Invalidate();
	}
}

void CFillAloneDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(lpDrawItemStruct->itemID==LB_ERR)
		return;

	if(lpDrawItemStruct->CtlID==IDC_COLOR)
	{
		CColor* color = d_pFill->m_pColor;
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;
		rect.InflateRect(-1, -1);
		if(color!=nullptr)
			pDC->FillSolidRect(rect, color->GetMonitorRGB());
		else
			pDC->FillSolidRect(rect, RGB(255, 255, 255));
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
