#include "Stdafx.h"
#include "GEODatasourceDlg.h"

#include "../Public/Tableset.h"
#include "../Public/Fieldset.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/Datasource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGEODatasourceDlg::CGEODatasourceDlg(CWnd* pParent, CODBCDatabase* pDatabase, CGEOHeaderProfile& headerfile)
	: CDialogEx(CGEODatasourceDlg::IDD, pParent), m_pDatabase(pDatabase), m_headerfile(headerfile)
{
}

CGEODatasourceDlg::~CGEODatasourceDlg()
{
}

void CGEODatasourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WHEREA, m_headerfile.m_whereA);
	DDX_Text(pDX, IDC_WHEREB, m_headerfile.m_whereB);
}

BEGIN_MESSAGE_MAP(CGEODatasourceDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_LBN_SELCHANGE(IDC_COMBOTABLEB, OnSelchangeTableB)
END_MESSAGE_MAP()

BOOL CGEODatasourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_pDatabase!=nullptr)
	{
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
					CString strTable=rs.m_strTableName;
					((CComboBox*)GetDlgItem(IDC_COMBOTABLEA))->AddString(strTable);
					((CComboBox*)GetDlgItem(IDC_COMBOTABLEB))->AddString(strTable);
					((CComboBox*)GetDlgItem(IDC_COMBOTABLEC))->AddString(strTable);
				}
				rs.MoveNext();
			}

			rs.Close();
		}
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEA))->SelectString(-1, m_headerfile.m_tableA);
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEB))->SelectString(-1, m_headerfile.m_tableB);
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEC))->SelectString(-1, m_headerfile.m_tableC);

		OnSelchangeTableB();
	}
	return TRUE;
}

void CGEODatasourceDlg::OnSelchangeTableB()
{
	CComboBox* pAnnoCombo = (CComboBox*)GetDlgItem(IDC_COMBOANNO);
	pAnnoCombo->ResetContent();
	if(m_pDatabase!=nullptr)
	{
		const CComboBox* pTableCombo = (CComboBox*)GetDlgItem(IDC_COMBOTABLEB);
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
					((CComboBox*)GetDlgItem(IDC_COMBOANNO))->AddString(strColumn);
					rs.MoveNext();
				}

				rs.Close();
			}
			m_headerfile.m_tableB = strTable;
			((CComboBox*)GetDlgItem(IDC_COMBOANNO))->SelectString(-1, m_headerfile.m_strAnnoField);
		}
		else
		{
			m_headerfile.m_tableA = _T("");
		}
	}
}

void CGEODatasourceDlg::OnOk()
{
	const int index1 = ((CComboBox*)GetDlgItem(IDC_COMBOTABLEA))->GetCurSel();
	const int index2 = ((CComboBox*)GetDlgItem(IDC_COMBOTABLEB))->GetCurSel();
	const int index3 = ((CComboBox*)GetDlgItem(IDC_COMBOTABLEC))->GetCurSel();
	const int index4 = ((CComboBox*)GetDlgItem(IDC_COMBOANNO))->GetCurSel();
	if(index1!=-1)
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEA))->GetLBText(index1, m_headerfile.m_tableA);
	if(index2!=-1)
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEB))->GetLBText(index2, m_headerfile.m_tableB);
	if(index3!=-1)
		((CComboBox*)GetDlgItem(IDC_COMBOTABLEC))->GetLBText(index3, m_headerfile.m_tableC);
	if(index4!=-1)
		((CComboBox*)GetDlgItem(IDC_COMBOANNO))->GetLBText(index4, m_headerfile.m_strAnnoField);

	CDialog::OnOK();
}
