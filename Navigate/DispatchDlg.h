#pragma once

#include "msflexgrid.h"
#include "resource.h"

class __declspec(dllexport) CDispatch1Dlg : public CDialog
{
	public:
	CMSFlexGrid* m_pGrid;

	public:
	CDispatch1Dlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CDispatch1Dlg)
	enum
	{
		IDD = IDD_DISPATCH
	};

	CMSFlexGrid m_VehGrid;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispatch1Dlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CDispatch1Dlg)
	BOOL OnInitDialog() override;
	afx_msg void OnOk();
	afx_msg void OnDialup();
	afx_msg void OnHangup();
	afx_msg void OnRowColChangeMsflexgrid();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
