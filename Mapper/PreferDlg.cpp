#include "stdafx.h"
#include "Resource.h"
#include "PreferDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferDlg dialog

extern BYTE a_nUnitIndex;
extern long a_nUndoNumber;
extern long a_nGreekType;
extern float a_fJoinTolerance;
extern float a_fTopoTolerance;
extern float a_fDensity;

extern __declspec(dllimport) BYTE d_nUnitIndex;

CPreferDlg::CPreferDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPreferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferDlg)
	m_nGreekType = a_nGreekType;
	m_nUndoNumber = a_nUndoNumber;
	m_fJoinTolerance = a_fJoinTolerance;
	m_fTopoTolerance = a_fTopoTolerance;
	m_fDensity = a_fDensity;
	//}}AFX_DATA_INIT
}

void CPreferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferDlg)
	DDX_Control(pDX, IDC_UNITCOMBO, m_unitcombo);
	DDX_Text(pDX, IDC_GREEKTYPE, m_nGreekType);
	DDV_MinMaxInt(pDX, m_nGreekType, 3, 100);
	DDX_Text(pDX, IDC_UNDONUMBER, m_nUndoNumber);
	DDV_MinMaxInt(pDX, m_nUndoNumber, 0, 10);
	DDX_Text(pDX, IDC_JOINTOLERANCE, m_fJoinTolerance);
	DDX_Text(pDX, IDC_TOPOTOLERANCE, m_fTopoTolerance);
	DDV_MinMaxInt(pDX, m_fJoinTolerance, 0.1, 10);
	DDV_MinMaxInt(pDX, m_fTopoTolerance, 0.1, 10);
	DDX_Text(pDX, IDC_DENSITY, m_fDensity);
	DDV_MinMaxInt(pDX, m_fDensity, 1, 10);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferDlg message handlers

BOOL CPreferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_unitcombo.SetCurSel(a_nUnitIndex);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferDlg::OnOK()
{
	// TODO: Add extra validation here
	CDialog::OnOK();

	a_nUnitIndex = m_unitcombo.GetCurSel();
	d_nUnitIndex = m_unitcombo.GetCurSel();

	a_nGreekType = m_nGreekType;
	a_nUndoNumber = m_nUndoNumber;
	a_fJoinTolerance = m_fJoinTolerance;
	a_fTopoTolerance = m_fTopoTolerance;
	a_fDensity = m_fDensity;
}
