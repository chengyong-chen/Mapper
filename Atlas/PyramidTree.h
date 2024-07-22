#pragma once

#include "Tree.h"
class CPyramid;

class __declspec(dllexport) CPyramidTree : public CTree1
{
public:
	CPyramidTree();

	// Attributes
public:
	CPyramid* m_pPyramid;
public:
	void BuildTree(CPyramid* pPyramid);

protected:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPyramidTree)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CPyramidTree() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CPyramidTree)
	afx_msg void OnAddAfter();
	afx_msg void OnAddBefore();
	afx_msg void OnAddIn();
	afx_msg void OnAttribute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
