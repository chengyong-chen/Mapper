// Orderkeycpp : implementation file
//

#include "stdafx.h"
#include "OrderkeyDlg.h"
#include "Columns.h"
#include "Column.h"

#include "TrueDBGridCtrl.h"
#include "MsdGridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderkeyDlg dialog

COrderkeyDlg::COrderkeyDlg(CWnd* pParent, CMsDgridCtrl* pDBGridCtrl, CTrueDBGridCtrl* pTDBGridCtrl)
	: CDialog(COrderkeyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COrderkeyDlg)

	//}}AFX_DATA_INIT
	m_pDBGridCtrl = pDBGridCtrl;
	m_pTDBGridCtrl = pTDBGridCtrl;
}

void COrderkeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderkeyDlg)
	DDX_Control(pDX, IDC_SECONDKEK, m_SKeyCombo);
	DDX_Control(pDX, IDC_FIRSTKEY, m_FKeyCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrderkeyDlg, CDialog)
	//{{AFX_MSG_MAP(COrderkeyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrderkeyDlg message handlers

BOOL COrderkeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_FASC))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_FDESC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SASC))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_SDESC))->SetCheck(FALSE);

	if(m_pDBGridCtrl!=nullptr)
	{
		CColumns columns = m_pDBGridCtrl->GetColumns();
		for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
		{
			VARIANT Var;
			Var.vt = VT_I4;
			Var.lVal = nIndex;

			CColumn column = columns.GetItem(Var);
			m_FKeyCombo.AddString(column.GetDataField());
			m_SKeyCombo.AddString(column.GetDataField());
		}
	}
	else if(m_pTDBGridCtrl!=nullptr)
	{
		CColumns columns = m_pTDBGridCtrl->GetColumns();
		for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
		{
			VARIANT Var;
			Var.vt = VT_I4;
			Var.lVal = nIndex;

			CColumn column = columns.GetItem(Var);
			m_FKeyCombo.AddString(column.GetDataField());
			m_SKeyCombo.AddString(column.GetDataField());
		}
	}

	if(m_FKeyCombo.GetCount()>0)
		m_FKeyCombo.SetCurSel(0);
	else
	{
		m_FKeyCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_FASC)->EnableWindow(FALSE);
		GetDlgItem(IDC_FDESC)->EnableWindow(FALSE);
	}

	if(m_SKeyCombo.GetCount()>1)
	{
		m_SKeyCombo.DeleteString(0);
		m_SKeyCombo.SetCurSel(0);
	}
	else
	{
		m_SKeyCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_SASC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SDESC)->EnableWindow(FALSE);
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COrderkeyDlg::OnOK()
{
	m_strOrder.Empty();

	int nIndex = m_FKeyCombo.GetCurSel();
	if(nIndex!=-1)
	{
		CString strKey;
		m_FKeyCombo.GetLBText(nIndex, strKey);
		if(strKey.IsEmpty()==FALSE)
		{
			if(((CButton*)GetDlgItem(IDC_FASC))->GetCheck()==BST_CHECKED)
			{
				strKey = _T("[")+strKey+_T("]");
				m_strOrder = m_strOrder+strKey+_T(" ASC ");
			}
			else if(((CButton*)GetDlgItem(IDC_FDESC))->GetCheck()==BST_CHECKED)
			{
				strKey = _T("[")+strKey+_T("]");
				m_strOrder = m_strOrder+strKey+_T(" DESC ");
			}
		}
	}
	nIndex = m_SKeyCombo.GetCurSel();
	if(nIndex!=-1)
	{
		CString strKey;
		m_SKeyCombo.GetLBText(nIndex, strKey);
		if(strKey.IsEmpty()==FALSE)
		{
			if(((CButton*)GetDlgItem(IDC_SASC))->GetCheck()==BST_CHECKED)
			{
				strKey = _T("[")+strKey+_T("]");
				m_strOrder = m_strOrder+_T(",")+strKey+_T(" ASC ");
			}
			else if(((CButton*)GetDlgItem(IDC_SDESC))->GetCheck()==BST_CHECKED)
			{
				strKey = _T("[")+strKey+_T("]");
				m_strOrder = m_strOrder+_T(",")+strKey+_T(" DESC ");
			}
		}
	}

	CDialog::OnOK();
}
