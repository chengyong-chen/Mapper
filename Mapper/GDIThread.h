#pragma once

class CGDIThread : public CWinThread
{
	DECLARE_DYNCREATE(CGDIThread)
protected:
	CGDIThread();           // protected constructor used by dynamic creation
	CGDIThread(CWnd* pWnd, HDC hDC);

// Attributes
public:
	HDC m_hDC;
	CDC m_pDC;

	HANDLE m_hEventKill;
	HANDLE m_hEventDead;
	static HANDLE m_hAnotherDead;

	static CRITICAL_SECTION m_csGDILock;

// Operations
public:	
	void KillThread();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGDIThread)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGDIThread();
	virtual void Delete() override;

	// Generated message map functions
	//{{AFX_MSG(CGDIThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

