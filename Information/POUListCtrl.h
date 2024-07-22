#pragma once

#include "POU.h"
#include "../Public/FavorListCtrl.h"

class CLibrary;
class CPOUList;

class __declspec(dllexport) CPOUListCtrl : public TFavorListCtrl<CPOU>
{
public:
	CPOUListCtrl(CPOUList& poulist);

public:
	CPOUList& m_poulist;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOUListCtrl)
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CPOUListCtrl)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnNew();
	afx_msg void OnSetSpot();
	afx_msg void OnSetWhere();
	afx_msg void SetTag();
	afx_msg void OnUpdateNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetWhere(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetTag(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSet(CCmdUI* pCmdUI);
	afx_msg void OnDatabase();
	afx_msg void OnDatasource();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
