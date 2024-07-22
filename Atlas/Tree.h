#pragma once

#include "TreeNode.h"

class CTreeNode1;

class __declspec(dllexport) CTree1 : public CTreeCtrl
{
public:
	CTree1();

public:

	// Attributes
public:	
	CImageList* m_pDragImage;

	BOOL m_bDragging;
	HTREEITEM m_hitemDrag;
	HTREEITEM m_hitemDrop;
	// Operations
public:
	CTreeNodeList* m_pNodelist;

public:
	void Select(CTreeNode1* pNode);
	void BuildTree(CTreeNodeList* pNodelist);
	DWORD GetMaxItemId() const;

private:
	BOOL TransferItem(HTREEITEM hitem, HTREEITEM hNewParent);
	BOOL IsUperOf(HTREEITEM hitemSuspectedup, HTREEITEM hitemdown) const;
	BOOL IsChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent) const;
	HTREEITEM GetUperItem(HTREEITEM hitem) const;

public:
	HTREEITEM GetDownItem(HTREEITEM hitem) const;
	HTREEITEM GetItemById(WORD wItemID) const;
	HTREEITEM GetItemByName(CString strName) const;

public:
	bool InsertAfter(CTreeNode1* newNode);
	bool InsertBefore(CTreeNode1* newNode);
	bool InsertIn(CTreeNode1* newNode);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTree1)
public:
	BOOL DestroyWindow() override;
	virtual CImageList* CreateDragImage(HTREEITEM hItem);
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CTree1)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDelete();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
