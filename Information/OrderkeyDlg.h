#pragma once
#include "resource.h"

class CTrueDBGridCtrl;
class CMsDgridCtrl;

/////////////////////////////////////////////////////////////////////////////
// COrderkeyDlg dialog

class COrderkeyDlg : public CDialog
{
	public:
	COrderkeyDlg(CWnd* pParent, CMsDgridCtrl* pDBGridCtrl, CTrueDBGridCtrl* pTDBGridCtrl);

	// Dialog Data
	//{{AFX_DATA(COrderkeyDlg)
	enum
	{
		IDD = IDD_ORDERKEY
	};

	CComboBox m_SKeyCombo;
	CComboBox m_FKeyCombo;
	//}}AFX_DATA

	CTrueDBGridCtrl* m_pTDBGridCtrl;
	CMsDgridCtrl* m_pDBGridCtrl;

	CString m_strOrder;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrderkeyDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(COrderkeyDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
