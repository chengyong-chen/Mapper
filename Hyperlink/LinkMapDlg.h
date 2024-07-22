#pragma once

#include "Resource.h"

class CLinkMap;

class CLinkMapDlg : public CDialog
{
public:
	CLinkMapDlg(CWnd* pParent = nullptr, CLinkMap* plink = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLinkMapDlg)
	enum
	{
		IDD = IDD_LINKMAP
	};

	CString m_strMap;
	UINT m_uZoom;
	long m_nX;
	long m_nY;
	//}}AFX_DATA

private:
	CLinkMap* d_pLink;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkMapDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkMapDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
