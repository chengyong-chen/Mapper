#pragma once
#include "resource.h"
#include "../Coding/Coder.h"

/////////////////////////////////////////////////////////////////////////////
// CCmdParamDlg dialog

class CCmdParamDlg : public CDialog
{
	public:
	CCmdParamDlg(CWnd* pParent = nullptr, CCoder::SParameter* pParameter = nullptr);

	protected:
	// Dialog Data
	//{{AFX_DATA(CCmdParamDlg)
	enum
	{
		IDD = IDD_CMDPARAM
	};

	CString m_strEdit0;
	CString m_strEdit1;
	CString m_strEdit2;
	CString m_strEdit3;
	CString m_strEdit4;
	CString m_strStatic0;
	CString m_strStatic1;
	CString m_strStatic2;
	CString m_strStatic3;
	CString m_strStatic4;
	COleDateTime m_dateTime1;
	COleDateTime m_dateTime2;
	COleDateTime m_dateTime3;
	COleDateTime m_dateTime4;
	//}}AFX_DATA

	public:
	CCoder::SParameter* m_pParameter;

	CString m_strDescription;
	CString m_strTitle;

	public:
	CStringArray m_saParams; // cmd parameters array
	CStringArray m_saTypes; // parameters types
	CStringArray m_saValues; // parameters values

	protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdParamDlg)
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CCmdParamDlg)
	afx_msg void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
