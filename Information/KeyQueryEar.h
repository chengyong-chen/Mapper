#pragma once

#include "Resource.h"

class __declspec(dllexport) CKeyQueryEar : public CDialog
{
	DECLARE_DYNCREATE(CKeyQueryEar)

public:
	CKeyQueryEar();

	// Dialog Data
	//{{AFX_DATA(CKeyQueryEar)
	enum
	{
		IDD = IDD_KEYQUERY
	};

	CListCtrl m_listCtrl;
	//}}AFX_DATA

	void AddMapCombo(CMap<WORD, WORD&, CString, CString&>& mapIdMap) const;
	void AddClassCombo1(CMap<DWORD, DWORD&, CString, CString&>& mapIdClass) const;

public:
	CString m_strSQL;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyQueryEar)
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	static void DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& wData);
	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyQueryEar)
	afx_msg void OnOK() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuery();
	BOOL OnInitDialog() override;
	afx_msg void OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClass1Selchange();
	//}}AFX_MSG
	afx_msg LONG OnSetCount(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnEditupdateMapcombo();
};
