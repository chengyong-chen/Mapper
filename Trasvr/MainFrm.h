#pragma once

#include "ChildView.h"
#include "MenuTreeCtrl.h"
#include "MessageWnd.h"
#include "../Atlas/SizeCtrlBar.h"

#include <map>

using namespace std;

#define DIALOGMAP map<UINT, CDialog*> 

class CSMSThread;

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

protected:
	CChildView m_ChildView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	virtual BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

public:
	CMenuTreeCtrl m_menuTreeCtrl;
	CMessageWnd   m_wndInBarEdit;
	
public:  // control bar embedded members
	CStatusBar m_wndStatusBar;
	CToolBar m_wndToolBar;

	CSizeControlBar m_wndResizableBar1;
	CSizeControlBar m_wndResizableBar2;


// Generated message map functions
protected:
	BOOL ChangeMonitorObject(UINT uCode);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnServerStart();
	afx_msg void OnServerStop();
	afx_msg void OnUpdateServerStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateServerStop(CCmdUI* pCmdUI);
	afx_msg void OnMenuitemChangeRootpass();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};