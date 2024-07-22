#pragma once

#include <afxframewndex.h>

#include "gfx.h"

class __declspec(dllexport) CTabFrame : public CFrameWndEx
{
	DECLARE_GWDYNCREATE(CTabFrame)

public:
	CTabFrame(CObject& mainview);

	// Attributes
public:
	CView& m_mainview;

	// Operations
public:
	CMFCToolBar m_wndToolBar;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFrame)
protected:
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CTabFrame() override;

	// Generated message map functions
protected:
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	//{{AFX_MSG(CTabFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
