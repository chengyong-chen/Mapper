#include "Stdafx.h"
#include "ATTDatasourceDlg.h"

#include "../Public/ODBCDatabase.h"
#include "../Public/Tableset.h"
#include "../Public/Fieldset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CATTDatasourceDlg::CATTDatasourceDlg(CWnd* pParent, CODBCDatabase* pDatabase, CATTHeaderProfile& headerprofile)
	: CDialogEx(CATTDatasourceDlg::IDD, pParent), m_pDatabase(pDatabase), m_headerprofile(headerprofile)
{
}

CATTDatasourceDlg::~CATTDatasourceDlg()
{
}

void CATTDatasourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANNOFIELD, m_headerprofile.m_strAnnoField);
	DDX_Text(pDX, IDC_CODEFIELD, m_headerprofile.m_strCodeField);
	DDX_Text(pDX, IDC_TABLES, m_headerprofile.m_strTables);
	DDX_Text(pDX, IDC_FIELDS, m_headerprofile.m_strFields);
	DDX_Text(pDX, IDC_WHERE, m_headerprofile.m_strWhere);
}

BEGIN_MESSAGE_MAP(CATTDatasourceDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_COMBOKEYTABLE, OnSelchangeTable)
	ON_LBN_SELCHANGE(IDC_COMBOKEYFIELD, OnSelchangeKey)
END_MESSAGE_MAP()

BOOL CATTDatasourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_pDatabase!=nullptr)
	{
		CComboBox* pTableCombo = (CComboBox*)GetDlgItem(IDC_COMBOKEYTABLE);
		pTableCombo->EnableWindow(TRUE);
		CTableset rs(m_pDatabase);
		if(rs.Open(nullptr, nullptr, nullptr, "'TABLE'", CRecordset::OpenType::snapshot)==TRUE)
		{
			while(!rs.IsEOF())
			{
				if(rs.m_strTableOwner == _T("sys"))
				{	
				}
				else if(rs.m_strTableName == _T("Query"))
				{
				}
				else
				{
					pTableCombo->AddString(rs.m_strTableName);
				}
				rs.MoveNext();
			}

			rs.Close();
		}
		pTableCombo->SelectString(-1, m_headerprofile.m_strKeyTable);
	}
	OnSelchangeTable();
	return TRUE;
}

void CATTDatasourceDlg::OnSelchangeTable()
{
	CComboBox* pKeyCombo = (CComboBox*)GetDlgItem(IDC_COMBOKEYFIELD);
	pKeyCombo->ResetContent();
	if(m_pDatabase!=nullptr)
	{
		const CComboBox* pTableCombo = (CComboBox*)GetDlgItem(IDC_COMBOKEYTABLE);
		const int index = pTableCombo->GetCurSel();
		if(index!=-1)
		{
			CString strTable;
			pTableCombo->GetLBText(index, strTable);

			CFieldset rs(m_pDatabase);
			if(rs.Open(nullptr, nullptr, strTable, nullptr, CRecordset::OpenType::snapshot))
			{
				while(!rs.IsEOF())
				{
					CString strColumn = rs.m_strColumnName;
					((CComboBox*)GetDlgItem(IDC_COMBOKEYFIELD))->AddString(strColumn);
					rs.MoveNext();
				}

				rs.Close();
			}

			GetDlgItem(IDC_COMBOKEYFIELD)->EnableWindow(TRUE);
			pKeyCombo->SelectString(-1, _T("AID"));
			m_headerprofile.m_strKeyTable = strTable;
			m_headerprofile.m_strTables = strTable;
		}
		else
		{
			m_headerprofile.m_strKeyTable = _T("");
			m_headerprofile.m_strTables = _T("");
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBOKEYFIELD)->EnableWindow(FALSE);
		}
	}
	else
	{
		m_headerprofile.m_strKeyTable = _T("");
		m_headerprofile.m_strTables = _T("");
		GetDlgItem(IDC_COMBOKEYFIELD)->EnableWindow(FALSE);
	}
	OnSelchangeKey();
}

void CATTDatasourceDlg::OnSelchangeKey()
{
	const CComboBox* pKeyCombo = (CComboBox*)GetDlgItem(IDC_COMBOKEYFIELD);
	const int index = pKeyCombo->GetCurSel();
	if(index!=-1)
	{
		pKeyCombo->GetLBText(index, m_headerprofile.m_strIdField);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}
