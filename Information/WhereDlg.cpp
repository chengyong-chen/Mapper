#include "stdafx.h"
#include "WhereDlg.h"

#include "../Public/Tableset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhereDlg dialog

CWhereDlg::CWhereDlg(CWnd* pParent, CDatabase* pDatabase, CString strTable, CString strWhere)
	: CDialog(CWhereDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhereDlg)
	//}}AFX_DATA_INIT
	m_pDatabase = pDatabase;
	m_strTable = strTable;
	m_strWhere = strWhere;
}

void CWhereDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhereDlg)
	DDX_Control(pDX, IDC_TABLE, m_TableCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWhereDlg, CDialog)
	//{{AFX_MSG_MAP(CWhereDlg)
	ON_CBN_SELCHANGE(IDC_TABLE, OnSelchangeTable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhereDlg message handlers

BOOL CWhereDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pDatabase==nullptr)
		return FALSE;
	else if(m_pDatabase->IsOpen()==FALSE)
		return FALSE;

	m_pickWherePage.m_pDatabase = m_pDatabase;
	m_pickWherePage.m_strTable = m_strTable;
	m_pickWherePage.m_strWhere = &m_strWhere;
	m_textWherePage.m_strWhere = &m_strWhere;

	try
	{
		CTableset set(m_pDatabase);
		if(set.Open(nullptr, nullptr, nullptr, "'TABLE'", CRecordset::OpenType::snapshot)==TRUE)
		{
			while(set.IsEOF()==FALSE)
			{
				if(set.m_strTableOwner.CompareNoCase(_T("sys")) == 0)
				{
				}
				else if(set.m_strTableName.CompareNoCase(_T("Query")) == 0)
				{
				}
				else
				{
					m_TableCombo.AddString(set.m_strTableName);
				}
				set.MoveNext();				
			}

			set.Close();
		}

		if(set.Open(nullptr, nullptr, nullptr, "'VIEW'", CRecordset::forwardOnly)==TRUE)
		{
			while(set.IsEOF()==FALSE)
			{
				CString strTable = set.m_strTableName;
				CString strType = set.m_strTableType;
				CString strOwner = set.m_strTableOwner;

				set.MoveNext();

				if(strTable.CompareNoCase(_T("Query"))==0)
					continue;
				else if(strOwner.CompareNoCase(_T("INFORMATION_SCHEMA"))==0)
					continue;
				else if(strOwner.CompareNoCase(_T("sys"))==0)
					continue;

				m_TableCombo.AddString(strTable);
			}
			set.Close();
		}

		if(m_strTable.IsEmpty()==FALSE)
			m_TableCombo.SelectString(-1, m_strTable);
		else
		{
			m_TableCombo.SetCurSel(0);
			OnSelchangeTable();
		}
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	m_WhereSheet.AddPage(&m_textWherePage);
	m_WhereSheet.AddPage(&m_pickWherePage);

	m_WhereSheet.Create(this, WS_CHILD|WS_VISIBLE, 0);
	m_WhereSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_WhereSheet.ModifyStyle(0, WS_TABSTOP);

	CRect frameSheet;
	GetDlgItem(IDC_WHEREFRAME)->GetWindowRect(&frameSheet);
	GetDlgItem(IDC_WHEREFRAME)->ShowWindow(SW_HIDE);
	this->ScreenToClient(&frameSheet);
	m_WhereSheet.SetWindowPos(nullptr, frameSheet.left, frameSheet.top, frameSheet.Width(), frameSheet.Height(), SWP_NOZORDER|SWP_NOACTIVATE);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWhereDlg::OnOK()
{
	m_WhereSheet.SendMessage(WM_COMMAND, IDOK, 0);
	//	m_strWhere = m_WhereCtrl.m_strWhere;

	CDialog::OnOK();
}

void CWhereDlg::OnSelchangeTable()
{
	const int index = m_TableCombo.GetCurSel();
	if(index!=-1)
	{
		CString strTable;
		m_TableCombo.GetLBText(index, strTable);
		if(strTable!=m_strTable)
		{
			m_strTable = strTable;
			m_strWhere.Empty();
			if(m_pickWherePage.m_hWnd!=nullptr)
			{
				m_pickWherePage.m_strTable = m_strTable;
				m_pickWherePage.m_ListCtrl.DeleteAllItems();
			}
		}
	}
}
