// MyPropertyPage1.h : header file


#pragma once

/////////////////////////////////////////////////////////////////////////////
// COptionPage1 dialog

class COptionPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionPage1)

// Construction
public:
	COptionPage1();
	virtual ~COptionPage1();

// Dialog Data
	//{{AFX_DATA(COptionPage1)
	enum { IDD = IDD_PROPPAGE1 };
	int		m_nDay;
	CString	m_strPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionPage1)
	public:
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionPage1)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// COptionPage2 dialog

class COptionPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionPage2)

// Construction
public:
	COptionPage2();
	virtual ~COptionPage2();

// Dialog Data
	//{{AFX_DATA(COptionPage2)
	enum { IDD = IDD_PROPPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionPage2)
	public:
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionPage2)
	afx_msg void OnCheckBroadcast();
	afx_msg void OnCheckFeemanage();
	afx_msg void OnCheckGroupmanage();
	afx_msg void OnCheckLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};