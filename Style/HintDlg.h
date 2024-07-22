#pragma once

#include "Resource.h"
#include "FontComBox.h"

class CColor;
class CHint;
class CFontDesc;

class __declspec(dllexport) CHintDlg : public CDialog
{
public:
	CHintDlg(CWnd* pParent, CHint& hint);

	~CHintDlg() override;

	// Dialog Data
	//{{AFX_DATA(CHintDlg)
	enum
	{
		IDD = IDD_HINT
	};

	CFontComboBox m_fontcombo;
	//}}AFX_DATA

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHintDlg)
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_VIRTUAL

	CImageList m_imagelist;
	CHint& m_hint;
	CDialog* m_pTextStyleDlgx;

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHintDlg)
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDCColor();
	afx_msg void OnChangeFont();
	afx_msg void OnChangeStyle();
	afx_msg void OnNotified(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextChanged();
};
