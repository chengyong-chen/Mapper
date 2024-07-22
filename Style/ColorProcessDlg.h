#pragma once
class CColorProcess;

class CColorProcessDlg : public CDialog
{
public:
	CColorProcessDlg(CWnd* pParent = nullptr, CColorProcess* pcolor = nullptr);

	// Dialog Data
	//{{AFX_DATA(CColorProcessDlg)
	enum
	{
		IDD = IDD_COLORPROCESS
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CColorProcess* d_pColor;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorProcessDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorProcessDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnChangeEditC();
	afx_msg void OnChangeEditM();
	afx_msg void OnChangeEditY();
	afx_msg void OnChangeEditK();
	afx_msg void OnChangeEditA();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
