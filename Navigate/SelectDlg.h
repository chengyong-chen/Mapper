#pragma once

#include "resource.h"

#define WM_SELDLG_DBCLICK  (WM_USER + 7000)
#define WM_SELDLG_CLICK    (WM_USER + 7001)
#define WM_SELDLG_RCLICK   (WM_USER + 7002)

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog
class CSelectDlg : public CDialog
{
	public:
	CSelectDlg(CWnd* pParent = nullptr, CDatabase* pDatabase = nullptr, DWORD dwUserID = 0);
	void SelectItemByData(const DWORD& dwId);

	// Dialog Data
	//{{AFX_DATA(CSelectDlg)
	enum
	{
		IDD = IDD_SELECT
	};

	CListCtrl m_listctrl;
	//}}AFX_DATA

	CDatabase* m_pDatabase;
	DWORD m_dwUser;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDlg)
	// NOTE: the ClassWizard will add member functions here
	BOOL OnInitDialog() override;
	public:
	afx_msg void OnSearch();
	afx_msg void OnGroup();
	afx_msg void OnVehicle();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
