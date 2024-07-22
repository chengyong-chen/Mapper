#pragma once

#include "ComThread.h"

class CWavecomDlg;

class AFX_EXT_CLASS CSMSThread : public CComThread
{
	DECLARE_DYNCREATE(CSMSThread)
protected:
	CSMSThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	CWavecomDlg* m_pWavecomDlg;
	BOOL SendVehicleMsg(WPARAM wParam, LPARAM lParam) override;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMSThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	~CSMSThread() override;

	// Generated message map functions
	//{{AFX_MSG(CSMSThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
