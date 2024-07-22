#pragma once
#include "resource.h"

class TextFrame;
class CTextStyleDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(CTextStyleDlg1)

public:
	CTextStyleDlg1(CWnd* pParent, TextFrame& frame);

	~CTextStyleDlg1() override;

	// Dialog Data
	enum
	{
		IDD = IDD_TEXTSTYLE1
	};

public:
	TextFrame& m_frame;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnColorBnClicked();
	afx_msg void OnCheckBnClicked();
};