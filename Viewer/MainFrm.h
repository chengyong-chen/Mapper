#pragma once

#include "RibbonBar.h"

#include "../Atlas/Security.h"
#include "../Atlas/AtlasPane.h"

#include "../Navigate/Vehicle.h"
#include "../Navigate/Route.h"
#include "../Navigate/NavinfoPane.h"
#include "../Navigate/VehiclePane.h"

#include "../Layer/LayerPane.h"

#include "../Information/QueryPane.h"
#include "../Information/POUPane.h"
#include "../information/TablePane.h"

class CMainBar;
class CPort;
class CIcon;
class CHtmlView;
class CReplay;
class CPOU;
class CDrawToolWnd;
class CAtlas;
//class CEaglePane;

class CAtlas;
class CPOIBar;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	// Attributes
public:
	static CSecurity m_Security;
	static CAtlas* m_pAtlas;

	// Operations
public:

public:
	CRibbonBar m_wndRibbonBar;
	CMFCToolBarImages m_imagePanel;

public:
	CLayerPane m_LayerPane;
	CAtlasPane m_AtlasPane;
	CQueryPane m_QueryPane;
	CPOUPane m_POUPane;
	CTablePane m_TablePane;
	CNavinfoPane m_NavinfoPane;
	CVehiclePane m_VehiclePane;
	//	CEaglePane*    m_pEaglePane; 

public:
	CHtmlView* m_pHtmlView;
	CPOIBar* m_pPOIBar;
	CPointF m_geoSign;

public:
	CPort* m_pPort;
	CReplay* m_pReplay;
	CMapVehicle m_mapVehicle;

public:
	CRoute m_Route;

public:
	BOOL CreateRibbonBar(CMFCRibbonBar& wndRibbonBar, CMFCToolBarImages& panelImages);

	static void ActivePane(char c)	{}

public:
	void OnLevelChanged(float fPos);
	void OnScaleChanged(float fPos);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	BOOL DestroyWindow() override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	void RecalcLayout(BOOL bNotify = TRUE) override;
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
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
	afx_msg void OnMove(int x, int y);
	afx_msg void OnGpsDevice();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGpsReplay();
	afx_msg void OnGpsTrack();
	afx_msg LONG OnSearchRoute(UINT WPARAM, LONG LPARAM);
	afx_msg void OnPaneAtlas();
	afx_msg void OnPaneLayer();
	afx_msg void OnPanePOU();
	afx_msg void OnPaneQuery();
	afx_msg void OnPaneVehicle();
	afx_msg void OnPaneNavigate();
	afx_msg void OnPaneTable();
	afx_msg void OnUpdatePaneAtlas(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneLayer(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanePOU(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneQuery(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneTable(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneNavinfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaneVehicle(CCmdUI* pCmdUI);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LRESULT OnChangingActiveTab(WPARAM, LPARAM);

	afx_msg LONG OnKeyQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSQLQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetQueryCtrl(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetPane(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnGetVehicle(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnVehicleControl(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnVehicleMove(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnVehicleStatus(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnVehicleIcon(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnVehicleMessage(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnActivateVehicle(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSetGeoSign(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnChildDocChanged(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetPort(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnVehicleDispatch(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnViewWillDraw(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnViewDrawOver(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnBrowseHtml(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnDestroyHtml(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnMatchMap(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetMainChild(UINT WPARAM, LONG LPARAM);
	afx_msg LRESULT OnURLChanged(WPARAM wp, LPARAM lp);
	afx_msg LONG OnFlashGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetDatabase(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
