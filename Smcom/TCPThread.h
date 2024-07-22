#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTCPThread thread

#include "ComThread.h"

class CUDPSocket;
class CTCPSocket;

class AFX_EXT_CLASS CTCPThread : public CComThread
{
	DECLARE_DYNCREATE(CTCPThread)
protected:
	CTCPThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	CTCPSocket* m_pTCPSocket;

public:
	BOOL SendVehicleMsg(WPARAM wParam, LPARAM lParam) override;

	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTCPThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	~CTCPThread() override;

	// Generated message map functions
	//{{AFX_MSG(CTCPThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
