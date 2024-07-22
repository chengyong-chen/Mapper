// FieldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../Public/Fieldset.h"
#include "FieldsPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldsPicker dialog

CFieldsPicker::CFieldsPicker(CWnd* pParent, CDatabase* pDatabase, CString strTable, CString strField, float minimumScale, float maximumScale)
	: CDialog(CFieldsPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFieldsPicker)

	m_minimumScale = 0;
	m_maximumScale = 10000;
	//}}AFX_DATA_INIT
	m_pDatabase = pDatabase;
	m_strTable = strTable;
	m_strField = strField;
	m_minimumScale = minimumScale;
	m_maximumScale = maximumScale;
}

void CFieldsPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldsPicker)
	DDX_Control(pDX, IDC_FIELDCOMBO, m_FieldCombo);

	DDX_Text(pDX, IDC_MINIMUMSCALE, m_minimumScale);
	DDX_Text(pDX, IDC_MAXIMUMSCALE, m_maximumScale);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFieldsPicker, CDialog)
	//{{AFX_MSG_MAP(CFieldsPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldsPicker message handlers

BOOL CFieldsPicker::OnInitDialog()
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

void CFieldsPicker::OnOK()
{
	const int nIndex = ((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetCurSel();
	if(nIndex!=-1)
		((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetLBText(nIndex, m_strField);
	else
		m_strField.Empty();

	CDialog::OnOK();
}
