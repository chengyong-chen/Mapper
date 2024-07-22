#pragma once
#include <afxdialogex.h>


class CDynamicDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDynamicDlg)

public:
	CDynamicDlg(CWnd* pParent = nullptr);   // standard constructor
	~CDynamicDlg() override;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DYNAMIC };
#endif

protected:
	void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()
};
