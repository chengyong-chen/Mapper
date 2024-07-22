#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTopoTableDlg dialog

class __declspec(dllexport) CTopoTableDlg : public CDialog
{
public:
	CTopoTableDlg(CWnd* pParent = nullptr);
	CTopoTableDlg(CWnd* pParent, const CString& tableEdge, const CString& tableNode, const CString& tablePolyData, const CString& tablePolyAttribute, const CString& strFilter);

	// Dialog Data
	//{{AFX_DATA(CTopoTableDlg)
	enum
	{
		IDD = IDD_TOPOTABLE
	};

	CString m_tableEdge;
	CString m_tableNode;
	CString m_tablePolyData;
	CString m_tablePolyAttribute;
	CString m_strFilter;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoTableDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTopoTableDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
