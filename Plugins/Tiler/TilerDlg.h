#pragma once
#include "resource.h"
#include "afxwin.h"

// CTilerDlg dialog

class CTilerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTilerDlg)

	CTilerDlg(CWnd* pParent = nullptr);
	virtual ~CTilerDlg();

	// Dialog Data
	enum
	{
		IDD = IDD_TILER
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;

	afx_msg void OnEnChangeImageFile();
	DECLARE_MESSAGE_MAP()
public:
	CString m_imagefile;
	CString m_outputpath;
	int m_minLevel;
	int m_maxLevel;
};
