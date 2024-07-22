#include "stdafx.h"
#include "Marker.h"
#include "PrefDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern  BYTE  a_nUnitIndex;
extern  long  a_nUndoNumber;
extern  long  a_nGreekType;

extern __declspec(dllimport) BYTE d_nUnitIndex;

CPrefDlg::CPrefDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPrefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrefDlg)
	m_nGreekType  = a_nGreekType;
	m_nUndoNumber = a_nUndoNumber;
	//}}AFX_DATA_INIT
}
	
void CPrefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDlg)
	DDX_Control(pDX, IDC_UNITCOMBO, m_unitcombo);
	DDX_Text(pDX, IDC_GREEKTYPE, m_nGreekType);
	DDV_MinMaxInt(pDX, m_nGreekType, 3, 100);
	DDX_Text(pDX, IDC_UNDONUMBER, m_nUndoNumber);
	DDV_MinMaxInt(pDX, m_nUndoNumber, 0, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefDlg, CDialog)
	//{{AFX_MSG_MAP(CPrefDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefDlg message handlers

BOOL CPrefDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_unitcombo.SetCurSel(a_nUnitIndex);
	
	CenterWindow();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPrefDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	
	a_nUnitIndex  = m_unitcombo.GetCurSel();
	d_nUnitIndex  = m_unitcombo.GetCurSel();
	
//	a_nGreekType  = m_nGreekType;
//	a_nUndoNumber = m_nUndoNumber;
}
