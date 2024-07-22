#pragma once

class CFill;
class CLibrary;

#include "Resource.h"

class __declspec(dllexport) CFillCtrl : public CDialog
{
public:
	CFillCtrl(CWnd* pParent, bool bComplex, CLibrary& library);

	~CFillCtrl() override;

	// Dialog Data
	//{{AFX_DATA(CFillCtrl)
	enum
	{
		IDD = IDD_FILLCTRL
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	CLibrary& m_library;
	bool m_bComplex;
	CImageList m_imagelist;

public:
	CFill* d_pFill;
	CDialog* d_pFilldlg;

	void Reset(CFill* pFill);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillCtrl)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFillCtrl)
	void OnOK() override;
	void OnCancel() override;
	afx_msg void OnPaint();
	afx_msg void OnTypeSelchange();
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LONG OnRedrawPreview(UINT WPARAM, LONG LPARAM);
};
