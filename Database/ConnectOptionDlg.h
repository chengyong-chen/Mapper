#pragma once

#include "Resource.h"

class CODBCDatabase;
class CConnectOptionDlg : public CDialog
{
public:
	CConnectOptionDlg(CODBCDatabase& ConnectInfo);

	// Dialog Data
	//{{AFX_DATA(CConnectOptionDlg)
	enum
	{
		IDD = IDD_ADOCONNECTOPTION
	};

	CString m_cOptionString;

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectOptionDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectOptionDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
