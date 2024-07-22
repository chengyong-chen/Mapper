#pragma once

/////////////////////////////////////////////////////////////////////////////
// CUDPThread thread

#include "ComThread.h"

class CUDPSocket;
class CTCPSocket;

class AFX_EXT_CLASS CUDPThread : public CComThread
{
	DECLARE_DYNCREATE(CUDPThread)
protected:
	CUDPThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	CUDPSocket* m_pUDPSocket;

public:
	BOOL SendVehicleMsg(WPARAM wParam, LPARAM lParam) override;

	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUDPThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	~CUDPThread() override;

	// Generated message map functions
	//{{AFX_MSG(CUDPThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
