#include "stdafx.h"
#include "SQLDlg.h"
#include "../Public/Tableset.h"
#include "../Public/Fieldset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSQLDlg dialog

CString CSQLDlg::m_strTable = _T("");
CString CSQLDlg::m_strColumns = _T("*");
CDatabase* CSQLDlg::m_pDatabase = nullptr;

CSQLDlg::CSQLDlg(CWnd* pParent, CDatabase* pDatabase, CString strTable)
	: CDialog(CSQLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSQLDlg)
	//	m_strColumns = _T('*');
	//	m_strOrder = _T("");
	//	m_strTable = _T("");
	//	m_strWhere = _T("");
	//}}AFX_DATA_INIT

	if(m_pDatabase!=pDatabase)
	{
		m_strColumns = _T('*');
		m_strTable = _T("");
		m_pDatabase = pDatabase;
	}

	if(strTable.IsEmpty()==FALSE)
		m_strTable = strTable;
}

void CSQLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSQLDlg)
	DDX_Text(pDX, IDC_COLUMNS, m_strColumns);
	DDX_Text(pDX, IDC_TABLE, m_strTable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSQLDlg, CDialog)
	//{{AFX_MSG_MAP(CSQLDlg)
	ON_CBN_SELCHANGE(IDC_TABLECOMBO, OnSelchangeTable)
	ON_CBN_SELCHANGE(IDC_FIELDCOMBO, OnSelchangeFieldcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSQLDlg message handlers
BOOL CSQLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pDatabase->IsOpen()==TRUE)
	{
		m_pickWherePage.m_pDatabase = m_pDatabase;
		m_pickWherePage.m_strTable = m_strTable;
		m_pickWherePage.m_strWhere = &m_strWhere;
		m_textWherePage.m_strWhere = &m_strWhere;

		int sel = -1;
		int index = 0;
		CTableset rs(m_pDatabase);
		if(rs.Open(nullptr, nullptr, nullptr, "'TABLE'", CRecordset::OpenType::snapshot)==TRUE)
		{
			while(rs.IsEOF()==FALSE)
			{
				CString strTable = rs.m_strTableName;
				CString strType = rs.m_strTableType;
				CString strOwner = rs.m_strTableOwner;

				rs.MoveNext();

				((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->AddString(strTable);
				if(strTable==m_strTable)
				{
					sel = index;
				}
				index++;
			}
			rs.Close();
		}

		if(rs.Open(nullptr, nullptr, nullptr, "'VIEW'", CRecordset::forwardOnly)==TRUE)
		{
			while(rs.IsEOF()==FALSE)
			{
				CString strTable = rs.m_strTableName;
				CString strType = rs.m_strTableType;
				CString strOwner = rs.m_strTableOwner;

				rs.MoveNext();

				((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->AddString(strTable);
				if(strTable==m_strTable)
				{
					sel = index;
				}
				index++;
			}
			rs.Close();
		}

		if(((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->GetCount()>0&&sel!=-1)
		{
			((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->SetCurSel(sel);
			((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->GetLBText(sel, m_strTable);
			OnSelchangeTable();
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
	}

	UpdateData(FALSE);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSQLDlg::OnSelchangeTable()
{
	const int nIndex = ((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->GetCurSel();
	if(nIndex!=-1)
	{
		CString strTable;
		((CComboBox*)GetDlgItem(IDC_TABLECOMBO))->GetLBText(nIndex, strTable);
		if(strTable!=m_strTable)
		{
			const int count = ((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetCount();
			for(int i = 0; i<count; i++)
			{
				((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->DeleteString(0);
			}

			CFieldset rs(m_pDatabase);
			if(rs.Open(nullptr, nullptr, strTable, nullptr, CRecordset::OpenType::snapshot))
			{
				if(!rs.IsBOF())
				{
					while(!rs.IsEOF())
					{
						CString strColumn;
						const int nPos = rs.m_strColumnName.Find(_T('.'));
						if(nPos==-1)
						{
							strColumn = rs.m_strColumnName;
							((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);
						}
						else
						{
							strColumn = rs.m_strColumnName.Left(nPos);
							((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);

							strColumn = rs.m_strColumnName.Mid(nPos+1);
							((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);
						}

						rs.MoveNext();
					}
				}

				rs.Close();
			}

			m_strTable = strTable;
			m_strWhere.Empty();
			m_pickWherePage.m_strTable = m_strTable;
			m_pickWherePage.m_ListCtrl.DeleteAllItems();
		}
	}

	UpdateData(FALSE);
}

void CSQLDlg::OnSelchangeFieldcombo()
{
	const int nIndex = ((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetCurSel();
	if(nIndex==-1)
		return;

	CString strColumn;
	((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetLBText(nIndex, strColumn);
	if(strColumn.IsEmpty()==TRUE)
		return;

	strColumn = _T("[")+strColumn+_T("]");
	if(m_strColumns.IsEmpty()==TRUE)
	{
		m_strColumns = strColumn;
	}
	else if(m_strColumns==_T('*'))
	{
		m_strColumns = strColumn;
	}
	else if(m_strColumns.Find(strColumn)!=-1)
	{
		return;
	}
	else
	{
		m_strColumns += _T(",")+strColumn;
	}

	UpdateData(FALSE);
}

void CSQLDlg::OnOK()
{
	m_WhereSheet.SendMessage(WM_COMMAND, IDOK, 0);

	CDialog::OnOK();
}
