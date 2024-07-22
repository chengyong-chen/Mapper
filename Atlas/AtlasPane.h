#pragma once

#include "CatalogTree.h"
#include "PyramidTree.h"

#include "../Public/PaneToolBar.h"

class CAtlas;
class CPage;

/////////////////////////////////////////////////////////////////////////////
// CAtlasPane dialog

class __declspec(dllexport) CAtlasPane : public CDockablePane
{
	DECLARE_DYNAMIC(CAtlasPane)

public:
	CAtlasPane();

public:
	CComboBox m_ComboBox;
	CPaneToolBar m_ToolBar;

	// Attributes
public:
	CCatalogTree m_CatalogTree;
	CPyramidTree m_PyramidTree;

public:
	CAtlas* m_pAtlas;

public:
	void Attach(CAtlas* pAtlas, bool showPyramid);
	void Detach(CAtlas* pAtlas);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtlasPane)
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CAtlasPane() override;

protected:
	// Generated message map functions
	//{{AFX_MSG(CAtlasPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCreatePyramid();
	afx_msg void OnAttribute();
	afx_msg void OnPublishPC();
	afx_msg void OnPublishCE();
	afx_msg void OnPublishWEB();
	afx_msg void OnAttachPOUDatabase();
	afx_msg void OnAttachLABDatabase();
	afx_msg void OnTopicSelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
