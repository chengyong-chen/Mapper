// SearchPage.cpp : implementation file
//

#include "stdafx.h"
#include "SearchPage.h"

#include "Vehicle.h"

#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchPage property page

IMPLEMENT_DYNCREATE(CSearchPage, CPropertyPage)

CSearchPage::CSearchPage() : CPropertyPage(CSearchPage::IDD)
{
	//{{AFX_DATA_INIT(CSearchPage)

	m_bTrack = FALSE;
	m_bAutoin = FALSE;
	m_bDrawTrace = FALSE;
	m_bSaveTrace = FALSE;
	m_bRotateMap = FALSE;
	//}}AFX_DATA_INIT

	m_bAutoin = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Autoin"), 1);
	m_bDrawTrace = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("DrawTrace"), 0);
	m_bSaveTrace = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("SaveTrace"), 0);
	m_bTrack = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Track"), 1);
	m_bRotateMap = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Rotatemap"), 0);
}

CSearchPage::~CSearchPage()
{
}

void CSearchPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here	
	DDX_Check(pDX, IDC_TRACKVEHICLE, m_bTrack);
	DDX_Check(pDX, IDC_AUTOIN, m_bAutoin);
	DDX_Check(pDX, IDC_DRAWTRACE, m_bDrawTrace);
	DDX_Check(pDX, IDC_SAVETRACE, m_bSaveTrace);
	DDX_Check(pDX, IDC_ROTATEMAP, m_bRotateMap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchPage message handlers

BOOL CSearchPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	const int nPriority = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Priority"), 0);
	switch(nPriority)
	{
	case 0:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
		break;
	}
	const CString strOilPrice = AfxGetApp()->GetProfileString(_T("Navigate"), _T("OilPrice"), _T("3.2"));
	const CString strOilLitre = AfxGetApp()->GetProfileString(_T("Navigate"), _T("OilLitre"), _T("10"));
	const CString strExpressPrice = AfxGetApp()->GetProfileString(_T("Navigate"), _T("ExpressPrice"), _T("0.5"));

	GetDlgItem(IDC_EDIT1)->SetWindowText(strOilPrice);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strOilLitre);
	GetDlgItem(IDC_EDIT3)->SetWindowText(strExpressPrice);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchPage::OnOK()
{
	CPropertyPage::OnOK();

	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	AfxGetApp()->WriteProfileInt(_T("Receiver"), _T("Autoin"), m_bAutoin);
	AfxGetApp()->WriteProfileInt(_T("Receiver"), _T("DrawTrace"), m_bDrawTrace);
	AfxGetApp()->WriteProfileInt(_T("Receiver"), _T("SaveTrace"), m_bSaveTrace);
	AfxGetApp()->WriteProfileInt(_T("Receiver"), _T("Track"), m_bTrack);
	AfxGetApp()->WriteProfileInt(_T("Receiver"), _T("RotateMap"), m_bRotateMap);

	AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, m_bTrack==true ? GPSSERVER_VEHICLE_TRACK : GPSSERVER_VEHICLE_UNTRACK, 0);
	AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, m_bDrawTrace==true ? GPSSERVER_TRACE_DRAW : GPSSERVER_TRACE_UNDRAW, 0);
	AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, m_bSaveTrace==true ? GPSSERVER_TRACE_SAVE : GPSSERVER_TRACE_UNSAVE, 0);

	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck()==BST_CHECKED)
	{
		AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Priority"), 0);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()==BST_CHECKED)
	{
		AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Priority"), 1);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck()==BST_CHECKED)
	{
		AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Priority"), 2);
	}

	CString strOilPrice;
	CString strOilLitre;
	CString strExpressPrice;
	GetDlgItem(IDC_EDIT1)->GetWindowText(strOilPrice);
	GetDlgItem(IDC_EDIT2)->GetWindowText(strOilLitre);
	GetDlgItem(IDC_EDIT3)->GetWindowText(strExpressPrice);

	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("OilPrice"), strOilPrice);
	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("OilLitre"), strOilLitre);
	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("ExpressPrice"), strExpressPrice);
}
