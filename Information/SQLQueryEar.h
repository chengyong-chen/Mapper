#pragma once

#include "Resource.h"



class __declspec(dllexport) CSQLQueryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSQLQueryPage)
// Construction
public:
	CSQLQueryPage();  

// Dialog Data
	//{{AFX_DATA(CSQLQueryPage)
	enum { IDD = IDD_SQLQUERY };
	CListCtrl	m_listCtrl;
	//}}AFX_DATA

public:
	CString m_strSQL;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSQLQueryPage)
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSQLQueryPage)
	afx_msg void OnOK();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuery();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetSQL();
	//}}AFX_MSG
	afx_msg LONG OnSetCount(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
