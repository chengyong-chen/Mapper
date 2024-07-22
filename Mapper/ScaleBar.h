#pragma once

#include "Resource.h"

class CScaleBar : public CDialog
{
public:
	CScaleBar(CWnd* pParent = nullptr);

	~CScaleBar() override;

	// Dialog Data
	//{{AFX_DATA(CScaleBar)
	enum
	{
		IDD = IDD_SCALEBAR
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	float m_fScale;
	float m_minScale;
	float m_maxScale;
public:
	void SetRange(float fMin, float fMax);
	void SetScale(float fScale);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleBar)
	BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleBar)
	afx_msg void OnOK() override;
	afx_msg void OnExcute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
