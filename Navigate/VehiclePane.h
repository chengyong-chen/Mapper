#include "LED.h"

#pragma once

// NavinfoPane.h : header file

class CCoder;

/////////////////////////////////////////////////////////////////////////////
// CVehiclePane dialog

class __declspec(dllexport) CVehiclePane : public CDockablePane
{
	public:
	CVehiclePane();

	public:
	CLED* m_pLED;
	CStatic* m_pTitle;
	int m_nCount;

	public:
	CCoder* m_pCoder;
	DWORD m_dwVehicle;
	DWORD m_dwStatus;

	public:
	void ShowStatus(CCoder* pCoder, DWORD dwVehicle, DWORD dwStatus);

	void ChangeIVU(CCoder* pCoder);
	void SetVehicle(DWORD dwVehicle);
	void SetStatus(DWORD dwStatus);

	public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehiclePane)
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
	protected:
	// Generated message map functions
	//{{AFX_MSG(CVehiclePane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
};
