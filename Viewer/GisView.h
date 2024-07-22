#pragma once

#include "GrfView.h"

class CODBCDatabase;
class CGisDoc;
class CVehicle;
class CPOUListCtrl;

class CGisView : public CGrfView
{
	DECLARE_GWDYNCREATE(CGisView)
protected:
	CGisView(CObject& parameter);
private:
	CGisDoc& m_document;
private:
	CToolTipCtrl m_tooltip;
	//	CMFCToolTipCtrl m_tooltip; 

	// Attributes
public:
	CPoint m_point;

private:
	// Operations
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGisView)
public:
	void OnInitialUpdate() override; // called first time after construct
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGisView() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGisView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnToolLink();
	afx_msg void OnToolInfo();
	afx_msg void OnToolRoute();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnPickLink(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnHighlightATT(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnShowRecInfo(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnRgnQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnLayerActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPOUActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSetRoute(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnChangeBackground(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnAlphaTransparency(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	static void WINAPI StaticCallback(void* point, CStringA strCommand, CStringA strArgument)
	{
		if(point==nullptr)
			return;
		else
		{
			CGisView* pGisView = (CGisView*)point;
			return pGisView->GotMessageFromPipe(strCommand, strArgument);
		}
	}

	void GotMessageFromPipe(CStringA strCommand, CStringA strArgument);
	static UINT ThreadSetBackground(LPVOID pParam);
};
