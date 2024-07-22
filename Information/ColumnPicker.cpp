// FieldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../Public/Fieldset.h"
#include "ColumnPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColumnPicker dialog

CColumnPicker::CColumnPicker(CWnd* pParent, CDatabase* pDatabase, CString strTable)
	: CDialog(CColumnPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColumnPicker)
	//}}AFX_DATA_INIT
	m_pDatabase = pDatabase;
	m_strTable = strTable;
}

void CColumnPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColumnPicker)
	DDX_Control(pDX, IDC_FIELDCOMBO, m_FieldCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColumnPicker, CDialog)
	//{{AFX_MSG_MAP(CColumnPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColumnPicker message handlers

BOOL CColumnPicker::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pDatabase==nullptr)
		return FALSE;
	if(m_strTable.IsEmpty()==TRUE)
		return FALSE;

	if(m_pDatabase->IsOpen()==TRUE)
	{
		CFieldset rs(m_pDatabase);
		if(rs.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot))
		{
			while(!rs.IsEOF())
			{
				CString strColumn = rs.m_strColumnName;
				const int nPos = strColumn.Find(_T('.'));
				if(nPos==-1)
				{
					((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);
				}
				else
				{
					strColumn = strColumn.Left(nPos);
					((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);
					strColumn = rs.m_strColumnName;
					strColumn = strColumn.Mid(nPos+1);
					((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->AddString(strColumn);
				}

				rs.MoveNext();
			}

			rs.Close();
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->SelectString(-1, m_strField);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColumnPicker::OnOK()
{
	const int nIndex = ((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetCurSel();
	if(nIndex!=-1)
		((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetLBText(nIndex, m_strField);
	else
		m_strField.Empty();

	CDialog::OnOK();
}
