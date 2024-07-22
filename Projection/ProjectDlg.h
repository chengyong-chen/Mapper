#pragma once

#include "Resource.h"
class CProjection;
class CParameterCtrl;

class __declspec(dllexport) CProjectDlg : public CDialog
{
public:
	CProjectDlg(CWnd* pParent=nullptr, CProjection* pProjection=nullptr, bool action=false);
	~CProjectDlg() override;

	// Dialog Data
	//{{AFX_DATA(CProjectDlg)
	enum	{ IDD=IDD_PROJECT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:
	bool m_bNeedAction;
public:
	CProjection* d_pProjection = nullptr;
	CParameterCtrl* d_pParameterCtrl;
	BOOL m_bInterpolate=TRUE;
	void Reset(CProjection* pProjection);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectDlg)
	void OnOK() override;
	void OnCancel() override;
	afx_msg void OnTypeSelchange();
	afx_msg void OnGCSSelchange();
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
