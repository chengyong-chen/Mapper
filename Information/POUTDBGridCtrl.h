#pragma once

#include "TDBGridCtrl.h"

class __declspec(dllexport) CPOUTDBGridCtrl : public CTDBGridCtrl
{
public:
	CPOUTDBGridCtrl();

public:
	virtual DWORD Position(CView* pView, double WPARAM, double LPARAM);
	void MarkField() override;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOUTDBGridCtrl)
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, CCreateContext* pContext = nullptr) override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPOUTDBGridCtrl)
	afx_msg void OnInsert();
	afx_msg void OnQuery();
	afx_msg void OnGeom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
