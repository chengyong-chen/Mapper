#pragma once

/////////////////////////////////////////////////////////////////////////////
// CConnectWizLogin dialog

#include "Resource.h"
class CODBCDatabase;
class CConnectWizLogin : public CPropertyPage
{
public:
	CConnectWizLogin(CODBCDatabase& ConnectInfo);

	~CConnectWizLogin() override;

	// Dialog Data
	//{{AFX_DATA(CConnectWizLogin)
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_USERINFO
	};

	CEdit m_edPassword;

	CEdit m_edUserName;

	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectWizLogin)
public:
	BOOL OnSetActive() override;

	LRESULT OnWizardNext() override;

	LRESULT OnWizardBack() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectWizLogin)
	afx_msg void OnBtnTest();

	afx_msg void OnChangeEdUsername();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetWizardButtons() const;

	CODBCDatabase& m_ConnectInfo;
};
