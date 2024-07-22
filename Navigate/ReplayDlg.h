#pragma once
#include "SelectDlg.h"

class CPort;

class __declspec(dllexport) CReplayDlg : public CDialog
{
	public:
	CReplayDlg(CWnd* pParent, CPort* pPort);

	// Dialog Data
	//{{AFX_DATA(CReplayDlg)
	enum
	{
		IDD = IDD_REPLAY
	};

	COleDateTime m_Date1;
	COleDateTime m_Date2;
	COleDateTime m_Time1;
	COleDateTime m_Time2;

	BOOL m_bHandle;
	BOOL m_bHashing;
	//}}AFX_DATA

	public:
	CPort* m_pPort;
	CSelectDlg m_SelectDlg;
	DWORD m_dwVehicle;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplayDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CReplayDlg)
	BOOL OnInitDialog() override;
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
