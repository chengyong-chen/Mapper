#pragma once

#include "DynamicPane.h"

#include "../Framework/MSplitterWnd.h"
#include "../Dataview/ViewMonitor.h"


class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame() noexcept;

	// Attributes
public:
	CRuler m_ruler;
	CDynamicPane m_DynamicPane;
	MSplitterWnd m_wndSplitter;

	// Operations
public:
	void ResetRuler(CViewMonitor& viewMonitor);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	virtual CWnd* CreateView(CCreateContext* pContext, UINT nId);
	virtual void AdjustClientArea();
	//}}AFX_VIRTUAL
	BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) override;
	void AdjustDockingLayout(HDWP hdwp = nullptr) override;
	BOOL DestroyWindow() override;
	// Implementation
public:
	~CChildFrame() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnViewRuler();
	afx_msg void OnMapAttForm();
	afx_msg void OnUpdateViewRuler(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttForm(CCmdUI* pCmdUI);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LONG OnSetRuler(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnIsMainChild(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
