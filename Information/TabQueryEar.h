#include "msdgridctrl.h"
#include "rdc.h"

#ifndef _TABQUERYPAGE_H_
#define _TABQUERYPAGE_H_

#include "Resource.h"

class __declspec(dllexport) CTabQueryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTabQueryPage)

	public:
	CTabQueryPage();

	// Dialog Data
	//{{AFX_DATA(CTabQueryPage)
	enum
	{
		IDD = IDD_TABLETQUERY
	};

	CMsDgridCtrl m_DBGridCtrl;
	CRdc m_Rdc;
	//}}AFX_DATA

	public:
	void RefillGrid(CString strStreet, CString strtablet);

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabQueryPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:
	// Generated message map functions
	//{{AFX_MSG(CTabQueryPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL OnInitDialog() override;
	afx_msg void OnChangeStreet();
	afx_msg void OnChangeTablet();
	afx_msg void OnLocate();
	afx_msg void OnDblClickGrid();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
