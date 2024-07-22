#include "Stdafx.h"
#include "POUDatasourceDlg.h"

#include "../Public/Tableset.h"
#include "../Public/Fieldset.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/Datasource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPOUDatasourceDlg::CPOUDatasourceDlg(CWnd* pParent, CODBCDatabase* pDatabase, CPOUHeaderProfile& headerprofile)
	: CDialogEx(CPOUDatasourceDlg::IDD, pParent), m_pDatabase(pDatabase), m_headerprofile(headerprofile)
{
}

CPOUDatasourceDlg::~CPOUDatasourceDlg()
{
}
template<typename E>
void AFXAPI DDX_CBIndexEnum(CDataExchange* pDX, int nIDC, E& value)
{
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if(pDX->m_bSaveAndValidate)
		value = static_cast<E>(::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L));
	else
		::SendMessage(hWndCtrl, CB_SETCURSEL, static_cast<WPARAM>(value), 0L);
}
void CPOUDatasourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANNOFIELD, m_headerprofile.m_strAnnoField);
	DDX_Text(pDX, IDC_COMBOKEYFIELD, m_headerprofile.m_strIdField);	
	DDX_Text(pDX, IDC_XFIELD, m_headerprofile.m_strXField);
	DDX_Text(pDX, IDC_YFIELD, m_headerprofile.m_strYField);
	DDX_CBIndexEnum(pDX, IDC_COMBOCOOR, m_headerprofile.m_coordinatortype);
	DDX_Text(pDX, IDC_TABLES, m_headerprofile.m_strTables);
	DDX_Text(pDX, IDC_FIELDS, m_headerprofile.m_strFields);
	DDX_Text(pDX, IDC_WHERE, m_headerprofile.m_strWhere);
}

BEGIN_MESSAGE_MAP(CPOUDatasourceDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_LBN_SELCHANGE(IDC_COMBOKEYTABLE, OnSelchangeTable)
	ON_LBN_SELCHANGE(IDC_COMBOKEYFIELD, OnSelchangeKey)
END_MESSAGE_MAP()

BOOL CPOUDatasourceDlg::OnInitDialog()
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
					pTableCombo->AddString(rs.m_strTableName);

				rs.MoveNext();
			}

			rs.Close();
		}
		pTableCombo->SelectString(-1, m_headerprofile.m_strKeyTable);
	}
	((CComboBox*)GetDlgItem(IDC_COMBOCOOR))->SetCurSel((int)m_headerprofile.m_coordinatortype);
	OnSelchangeTable();
	return TRUE;
}

void CPOUDatasourceDlg::OnSelchangeTable()
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
			pKeyCombo->SelectString(-1, _T("PID"));
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

void CPOUDatasourceDlg::OnSelchangeKey()
{
	const CComboBox* pKeyCombo = (CComboBox*)GetDlgItem(IDC_COMBOKEYFIELD);
	//	assert(pKeyCombo != nullptr);
	const int index = pKeyCombo->GetCurSel();
	if(index!=-1)
	{
		CString dd = _T("PID");
		pKeyCombo->GetLBText(index, dd);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

void CPOUDatasourceDlg::OnOk()
{
	m_headerprofile.m_coordinatortype = (CPOUHeaderProfile::COORDINATORTYPE)((CComboBox*)GetDlgItem(IDC_COMBOCOOR))->GetCurSel();
	CDialog::OnOK();
}
