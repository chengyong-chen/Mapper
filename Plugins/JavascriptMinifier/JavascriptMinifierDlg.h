#pragma once
#include "resource.h"
#include "afxwin.h"

// CJavascriptMinifierDlg dialog

class CJavascriptMinifierDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJavascriptMinifierDlg)

public:
	CJavascriptMinifierDlg(CWnd* pParent = nullptr);
	virtual ~CJavascriptMinifierDlg();

	// Dialog Data
	enum
	{
		IDD = IDD_JAVASCRIPTMINIFIER
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;

public:
	afx_msg void OnRadioButtonClicked();
	afx_msg void OnLoadpreserves();
	DECLARE_MESSAGE_MAP()
};
