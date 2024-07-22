#pragma once

#include "Resource.h"

// CLicenceDlg dialog

class __declspec(dllexport) CLicenceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenceDlg)

public:
	CLicenceDlg(CWnd* pParent = nullptr);

	~CLicenceDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_LICENCE
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickTableDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static void Display();
};
