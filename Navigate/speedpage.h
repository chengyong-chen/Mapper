#pragma once
#include "resource.h"

class CSpeedPage : public CPropertyPage
{
public:
	CSpeedPage();

	~CSpeedPage() override;

	// Dialog Data
	//{{AFX_DATA(CSpeedPage)
	enum
	{
		IDD = IDD_SPEED
	};

	CSliderCtrl m_SlideCtrl;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	long m_nSlidePos;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeedPage)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpeedPage)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnReleasedSlider(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
