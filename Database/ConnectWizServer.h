#pragma once

#include "Resource.h"
class CODBCDatabase;
class CConnectWizServer : public CPropertyPage
{
public:
	CConnectWizServer(CODBCDatabase& ConnectInfo);

	~CConnectWizServer() override;

	// Dialog Data
	//{{AFX_DATA(CConnectWizServer)
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_DBSERVER
	};

	CEdit m_edDbName;

	CStatic m_stcDbText;

	CEdit m_edServerName;

	CStatic m_stcSnText;

	CStatic m_stcSnCaption;

	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectWizServer)
public:
	BOOL OnSetActive() override;

	LRESULT OnWizardNext() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectWizServer)
	BOOL OnInitDialog() override;

	afx_msg void OnChangeEditDbname();
	afx_msg void OnChangeEdtServerName();
	afx_msg void OnEnChangeEdtPort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetWizardButtons();

	CString m_sDbType;

	CODBCDatabase& m_ConnectInfo;

public:
	CStatic m_stcPort;

	CEdit m_edtPort;
};
