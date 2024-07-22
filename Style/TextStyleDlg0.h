#pragma once
#include "resource.h"

#include "TextStyle.h"

class CTextStyleDlg0 : public CDialog
{
	DECLARE_DYNAMIC(CTextStyleDlg0)

public:
	CTextStyleDlg0(CWnd* pParent, TextDeform& deform);

	~CTextStyleDlg0() override;

	// Dialog Data
	enum
	{
		IDD = IDD_TEXTSTYLE0
	};

public:
	TextDeform& m_deform;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
