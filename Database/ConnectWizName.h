#pragma once


using namespace std;
class CODBCDatabase;

/////////////////////////////////////////////////////////////////////////////
// CConnectWizName dialog

#include "Resource.h"

class CConnectWizName : public CPropertyPage
{
public:
	CConnectWizName(CString& strName, CODBCDatabase& ConnectInfo);

	~CConnectWizName() override;

	// Dialog Data
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_NAME
	};

	CEdit m_edName;

	// Overrides
public:
	BOOL OnSetActive() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	// Implementation
protected:
	// Generated message map functions
	BOOL OnInitDialog() override;

	afx_msg void SetWizardButtons();
	DECLARE_MESSAGE_MAP()

	CString& m_strName;

	CODBCDatabase& m_ConnectInfo;
};
