#pragma once

class CPrefDlg : public CDialog
{
// Construction
public:
	CPrefDlg(CWnd* pParent = nullptr);  

// Dialog Data
	//{{AFX_DATA(CPrefDlg)
	enum { IDD = IDD_PREFERENCES };
	CComboBox	m_unitcombo;
	long    	m_nGreekType;
	long	    m_nUndoNumber;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrefDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
