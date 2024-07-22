// SpeedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpeedPage.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedPage dialog

CSpeedPage::CSpeedPage() : CPropertyPage(CSpeedPage::IDD)
{
	//{{AFX_DATA_INIT(CSpeedPage)

	//}}AFX_DATA_INIT
}

CSpeedPage::~CSpeedPage()
{
}

void CSpeedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here	
	DDX_Control(pDX, IDC_SLIDER, m_SlideCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpeedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSpeedPage)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER, OnReleasedSlider)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedPage message handlers

BOOL CSpeedPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_SlideCtrl.SetLineSize(100);
	m_SlideCtrl.SetPageSize(100);
	m_SlideCtrl.SetRange(100, 2000);
	const long nReplaySpeed = AfxGetApp()->GetProfileInt(_T("Socket"), _T("ReplaySpeed"), 200);
	const long pos = 2000-(nReplaySpeed-100);
	m_SlideCtrl.SetPos(pos);

	TCHAR str[6];
	_itot(nReplaySpeed, str, 10);
	GetDlgItem(IDC_EDIT)->SetWindowText(str);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeedPage::OnOK()
{
	CPropertyPage::OnOK();

	m_nSlidePos = m_SlideCtrl.GetPos();
	const long nReplaySpeed = 2000-(m_nSlidePos-100);
	AfxGetApp()->WriteProfileInt(_T("Socket"), _T("ReplaySpeed"), nReplaySpeed);
}

void CSpeedPage::OnReleasedSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	long nReplaySpeed = m_SlideCtrl.GetPos();
	nReplaySpeed = 2000-(nReplaySpeed-100);

	TCHAR str[6];
	_itot(nReplaySpeed, str, 10);
	GetDlgItem(IDC_EDIT)->SetWindowText(str);

	*pResult = 0;
}
