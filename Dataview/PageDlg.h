#pragma once

#include "Resource.h"

// PageDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CPageDlg dialog

class CPageDlg : public CDialog
{
public:
	CPageDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPageDlg)
	enum
	{
		IDD = IDD_PAGE
	};

	float m_bottomMargin;
	float m_leftMargin;
	float m_rightMargin;
	float m_topMargin;
	BOOL m_bVCenter;
	BOOL m_bHCenter;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	void OnOK() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
