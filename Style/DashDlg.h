#pragma once

class CDashDlg : public CDialog
{
public:
	CDashDlg(CWnd* pParent, DWORD* pseg);

	// Dialog Data
	//{{AFX_DATA(CDashDlg)
	enum
	{
		IDD = IDD_DASH
	};

	float d_segoff1;
	float d_segoff2;
	float d_segoff3;
	float d_segoff4;
	float d_segon1;
	float d_segon2;
	float d_segon3;
	float d_segon4;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDashDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDashDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
