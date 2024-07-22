#pragma once

// SexageCtrl.h : header file

#include "Resource.h"

#include "../Public/Global.h"

/////////////////////////////////////////////////////////////////////////////
// CSexageCtrl dialog

class __declspec(dllexport) CSexageCtrl : public CDialog
{
	public:
	CSexageCtrl(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CSexageCtrl)
	enum
	{
		IDD = IDD_SEXAGE
	};

	int m_LatD;
	int m_LatM;
	float m_LatS;
	int m_LonD;
	int m_LonM;
	float m_LonS;
	//}}AFX_DATA

	CPointF m_geoPoint;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSexageCtrl)
	protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CSexageCtrl)
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
