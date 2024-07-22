#include "stdafx.h"
#include "TopoTableDlg.h"
#include "Topo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopoTableDlg dialog

CTopoTableDlg::CTopoTableDlg(CWnd* pParent)
	: CDialog(CTopoTableDlg::IDD, pParent)
{
	//{{AFX_DATA
	//}}AFX_DATA_INIT
}

CTopoTableDlg::CTopoTableDlg(CWnd* pParent, const CString& tableEdge, const CString& tableNode, const CString& tablePolyData, const CString& tablePolyAttribute, const CString& strFilter)
	: CDialog(CTopoTableDlg::IDD, pParent)
{
	//{{AFX_DATA
	m_tableEdge = tableEdge;
	m_tableNode = tableNode;
	m_tablePolyData = tablePolyData;
	m_tablePolyAttribute = tablePolyAttribute;
	m_strFilter = strFilter;
	//}}AFX_DATA_INIT
}

void CTopoTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTopoTableDlg)
	DDX_Text(pDX, IDC_TABLEEDGE, m_tableEdge);
	DDX_Text(pDX, IDC_TABLENODE, m_tableNode);
	DDX_Text(pDX, IDC_TABLEPOLYDATA, m_tablePolyData);
	DDX_Text(pDX, IDC_TABLEPOLYATTRIBUTE, m_tablePolyAttribute);
	DDX_Text(pDX, IDC_FILTER, m_strFilter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTopoTableDlg, CDialog)
	//{{AFX_MSG_MAP(CTopoTableDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopoTableDlg message handlers
