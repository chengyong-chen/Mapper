#pragma once

#include "afxwin.h"
#include "Resource.h"

class CClauseSelect;

class __declspec(dllexport) CSQLSelectDlg : public CDialogEx
{
public:
	CSQLSelectDlg(CWnd* pParent, CClauseSelect& clause);

	~CSQLSelectDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_SQLSELECT
	};

public:
	CClauseSelect& m_clause;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
