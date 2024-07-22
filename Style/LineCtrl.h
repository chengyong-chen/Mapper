#pragma once

#include "resource.h"

class CLine;
class CLibrary;

class __declspec(dllexport) CLineCtrl : public CDialog
{
public:
	CLineCtrl(CWnd* pParent, bool bComplex, CLibrary& library);

	~CLineCtrl() override;

	// Dialog Data
	//{{AFX_DATA(CLineCtrl)
	enum
	{
		IDD = IDD_LINECTRL
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	CLibrary& m_library;
	bool m_bComplex;
	CImageList m_imagelist;

public:
	CLine* d_pLine;
	CDialog* d_pLinedlg;

	void Reset(CLine* pline);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineCtrl)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLineCtrl)
	void OnOK() override;
	void OnCancel() override;
	afx_msg void OnTypeSelchange();
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
