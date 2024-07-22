#pragma once

#include "ListCtrlEx.h"

using namespace std;
#include <list>

template<class T>
class __declspec(dllexport) TFavorListCtrl : public CListCtrlEx
{
public:
	TFavorListCtrl(std::list<T*>& elements);

	// Attributes	
public:
	CDocument* m_pDocument;
	std::list<T*>& m_elements;

protected:
	bool m_bModified;

public:
	BOOL Create(CWnd* parent, bool bEditble);
	void Build();

public:
	T* GetActive();
	void SetActive(T* pT);

	// Operations
public:

	HTREEITEM GetDownItem(HTREEITEM hitem);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TFavorListCtrl)
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(TFavorListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnSetHint();
	afx_msg void OnUpdateSetHint(CCmdUI* pCmdUI);
	afx_msg void OnAllon();
	afx_msg void OnUpdateAllon(CCmdUI* pCmdUI);
	afx_msg void OnAlloff();
	afx_msg void OnUpdateAlloff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

BEGIN_TEMPLATE_MESSAGE_MAP(TFavorListCtrl, T, CListCtrlEx)
	//{{AFX_MSG_MAP(TFavorListCtrl)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MA
	END_MESSAGE_MAP()

#include "FavorListCtrl.cpp"
