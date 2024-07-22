#pragma once

#include "Resource.h"
class CProjection;

class __declspec(dllexport) CRotateDlg : public CDialog
{
public:
	CRotateDlg(CWnd* pParent=nullptr);
	~CRotateDlg() override;

	// Dialog Data
	//{{AFX_DATA(CRotateDlg)
	enum	{ IDD=IDD_ROTATE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	float m_angle;
	float m_lngDelta;
	float m_latDelta;

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRotateDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
