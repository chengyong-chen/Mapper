#include "stdafx.h"
#include "LayTableDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayTableDlg dialog

CLayTableDlg::CLayTableDlg(CWnd* pParent)
	: CDialog(CLayTableDlg::IDD, pParent)
{
	//{{AFX_DATA
	//}}AFX_DATA_INIT
}

CLayTableDlg::CLayTableDlg(CWnd* pParent, CString geoTable, CString attTable, CString strWhere, CString strTag)
	: CDialog(CLayTableDlg::IDD, pParent)
{
	//{{AFX_DATA
	m_geoTable = geoTable;
	m_attTable = attTable;
	m_strWhere = strWhere;
	m_strTag = strTag;
	//}}AFX_DATA_INIT
}

void CLayTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayTableDlg)
	DDX_Text(pDX, IDC_DATATABLE, m_geoTable);
	DDX_Text(pDX, IDC_ATTRIBUTETABLE, m_attTable);
	DDX_Text(pDX, IDC_WHERE, m_strWhere);
	DDX_Text(pDX, IDC_TIPFIELD, m_strTag);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLayTableDlg, CDialog)
	//{{AFX_MSG_MAP(CLayTableDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayTableDlg message handlers

BOOL CLayTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);
	return TRUE;
}
