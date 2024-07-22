#pragma once

// PointCtrl.h : header file

/////////////////////////////////////////////////////////////////////////////
// CPointCtrl dialog

#include "Resource.h"

class __declspec(dllexport) CPointCtrl : public CDialog
{
public:
	CPointCtrl(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPointCtrl)
	enum
	{
		IDD = IDD_POINT
	};

	double m_X;
	double m_Y;
	//}}AFX_DATA

	CView* m_pView;
	CPoint m_docPoint;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointCtrl)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPointCtrl)
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
