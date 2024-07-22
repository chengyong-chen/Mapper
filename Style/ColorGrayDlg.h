#pragma once
class CColorGray;

class CColorGrayDlg : public CDialog
{
public:
	CColorGrayDlg(CWnd* pParent = nullptr, CColorGray* pcolor = nullptr);

	// Dialog Data
	//{{AFX_DATA(CColorGrayDlg)
	enum
	{
		IDD = IDD_COLORGRAY
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	CColorGray* d_pColor;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorGrayDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorGrayDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditT();
	afx_msg void OnChangeEditA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
