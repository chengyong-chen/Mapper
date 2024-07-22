#pragma once

// ExportDIBDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CExportDIBDlg dialog

class __declspec(dllexport) CExportDIBDlg : public CDialog
{
public:
	CExportDIBDlg(CWnd* pParent = nullptr, double fScale = 1.0f);

	// Dialog Data
	//{{AFX_DATA(CExportDIBDlg)
	enum
	{
		IDD = IDD_EXPORTDIB
	};

	// NOTE: the ClassWizard will add data members here
	double m_fScale;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDIBDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDIBDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
