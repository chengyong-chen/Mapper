#pragma once


#include "Resource.h"

class CSelectDlg : public CDialog
{
// Construction
public:
	CSelectDlg(CWnd* pParent = nullptr);  
	CSelectDlg(CWnd* pParent,CString strTable);
   virtual ~CSelectDlg();

// Dialog Data
	//{{AFX_DATA(CSelectDlg)
	enum { IDD = IDD_SELECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	struct Kind
	{
		DWORD   GB;
		DWORD   Code;
		CString Name;
		BOOL    inPat;
		BOOL    inAat;
		bool    Check;
	};
	typedef CList<Kind, Kind> KIND_LIST;
	
	KIND_LIST m_Kinds;
    
public:
	CButton* m_pButtons;

	static CString m_strDb; 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



