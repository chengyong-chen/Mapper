#pragma once
#include "resource.h"

class CSearchPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchPage)

	public:
	CSearchPage();

	~CSearchPage() override;

	// Dialog Data
	//{{AFX_DATA(CSearchPage)
	enum
	{
		IDD = IDD_ROUTE
	};

	BOOL m_bTrack;
	BOOL m_bAutoin;
	BOOL m_bDrawTrace;
	BOOL m_bSaveTrace;
	BOOL m_bRotateMap;
	// NOTE - ClassWizard will add data members here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchPage)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
