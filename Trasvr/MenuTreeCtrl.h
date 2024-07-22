// MapTreeCtrl.h: interface for the CMenuTreeCtrl class.

//////////////////////////////////////////////////////////////////////





#pragma once


#include "global.h"

#include <list>
using namespace std;

class CChildView;

class CMenuTreeCtrl : public CTreeCtrl  
{
	DECLARE_DYNCREATE(CMenuTreeCtrl)
public:
	CMenuTreeCtrl();
	virtual ~CMenuTreeCtrl();

public:
	void ActiveItem(CString strItem);
	HTREEITEM GetDownItem(HTREEITEM hitem);

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuTreeCtrl)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMenuTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	DECLARE_MESSAGE_MAP()

public:
	CImageList  m_imgList;
	
protected:
	HCURSOR hHand;
public:
	CChildView* m_pChildView;
};


