#pragma once
#include "resource.h"
#include "SymbolMid.hpp"
#include "Midtable.hpp"
#include "SymbolMidDlg.h"

class CSymbolFill;
class CFillSymbol;

class CFillSymbolDlg : public TSymbolMidDlg<CSymbolFill, CFillSymbol>
{
public:
	CFillSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolFill>>& tagtable, CFillSymbol* pFill);

	// Dialog Data
	//{{AFX_DATA(CFillSymbolDlg)
	enum
	{
		IDD = IDD_FILLSYMBOL
	};

	short m_nAngle;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillSymbolDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFillSymbolDlg)
	afx_msg void OnChangeAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
