#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWhereTextPage dialog

class CWhereTextPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWhereTextPage)

	public:
	CWhereTextPage();

	// Dialog Data
	//{{AFX_DATA(CWhereTextPage)
	enum
	{
		IDD = IDD_WHERETEXT
	};

	//}}AFX_DATA
	public:
	CStringW* m_strWhere;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhereTextPage)
	public:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnSetActive() override;
	BOOL OnKillActive() override;
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CWhereTextPage)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
