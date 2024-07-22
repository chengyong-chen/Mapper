#pragma once

#include "Resource.h"
class CGeocentric;
class CParameterCtrl;

class __declspec(dllexport) CParameterDlg : public CDialog
{
public:
	CParameterDlg(CWnd* pParent, CGeocentric* pGeocentric);
	~CParameterDlg() override;

	// Dialog Data
	//{{AFX_DATA(CParameterDlg)
	enum { IDD = IDD_HOLDER };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CGeocentric* m_pGeocentric;
	CParameterCtrl* d_pParameterCtrl;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParameterDlg)
	void OnOK() override;
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
