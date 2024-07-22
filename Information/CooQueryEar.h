#pragma once
#include "resource.h"

#include "../Public/Global.h"

/////////////////////////////////////////////////////////////////////////////
// CCooQueryEar dialog

class CCooQueryEar : public CDialog
{
	DECLARE_DYNCREATE(CCooQueryEar)

public:
	CCooQueryEar();

	// Dialog Data
	//{{AFX_DATA(CCooQueryEar)
	enum
	{
		IDD = IDD_COOQUERY
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CDialog* d_pCoorCtrl;
	CPointF m_geoPoint;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCooQueryEar)
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCooQueryEar)
	BOOL OnInitDialog() override;
	afx_msg void OnSelchangeTypecombo();
	afx_msg void OnDestroy();
	afx_msg void OnLocate();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
