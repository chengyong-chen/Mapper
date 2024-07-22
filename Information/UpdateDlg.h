



#pragma once


#include "Cursor.h"

class CUpdateDlg : public CDialog
{
// Construction
public:
	CUpdateDlg(CWnd* pParent = nullptr,COL* pCol=nullptr, SWORD nCol=0);  

// Dialog Data
	//{{AFX_DATA(CUpdateDlg)
	enum { IDD = IDD_UPDATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	COL*  m_pCol;
	SWORD m_nCol;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdateDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





