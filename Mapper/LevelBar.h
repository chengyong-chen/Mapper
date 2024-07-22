#pragma once

#include "Resource.h"

class CLevelBar : public CDialog
{
public:
	CLevelBar(CWnd* pParent = nullptr);

	~CLevelBar() override;

	// Dialog Data
	//{{AFX_DATA(CLevelBar)
	enum
	{
		IDD = IDD_LEVELBAR
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	float m_fLevel;
	float m_minLevel;
	float m_maxLevel;
public:
	void SetRange(float fMin, float fMax);
	void SetLevel(float fLevel);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLevelBar)
	BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLevelBar)
	afx_msg void OnOK() override;
	afx_msg void OnExcute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
