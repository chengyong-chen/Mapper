#include "stdafx.h"
#include "MyPrintDlg.h"
#include "PageDlg.h"
#include "Datainfo.h"
#include "ViewPrinter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPrintDlg

IMPLEMENT_DYNAMIC(CMyPrintDlg, CPrintDialog)

CMyPrintDlg::CMyPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd)
	: CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
	//{{AFX_DATA_INIT(CMyPrintDlg)
	m_nOverlap = 0;
	m_viewPrinter = nullptr;
	//}}AFX_DATA_INIT

	//HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	//HINSTANCE hInstOld = AfxGetResourceHandle();
	//AfxSetResourceHandle(hInst);

	m_pd.lpPrintTemplateName = (LPTSTR)MAKEINTRESOURCE(IDD_PRINTDLG);
	m_pd.Flags |= PD_ENABLEPRINTTEMPLATE;
	m_pd.hInstance = AfxGetInstanceHandle();

	//AfxSetResourceHandle(hInstOld);
	//::FreeLibrary(hInst);
}

void CMyPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CPrintDialog::DoDataExchange(pDX);
	///{{AFX_DATA_MAP(CMyPrintDlg)
	DDX_Text(pDX, IDC_OVERLAP, m_nOverlap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyPrintDlg, CPrintDialog)
	//{{AFX_MSG_MAP(CMyPrintDlg)
	ON_BN_CLICKED(IDC_PAGE, OnPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyPrintDlg::OnPage()
{
	HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CPageDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_viewPrinter->m_bVCenter = dlg.m_bVCenter;
		m_viewPrinter->m_bHCenter = dlg.m_bHCenter;
		m_viewPrinter->m_marginTop = dlg.m_topMargin;
		m_viewPrinter->m_marginBottom = dlg.m_bottomMargin;
		m_viewPrinter->m_marginLeft = dlg.m_leftMargin;
		m_viewPrinter->m_marginRight = dlg.m_rightMargin;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BOOL CMyPrintDlg::OnInitDialog()
{
	CPrintDialog::OnInitDialog();

	if(m_viewPrinter!=nullptr)
	{
		if(m_viewPrinter->m_docPrint.IsRectEmpty()==FALSE)
		{
			GetDlgItem(rad1)->EnableWindow(FALSE);
		}

		CString str;
		str.Format(_T("%.2f"), m_viewPrinter->m_datainfo.m_scaleSource);
		str.TrimRight(_T("0"));
		str.TrimRight(_T("."));
		((CWnd*)GetDlgItem(IDC_SCALE))->SetWindowText(str);
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyPrintDlg::OnOK()
{
	CPrintDialog::OnOK();

	if(m_viewPrinter!=nullptr)
	{
		CString str;
		((CWnd*)GetDlgItem(IDC_SCALE))->GetWindowText(str);
		m_viewPrinter->m_scalePrint = _ttof(str);
		const BOOL bAll = this->PrintAll();
		if(bAll==TRUE)
		{
			m_viewPrinter->m_docPrint = m_viewPrinter->m_datainfo.GetDocRect();
		}
	}
}
