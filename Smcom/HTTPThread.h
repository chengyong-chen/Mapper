#pragma once

/////////////////////////////////////////////////////////////////////////////
// CHTTPThread thread

#include "ComThread.h"

class CHTTPSession;

class AFX_EXT_CLASS CHTTPThread : public CComThread
{
	DECLARE_DYNCREATE(CHTTPThread)
protected:
	CHTTPThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	CHTTPSession* m_pHTTPSession;

	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHTTPThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	~CHTTPThread() override;

	// Generated message map functions
	//{{AFX_MSG(CHTTPThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
