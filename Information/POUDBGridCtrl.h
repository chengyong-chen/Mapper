#pragma once

#include "DBGridCtrl.h"

class CPointF;

class __declspec(dllexport) CPOUDBGridCtrl : public CDBGridCtrl
{
	DECLARE_DYNCREATE(CPOUDBGridCtrl);
	public:
	CPOUDBGridCtrl();

	public:
	static void PolyFilter(CPointF* pPoints, int nAnchors);
	void PolyFilter(CPoint* pPoints, int nAnchors);

	void MarkField();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOUDBGridCtrl)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
	protected:
	// Generated message map functions
	//{{AFX_MSG(CPOUDBGridCtrl)
	afx_msg void OnGeometry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
