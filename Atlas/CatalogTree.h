#pragma once

#include "Tree.h"

class CCatalog;
class CPage;

class __declspec(dllexport) CCatalogTree : public CTree1
{
public:
	CCatalogTree();

	// Attributes
public:
	CCatalog* m_pCatalog;
public:
	void BuildTree(CCatalog* pCatalog);
	CPage* SelectMap(CString strMap);

protected:
	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCatalogTree)
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CCatalogTree() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCatalogTree)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddAfter();
	afx_msg void OnAddBefore();
	afx_msg void OnAddIn();
	afx_msg void OnAttribute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
