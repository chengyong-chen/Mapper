#include "stdafx.h"

#include "Geocentric.h"

#include "ParameterDlg.h"
#include "ParameterCtrl.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CParameterDlg::CParameterDlg(CWnd* pParent, CGeocentric* pGeocentric)
	: CDialog(CParameterDlg::IDD, pParent), m_pGeocentric(pGeocentric)
{
	//{{AFX_DATA_INIT(CParameterDlg)
	d_pParameterCtrl = nullptr;
	//}}AFX_DATA_INIT
}

CParameterDlg::~CParameterDlg()
{
}

void CParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterDlg)	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParameterDlg, CDialog)
	//{{AFX_MSG_MAP(CParameterDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterDlg message handlers

BOOL CParameterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();

	HINSTANCE hInst = ::LoadLibrary(_T("Projection.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	d_pParameterCtrl = new CParameterCtrl((CWnd*)this, m_pGeocentric);
	d_pParameterCtrl->Create(IDD_PARAMETER, (CWnd*)this);
	d_pParameterCtrl->ShowWindow(SW_SHOWNORMAL);

	CRect rect1;
	this->GetWindowRect(rect1);
	d_pParameterCtrl->SetWindowPos(&wndTop, 3, 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	CRect rect2;
	d_pParameterCtrl->GetWindowRect(rect2);
	this->SetWindowPos(&wndTop, 0, 0, rect1.Width(), rect2.Height()+190, SWP_NOMOVE | SWP_SHOWWINDOW);

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	return TRUE; 
}

void CParameterDlg::OnOK()
{
	if(d_pParameterCtrl != nullptr && d_pParameterCtrl->m_hWnd != nullptr)
	{
		d_pParameterCtrl->SendMessage(WM_COMMAND, IDOK, 0);
	}
	if(m_pGeocentric != nullptr)
	{
		m_pGeocentric->Reconfig();
	}

	CDialog::OnOK();
}

void CParameterDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(d_pParameterCtrl != nullptr)
	{
		d_pParameterCtrl->PostMessage(WM_DESTROY, 0, 0);
		d_pParameterCtrl->DestroyWindow();
		delete d_pParameterCtrl;
		d_pParameterCtrl = nullptr;
	}
}
