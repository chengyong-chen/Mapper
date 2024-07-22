#ifndef _SOCKETPAGE_H_
#define _SOCKETPAGE_H_

#pragma once

// SocketDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSocketPage dialog

class CSocketPage : public CPropertyPage
{
	public:
	CSocketPage();

	~CSocketPage() override;

	// Dialog Data
	//{{AFX_DATA(CSocketPage)
	enum
	{
		IDD = IDD_SOCKET
	};

	int m_nPort;
	CString m_strServer;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CSocketPage)
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
