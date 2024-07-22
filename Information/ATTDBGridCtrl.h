#pragma once

#include "DBGridCtrl.h"

class CPointF;

class __declspec(dllexport) CAATDBGridCtrl : public CDBGridCtrl
{
protected:
	DECLARE_DYNCREATE(CAATDBGridCtrl);
public:
	CAATDBGridCtrl();

public:
	static void PolyFilter(CPointF* pPoints, int nAnchors);
	void PolyFilter(CPoint* pPoints, int nAnchors);

	void MarkField(CString field);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAATDBGridCtrl)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAATDBGridCtrl)
	afx_msg void OnGeomAll();
	afx_msg void OnGeom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
