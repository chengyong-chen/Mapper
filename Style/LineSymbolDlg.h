#pragma once
#include "resource.h"
#include "Midtable.hpp"
#include "SymbolMid.hpp"
#include "SymbolMidDlg.h"

class CLineSymbol;

class CLineSymbolDlg : public TSymbolMidDlg<CSymbolLine, CLineSymbol>
{
public:
	CLineSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolLine>>& tagtable, CLineSymbol* pLine);

	// Dialog Data
	//{{AFX_DATA(CLineSymbolDlg)
	enum
	{
		IDD = IDD_LINESYMBOL
	};

	//}}AFX_DATA

	void DrawPreview() override;
	BOOL Reset();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineSymbolDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLineSymbolDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
