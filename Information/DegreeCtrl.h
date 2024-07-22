#pragma once

// DegreeCtrl.h : header file

#include "Resource.h"

#include "../Public/Global.h"

/////////////////////////////////////////////////////////////////////////////
// CDegreeCtrl dialog

class __declspec(dllexport) CDegreeCtrl : public CDialog
{
public:
	CDegreeCtrl(CWnd* pParent, CPointF& point);

	// Dialog Data
	//{{AFX_DATA(CDegreeCtrl)
	enum
	{
		IDD = IDD_DEGREE
	};

	double m_x;
	double m_y;
	//}}AFX_DATA

	CPointF& m_point;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDegreeCtrl)
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDegreeCtrl)
	// NOTE: the ClassWizard will add member functions here
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
