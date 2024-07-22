#pragma once

#include "Resource.h"

class CGeocentric;

/////////////////////////////////////////////////////////////////////////////
// CParameterCtrl dialog

class __declspec(dllexport) CParameterCtrl : public CDialog
{
public:
	CParameterCtrl(CWnd* pParent, CGeocentric*& pGeocentric);

	// Dialog Data
	//{{AFX_DATA(CParameterCtrl)
	enum { IDD = IDD_PARAMETER };
	double m_lng0;
	double m_lat0;
	double m_lat1;
	double m_lat2;
	double m_lats;
	double m_lngc;
	double m_lng1;
	double m_lng2;
	double m_k0;
	BOOL m_south;
	//}}AFX_DATA
public:
	CGeocentric*& m_pGeocentric;
	bool m_bModifyToPopup;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterCtrl)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	void PreSubclassWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParameterCtrl)
	// NOTE: the ClassWizard will add member functions here
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
