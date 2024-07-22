#pragma once

#include "Resource.h"

class CSpot;
class CLibrary;

class __declspec(dllexport) CSpotCtrl : public CDialog
{
public:
	CSpotCtrl(CWnd* pParent, bool bComplex, CLibrary& library, CSpot* pSpot);

	~CSpotCtrl() override;

	// Dialog Data
	//{{AFX_DATA(CSpotCtrl)
	enum
	{
		IDD = IDD_SPOTCTRL
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
	CLibrary& m_library;
	bool m_bComplex;
	CImageList m_imagelist;

public:
	CSpot* d_pSpot;
	CDialog* d_pSpotdlg;

	void Reset(CSpot* pspot);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotCtrl)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotCtrl)
	void OnOK() override;
	void OnCancel() override;
	afx_msg void OnTypeSelchange();
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
