#pragma once
class CColorSpot;

class CColorSpotDlg : public CDialog
{
public:
	CColorSpotDlg(CWnd* pParent = nullptr, CColorSpot* pcolor = nullptr);

	// Dialog Data
	//{{AFX_DATA(CColorSpotDlg)
	enum
	{
		IDD = IDD_COLORSPOT
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CColorSpot* d_pColor;
	void OnSetRGB(COLORREF rgb);
	bool m_bPassive;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSpotDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorSpotDlg)
	afx_msg void OnChangeEditB();
	afx_msg void OnChangeEditG();
	afx_msg void OnChangeEditR();
	afx_msg void OnChangeEditA();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};