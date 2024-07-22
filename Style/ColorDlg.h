#pragma once

#include "Resource.h"

class CColor;

class __declspec(dllexport) CColorDlg : public CDialog
{
public:
	CColorDlg(CWnd* pParent = nullptr);
	CColorDlg(CWnd* pParent, CColor* pcolor, COLORREF defaultRGB);

	~CColorDlg() override;
private:
	Gdiplus::Color d_oldcolor;
public:
	CColor* d_pColor;
private:
	CDialog* d_pDlg;

public:
	void DrawOld() const;
	void Reset(CColor* pColor);

	// Dialog Data
	//{{AFX_DATA(CColorDlg)
	enum
	{
		IDD = IDD_COLOR
	};

	CString m_strName;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnTypeSelchange();
	afx_msg void OnPaint();
	void OnOK() override;
	afx_msg void OnDestroy();
	void OnCancel() override;
	//}}AFX_MSG
	afx_msg LONG OnDrawNew(UINT WPARAM, LONG LPARAM);

	DECLARE_MESSAGE_MAP()
};
