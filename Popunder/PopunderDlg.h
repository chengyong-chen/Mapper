#pragma once

#include "resource.h"
#include "EdgeWebCtrl.h"


class CPopunderDlg : public CDialog
{
public:
	CPopunderDlg(CWnd* pParent = nullptr);

	enum
	{
		IDD = IDD_POPUNDER
	};

	DWORD m_dwOwner;
	HANDLE m_hReadPipe;
	HANDLE m_hWritePipe;
public:
	CEdgeWebCtrl m_ctrlEdge;

public:
	void SendMessageToPipe(std::string str) const;
	
	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	BOOL OnInitDialog() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	
protected:
	//{{AFX_MSG(CPopunderDlg)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG		
	DECLARE_MESSAGE_MAP()
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	
public:
	static UINT ReadFromPipe(LPVOID lParam);
};
