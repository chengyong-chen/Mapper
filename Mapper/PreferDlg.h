#pragma once

class CPreferDlg : public CDialog
{
public:
	CPreferDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPreferDlg)
	enum
	{
		IDD = IDD_PREFER
	};

	CComboBox m_unitcombo;
	long m_nGreekType;
	long m_nUndoNumber;
	float m_fJoinTolerance;
	float m_fTopoTolerance;
	float m_fDensity;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
