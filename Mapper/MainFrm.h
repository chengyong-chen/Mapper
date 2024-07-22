#pragma once
class CDrawToolWnd;
class CTablePane;
class CAtlas;
class CLayerPane;
class CThemePane;
class CAtlasPane;
class CTopoPane;
class CQueryPane;

class CPOUPane;
class CNavinfoPane;
class CVehiclePane;
class CTrafficPane;

#include "StatusBarEx.h"
#include "../Tool/DrawToolWnd.h"
#include "../Layer/LayerPane.h"
#include "../Atlas/AtlasPane.h"
#include "../Logger/OutputPane.h"

#include "../Information/QueryPane.h"
#include "../Information/POUPane.h"
#include "../information/TablePane.h"
#include "../Topology/TopoPane.h"
#include "../Public/PropertyPane.h"
#include "../Framework/MFCToolBarEx.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

public:
	CLayerPane m_LayerPane;
	CAtlasPane m_AtlasPane;
	CTopoPane m_TopoPane;
	CQueryPane m_QueryPane;
	CPOUPane m_POUPane;
	CTablePane m_TablePane;
	COutputPane m_OutputPane;
	CPropertyPane m_PropertyPane;

public:
	CDrawToolWnd m_wndDrawTool;

protected:
	CMFCRibbonBar m_wndRibbonBar;

private:
	CStatusBarEx m_wndStatusBar;

public:
	CAtlas* m_pAtlas;
	CList<CPane*> m_regCmdMsg;
public:
	void UnregCmdMsg(CPane* pane);// called to un-register pane as command target
	void RegCmdMsg(CPane* pane); // called to register pane as command target
	void ActivatePane(char pane);

public:
	CStatusBarEx& GetStatusBar() { return m_wndStatusBar; }
	CMFCRibbonBar& GetRibbonBar() { return m_wndRibbonBar; }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL DestroyWindow() override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL OnMenuButtonToolHitTest(CMFCToolBarButton* pButton, TOOLINFO* pTI) override;
	BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd) override;
	BOOL OnCloseDockingPane(CDockablePane* pWnd) override;	
	//}}AFX_VIRTUAL
	// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnFilePreferences();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnAtlasNew();
	afx_msg void OnAtlasOpen();
	afx_msg void OnAtlasSave();
	afx_msg void OnAtlasClose();

	afx_msg void OnViewPane(UINT nId);
	afx_msg void OnViewDrawTool();
	afx_msg void OnViewCustomize();
	afx_msg void OnResetwindowLayout();
	afx_msg LRESULT OnChangingActiveTab(WPARAM, LPARAM);
	afx_msg LRESULT OnClosePane(WPARAM, LPARAM lp);
	afx_msg LRESULT OnResetMember(WPARAM wp, LPARAM);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LONG OnCrossThread(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnKeyQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSQLQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetQueryCtrl(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetPane(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFlashGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMatchMap(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetDatabase(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
void ExportFolder(CString strFold);
