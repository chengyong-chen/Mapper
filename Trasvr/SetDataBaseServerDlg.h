



#pragma once

// SetDataBaseServerDlg.h : header file


/////////////////////////////////////////////////////////////////////////////
// CSetDataBaseServerDlg dialog

class CSetDataBaseServerDlg : public CDialog
{
// Construction
public:
	CSetDataBaseServerDlg(CWnd* pParent = nullptr);  

// Dialog Data
	//{{AFX_DATA(CSetDataBaseServerDlg)
	enum { IDD = IDD_SET_DATABASE_SERVER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDataBaseServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDataBaseServerDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





