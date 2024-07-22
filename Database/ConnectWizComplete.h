#pragma once

#include "Resource.h"

class CODBCDatabase;
class CConnectWizComplete : public CPropertyPage
{
public:
	CConnectWizComplete(CODBCDatabase& ConnectInfo);

	~CConnectWizComplete() override;

	// Dialog Data
	//{{AFX_DATA(CConnectWizComplete)
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_COMPLETE
	};

	// NOTE - ClassWizard will add data members here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectWizComplete)
public:
	BOOL OnSetActive() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectWizComplete)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
