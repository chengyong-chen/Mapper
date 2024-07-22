#pragma once

/////////////////////////////////////////////////////////////////////////////
// CWizard

class CWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CWizard)

public:
	CWizard(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0, HBITMAP hbmWatermark = nullptr, HPALETTE hpalWatermark = nullptr, HBITMAP hbmHeader = nullptr, const bool& bHasPreFinishBtn = false, const bool& bHasHelpIcon = false);

	CWizard(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0, HBITMAP hbmWatermark = nullptr, HPALETTE hpalWatermark = nullptr, HBITMAP hbmHeader = nullptr, const bool& bHasPreFinishBtn = false, const bool& bHasHelpIcon = false);

	// Attributes
public:
	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizard)
	BOOL OnInitDialog() override;

	//}}AFX_VIRTUAL

	// Implementation
public:
	~CWizard() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizard)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
