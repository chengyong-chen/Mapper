#pragma once

#include "Resource.h"

enum ANCHOR : BYTE;

/////////////////////////////////////////////////////////////////////////////
// CTagDlg dialog

class AFX_EXT_CLASS CTagDlg : public CDialog
{
public:
	CTagDlg(CWnd* pParent, ANCHOR& anchor, HALIGN& hAlign, VALIGN& vAlign);

	// Dialog Data
	//{{AFX_DATA(CTagDlg)
	enum
	{
		IDD = IDD_TAG
	};

	//}}AFX_DATA

	ANCHOR& m_anchor;
	HALIGN& m_hAlign;
	VALIGN& m_vAlign;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTagDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
