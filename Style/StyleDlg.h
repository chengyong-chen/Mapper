#pragma once

#include "FillCtrl.h"
#include "LineCtrl.h"

class CLibrary;
class CLine;
class CFill;

class __declspec(dllexport) CStyleDlg : public CDialog
{
public:
	CStyleDlg(CWnd* pParent, bool bComplex, CLibrary& library, CLine* pline, CFill* pfill);

	// Dialog Data
	//{{AFX_DATA(CStyleDlg)
	enum
	{
		IDD = IDD_STYLE
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CFill* d_pFill;
	CLine* d_pLine;

	CFillCtrl m_FillCtrl;
	CLineCtrl m_LineCtrl;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStyleDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
