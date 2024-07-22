#pragma once

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CModemPage dialog

class __declspec(dllexport) CModemPage : public CPropertyPage
{
	public:
	CModemPage(CWnd* pParent = nullptr);

	//{{AFX_DATA(CModemPage)
	enum
	{
		IDD = IDD_MODEM
	};

	CComboBox m_ctrlModem;
	int m_nPort;
	CString m_strPassword;
	CString m_strTelephone;
	CString m_strUser;
	CString m_strZone;
	BOOL m_bIVU;
	//}}AFX_DATA

	DWORD m_dwBaudRate;
	DWORD m_dwModem;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	static void DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData);
	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CModemPage)
	BOOL OnInitDialog() override;
	afx_msg void OnSelchangeModem();
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
