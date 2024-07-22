#pragma once

#include "Resource.h"

class CAtlas;

class CAtlasDlg : public CDialog
{
public:
	CAtlasDlg(CWnd* pParent = nullptr, CAtlas* m_Atlas = nullptr);

	// Dialog Data
	//{{AFX_DATA(CAtlasDlg)
	enum
	{
		IDD = IDD_ATLAS
	};

	BOOL m_bHtml;
	BOOL m_bNavigate;
	BOOL m_bDatabase;
	BOOL m_bFree;
	BOOL m_bEdit;
	BOOL m_bRegionQuery;
	CString m_strIndexHtml;
	CString m_strTitle;
	CString m_strHomePage;
	CString m_strName;
	//}}AFX_DATA

public:
	CAtlas* m_pAtlas;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtlasDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAtlasDlg)
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
