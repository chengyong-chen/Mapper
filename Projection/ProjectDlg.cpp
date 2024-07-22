#include "stdafx.h"

#include "ProjectDlg.h"
#include "Projection1.h"
#include "Geocentric.h"
#include "Geographic.h"

#include "ParameterCtrl.h"
#include "ParameterDlg.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProjectDlg::CProjectDlg(CWnd* pParent, CProjection* pProjection, bool action)
	: CDialog(CProjectDlg::IDD, pParent), m_bNeedAction(action)
{
	//{{AFX_DATA_INIT(CProjectDlg)
	d_pProjection = nullptr;
	d_pParameterCtrl = nullptr;
	//}}AFX_DATA_INIT

	if(pProjection != nullptr)
	{
		d_pProjection = pProjection->Clone();
	}
}

CProjectDlg::~CProjectDlg()
{
	delete d_pProjection;
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectDlg)
	DDX_Check(pDX, IDC_INTERPOLATE, m_bInterpolate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectDlg)
	ON_CBN_SELCHANGE(IDC_TYPE, OnTypeSelchange)
	ON_CBN_SELCHANGE(IDC_GCS, OnGCSSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg message handlers

BOOL CProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_TYPE))->AddString(_T("None"));
	((CComboBox*)GetDlgItem(IDC_TYPE))->SetItemData(0, -1);
	((CComboBox*)GetDlgItem(IDC_TYPE))->AddString(_T("Geographic"));
	((CComboBox*)GetDlgItem(IDC_TYPE))->SetItemData(1, 0);

	for(int index = 0; CGeocentric::ProjTypes[index].key != nullptr; index++)
	{
		const int item = ((CComboBox*)GetDlgItem(IDC_TYPE))->AddString(CString(CGeocentric::ProjTypes[index].name));
		((CComboBox*)GetDlgItem(IDC_TYPE))->SetItemData(item, (DWORD)&CGeocentric::ProjTypes[index]);
	}
	for(int index = 0; CProjection::SpheroidList[index].key != nullptr; index++)
	{
		const int item = ((CComboBox*)GetDlgItem(IDC_GCS))->AddString(CString(CProjection::SpheroidList[index].name));
		((CComboBox*)GetDlgItem(IDC_GCS))->SetItemData(item, (DWORD)&CProjection::SpheroidList[index]);
	}
	if(m_bNeedAction == false)
	{
		GetDlgItem(IDC_INTERPOLATE)->ShowWindow(SW_HIDE);
	}
	CenterWindow();

	CProjection* pProjection = d_pProjection;
	d_pProjection = nullptr;
	Reset(pProjection);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProjectDlg::OnTypeSelchange()
{
	CProjection* pProjection = nullptr;
	const int index = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();
	const DWORD data = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetItemData(index);
	switch(data)
	{
		case 0XFFFFFFFF:
			pProjection = nullptr;
			break;
		case 0:
			pProjection = new CGeographic();
			break;
		default:
			const CGeocentric::ProjType* pProjType = (const CGeocentric::ProjType*)data;
			pProjection = new CGeocentric(pProjType->key);
			break;
	}
	if(d_pProjection != nullptr && pProjection != nullptr)
	{
		d_pProjection->CopyTo(pProjection);
	}
	Reset(pProjection);
}

void CProjectDlg::OnGCSSelchange()
{
	const long index = ((CComboBox*)GetDlgItem(IDC_GCS))->GetCurSel();
	if(d_pProjection != nullptr)
	{
		const Spheroid* pSpheroid = (const Spheroid*)((CComboBox*)GetDlgItem(IDC_GCS))->GetItemData(index);
		d_pProjection->SetupGCS(pSpheroid->key);
	}
}

void CProjectDlg::OnOK()
{
	if(d_pParameterCtrl != nullptr && d_pParameterCtrl->m_hWnd != nullptr)
	{
		d_pParameterCtrl->SendMessage(WM_COMMAND, IDOK, 0);
	}
	if(d_pProjection != nullptr)
	{
		d_pProjection->Reconfig();
	}

	CDialog::OnOK();
}

void CProjectDlg::OnCancel()
{
	delete d_pProjection;
	d_pProjection = nullptr;

	CDialog::OnCancel();
}

void CProjectDlg::Reset(CProjection* pProjection)
{
	HINSTANCE hInst = ::LoadLibrary(_T("Projection.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	delete d_pProjection;
	d_pProjection = nullptr;

	if(d_pParameterCtrl != nullptr)
	{
		d_pParameterCtrl->PostMessage(WM_DESTROY, 0, 0);
		d_pParameterCtrl->DestroyWindow();
		delete d_pParameterCtrl;
		d_pParameterCtrl = nullptr;
	}

	if(pProjection == nullptr)
	{
		GetDlgItem(IDC_GCS)->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(0);
	}
	else
	{
		GetDlgItem(IDC_GCS)->EnableWindow(TRUE);
		const CString strName = pProjection->GetName();
		((CComboBox*)GetDlgItem(IDC_TYPE))->SelectString(-1, strName);
		((CComboBox*)GetDlgItem(IDC_GCS))->SetCurSel(CGcs::GetGCSIndex(pProjection->GCS.m_Key));
	}
	if(pProjection == nullptr)
	{
	}
	else if(pProjection->IsKindOf(RUNTIME_CLASS(CGeocentric)) == TRUE)
	{
		d_pProjection = pProjection;
		d_pParameterCtrl = new CParameterCtrl((CWnd*)this, (CGeocentric*&)d_pProjection);
		d_pParameterCtrl->Create(IDD_PARAMETER, (CWnd*)this);
		d_pParameterCtrl->ShowWindow(SW_SHOWNORMAL);
	}

	CRect rect1;
	this->GetWindowRect(rect1);
	if(d_pParameterCtrl != nullptr)
	{
		d_pParameterCtrl->SetWindowPos(&wndTop, 33, 90, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		CRect rect2;
		d_pParameterCtrl->GetWindowRect(rect2);
		this->SetWindowPos(&wndTop, 0, 0, rect1.Width(), 100 + rect2.Height() - 10, SWP_NOMOVE | SWP_SHOWWINDOW);
	}
	else
	{
		this->SetWindowPos(&wndTop, 0, 0, rect1.Width(), 160, SWP_NOMOVE | SWP_SHOWWINDOW);
	}
	d_pProjection = pProjection;
	//	this->SetWindowPos(&wndTop, 0, 0. 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CProjectDlg::OnDestroy()
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
