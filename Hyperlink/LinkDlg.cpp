#include "stdafx.h"
#include "Link.h"

#include "LinkDlg.h"

#include "LinkMapDlg.h"
#include "LinkHttpDlg.h"
#include "LinkHtmlDlg.h"
#include "LinkEMailDlg.h"
#include "LinkFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkDlg::CLinkDlg(CWnd* pParent /*=nullptr*/, CLink* plink)
	: CDialog(CLinkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkDlg)

	d_pLinkdlg = nullptr;
	//}}AFX_DATA_INIT
	if(plink!=nullptr)
	{
		d_pLink = plink->Clone();
	}
}

CLinkDlg::~CLinkDlg()
{
	delete d_pLink;
}

void CLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkDlg)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkDlg message handlers

BOOL CLinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nIndex = 0;
	const CString strProject = AfxGetApp()->m_lpCmdLine;
	if(strProject.IsEmpty()==TRUE)
	{
		nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Map"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetItemData(nIndex, 1);
	}

	nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Http"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetItemData(nIndex, 2);
	nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Html"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetItemData(nIndex, 3);
	nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("e-Mail"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetItemData(nIndex, 4);
	nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("File"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetItemData(nIndex, 5);

	CLink* link = d_pLink;
	d_pLink = nullptr;

	Reset(link);
	const BYTE type = link->Gettype();
	switch(type)
	{
	case 1:
	{
		const int nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, _T("Map"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
	}
	break;
	case 2:
	{
		const int nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, _T("Http"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
	}
	break;
	case 3:
	{
		const int nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, _T("Html"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
	}
	break;
	case 4:
	{
		const int nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, _T("e-Mail"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
	}
	break;
	case 5:
	{
		const int nIndex = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, _T("File"));
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
	}
	break;
	}

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLinkDlg::OnTypeSelchange()
{
	const int index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	const DWORD data = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(index);
	CLink* link = CLink::Apply(data);

	Reset(link);
}

void CLinkDlg::Reset(CLink* plink)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Hyperlink.dll"));
	AfxSetResourceHandle(hInst);

	if(d_pLink!=nullptr)
	{
		plink->m_dwGeomId = d_pLink->m_dwGeomId;
		delete d_pLink;
		d_pLink = nullptr;
	}

	if(d_pLinkdlg!=nullptr)
	{
		d_pLinkdlg->ShowWindow(SW_HIDE);
		d_pLinkdlg->PostMessage(WM_DESTROY, 0, 0);
		d_pLinkdlg->DestroyWindow();
		delete d_pLinkdlg;
		d_pLinkdlg = nullptr;
	}

	if(plink==nullptr)
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return;
	}

	d_pLink = plink;
	const BYTE type = d_pLink->Gettype();
	switch(type)
	{
	case 1:
	{
		d_pLinkdlg = new CLinkMapDlg((CWnd*)this, (CLinkMap*)d_pLink);
		d_pLinkdlg->Create(IDD_LINKMAP, (CWnd*)this);
		d_pLinkdlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 2:
	{
		d_pLinkdlg = new CLinkHttpDlg((CWnd*)this, (CLinkHttp*)d_pLink);
		d_pLinkdlg->Create(IDD_LINKHTTP, (CWnd*)this);
		d_pLinkdlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 3:
	{
		d_pLinkdlg = new CLinkHtmlDlg((CWnd*)this, (CLinkHtml*)d_pLink);
		d_pLinkdlg->Create(IDD_LINKHTML, (CWnd*)this);
		d_pLinkdlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 4:
	{
		d_pLinkdlg = new CLinkEMailDlg((CWnd*)this, (CLinkEMail*)d_pLink);
		d_pLinkdlg->Create(IDD_LINKEMAIL, (CWnd*)this);
		d_pLinkdlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 5:
	{
		d_pLinkdlg = new CLinkFileDlg((CWnd*)this, (CLinkFile*)d_pLink);
		d_pLinkdlg->Create(IDD_LINKFILE, (CWnd*)this);
		d_pLinkdlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLinkDlg::OnOK()
{
	if(d_pLinkdlg!=nullptr&&d_pLinkdlg->m_hWnd!=nullptr)
	{
		d_pLinkdlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	CDialog::OnOK();
}

void CLinkDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(d_pLinkdlg!=nullptr)
	{
		d_pLinkdlg->PostMessage(WM_DESTROY, 0, 0);
		d_pLinkdlg->DestroyWindow();
		delete d_pLinkdlg;
		d_pLinkdlg = nullptr;
	}
}
