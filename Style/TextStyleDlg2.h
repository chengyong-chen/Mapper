#pragma once
#include "resource.h"


struct TextOutline;
class CTextStyleDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(CTextStyleDlg2)

public:
	CTextStyleDlg2(CWnd* pParent, TextOutline& outline);

	~CTextStyleDlg2() override;

	// Dialog Data
	enum
	{
		IDD = IDD_TEXTSTYLE2
	};

public:
	TextOutline& m_outline;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnColorBnClicked();
	afx_msg void OnContextChanged();
public:
};
