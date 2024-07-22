#pragma once

#include "../Framework/GSplitterWnd.h"
#include "../Framework/MFCToolBarEx.h"

#include "../Tool/DrawToolWnd.h"

#define	ID_QUERYCOUNT		    1
#define	ID_PICKANCHORXY			2
#define	ID_MOUSECOORDINATE	    3

class CMainFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)
protected: // create from serialization only
	CMainFrame();

	// Attributes
public:
	CDrawToolWnd m_wndDrawTool;
	CMFCStatusBar m_wndStatusBar;

private:
	CMFCMenuBar m_wndMenuBar;
	CMFCToolBarEx m_toolbarMain;
	CMFCToolBarEx m_toolbarGeomAlign;
	CMFCToolBarEx m_toolbarAxisAlign;
	CMFCToolBarEx m_toolbarMagnify;

public:
	GSplitterWnd m_wndSplitter;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	BOOL DestroyWindow() override;
protected:
	BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) override;
	virtual CWnd* CreateView(CCreateContext* pContext, UINT nId);
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CMainFrame() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFilePreferences();
	afx_msg void OnResetwindowLayout();
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};
