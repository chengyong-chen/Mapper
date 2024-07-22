#pragma once
#include "resource.h"

class CStatDlg : public CDialog
{
	public:
	CStatDlg(CWnd* pParent, CString strField, CString strCount, CString strMax, CString strMin, CString strAvg, CString strSum);

	// Dialog Data
	//{{AFX_DATA(CStatDlg)
	enum
	{
		IDD = IDD_STAT
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	private:
	CString m_strField;
	CString m_strCount;
	CString m_strMax;
	CString m_strMin;
	CString m_strAvg;
	CString m_strSum;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CStatDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
