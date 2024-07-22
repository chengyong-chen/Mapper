#pragma once

#include "..\Framework\MSplitterWnd.h"

class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:
	CRuler  m_ruler;
	MSplitterWnd  m_wndSplitter;	

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) override;
	virtual CWnd* CreateView(CCreateContext* pContext, UINT nId);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
