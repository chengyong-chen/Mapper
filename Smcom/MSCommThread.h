#pragma once

// ThreadMSComm.h : header file

class CMSCommDlg;

/////////////////////////////////////////////////////////////////////////////
// CMSCommThread thread

class AFX_EXT_CLASS CMSCommThread : public CWinThread
{
	DECLARE_DYNCREATE(CMSCommThread)
protected:
	CMSCommThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	CMSCommDlg* m_pMSCommDlg;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSCommThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CMSCommThread() override;

	// Generated message map functions
	//{{AFX_MSG(CMSCommThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
