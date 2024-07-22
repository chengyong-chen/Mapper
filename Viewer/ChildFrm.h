#pragma once

#include "../Atlas/Ruler.h"
#include "../Hyperlink/Go.h"

class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

	// Attributes
public:
	CRuler m_ruler;
	CGo m_Go;
public:
	bool m_bMainChild;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
public:
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
protected:
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) override;
	virtual CWnd* CreateView(CCreateContext* pContext, UINT nId);
	//}}AFX_VIRTUAL

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
	afx_msg void OnGoForward();
	afx_msg void OnGoBack();
	afx_msg void OnGoHome();
	afx_msg void OnPaint();
	afx_msg void OnViewRuler();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LONG OnIsMainChild(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMapRoad(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnOpenDocumentFile(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnAddHistory(UINT WPARAM, LONG LPARAM);
};
