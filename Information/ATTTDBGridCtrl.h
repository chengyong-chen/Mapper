#pragma once

#include "TDBGridCtrl.h"

class __declspec(dllexport) CATTTDBGridCtrl : public CTDBGridCtrl
{
public:
	CATTTDBGridCtrl();

public:
	void MarkField() override;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATTTDBGridCtrl)
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, CCreateContext* pContext = nullptr) override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CATTTDBGridCtrl)
	afx_msg void OnInsert();
	afx_msg void OnUpdateInsert(CCmdUI* pCmdUI);
	afx_msg void OnQuery();
	afx_msg void OnUpdateQuery(CCmdUI* pCmdUI);
	afx_msg void OnGeom();
	afx_msg void OnUpdateGeom(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
