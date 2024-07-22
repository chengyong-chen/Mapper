#pragma once

#include <afxcmn.h>
class CLayerTree;
class CLibrary;

class CLayer;
class CODBCDatabase;

class AFX_EXT_CLASS CLayerTreeCtrl : public CTreeCtrl
{
public:
	CLayerTreeCtrl(CDocument& document, const CDatainfo& datainfo, CLayerTree& layertree, Undoredo::CActionStack& actionstack);

public:
	const CDatainfo& m_datainfo;

	// Attributes
public:	
	CImageList* m_pDragImage;
	BOOL m_bMulti;

	HTREEITEM m_hFixedItem;
	BOOL m_bDragging;
	HTREEITEM m_hitemDrag;
	HTREEITEM m_hitemDrop;
	// Operations
public:
	Undoredo::CActionStack& m_actionStack;
	CLayerTree& m_layertree;
	bool m_bShowhidelayer;

	static CString m_strActiveLayerName;

private:
	CDocument& m_document;

public:
	void AddNode(HTREEITEM hitemParent, HTREEITEM hitemAfter, CLayer* pLayer);

	void Select(CLayer* pLayer);
	void SelectFixed();
	void UpdateSelect();
	CLayer* GetSelLayer() const;
	void BuildTree();
	void SwitchState(HTREEITEM hItem, long bState);

private:
	void MoveItem(HTREEITEM hitemToBeMoved, HTREEITEM hitemInsertIn, HTREEITEM hitemInsertAfter);
	BOOL IsUperOf(HTREEITEM hitemSuspectedup, HTREEITEM hitemdown) const;
	BOOL IsChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent) const;
public:
	HTREEITEM GetUperItem(HTREEITEM hitem) const;
	HTREEITEM GetDownItem(HTREEITEM hitem) const;
public:
	HTREEITEM GetItem(DWORD dwData) const;
	HTREEITEM GetItemByName(CString strName) const;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerTreeCtrl)
public:
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	virtual CImageList* CreateDragImage(HTREEITEM hItem);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerTreeCtrl)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateMoveDown(CCmdUI* pCmdUI);
	afx_msg void OnMoveDown();
	afx_msg void OnUpdateMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnMoveUp();
	afx_msg void OnDelete();
	afx_msg void OnNewAfter();
	afx_msg void OnNewBefore();
	afx_msg void OnNewIn();
	afx_msg void OnSetType();
	afx_msg void OnSetStyle();
	afx_msg void OnSetSpot();
	afx_msg void OnAllon();
	afx_msg void OnAlloff();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGEODatabase();
	afx_msg void OnGEODatasource();
	afx_msg void OnATTDatabase();
	afx_msg void OnATTDatasource();
	afx_msg void OnApplyName();
	afx_msg void OnApplyCode();
	afx_msg void OnATTTheme();
	afx_msg void OnSetProperty();
	afx_msg void OnMulti();
	afx_msg void OnSetHint();
	afx_msg void OnToLogical();
	afx_msg void OnSetIf();
	afx_msg void OnRematch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CLayerPane;
};
