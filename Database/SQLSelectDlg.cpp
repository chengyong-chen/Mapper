#include "stdafx.h"
#include "afxdialogex.h"

#include "SQLSelectDlg.h"
#include "SQLClause.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSQLSelectDlg::CSQLSelectDlg(CWnd* pParent, CClauseSelect& clause)
	: CDialogEx(CSQLSelectDlg::IDD, pParent), m_clause(clause)
{
}

CSQLSelectDlg::~CSQLSelectDlg()
{
}

void CSQLSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FIELDS, m_clause.m_strFields);
	DDX_Text(pDX, IDC_TABLES, m_clause.m_strTables);
	DDX_Text(pDX, IDC_WHERE, m_clause.m_strWhere);
	DDX_Text(pDX, IDC_ORDERBY, m_clause.m_strOrderby);
}

BEGIN_MESSAGE_MAP(CSQLSelectDlg, CDialogEx)
END_MESSAGE_MAP()
