



#pragma once

// DlgStatics.h : header file



/////////////////////////////////////////////////////////////////////////////
// CDlgStatics dialog

class CDlgStatics : public CDialog
{
// Construction
public:
	CDlgStatics(CWnd* pParent = nullptr);  

// Dialog Data
	//{{AFX_DATA(CDlgStatics)
	enum { IDD = IDD_DIALOG_STATICS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStatics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStatics)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





