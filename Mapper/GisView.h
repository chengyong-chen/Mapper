#pragma once

#include "GrfView.h"
#include "../Framework/AidView.hpp"

class CGisDoc;
class CThirdMap;

class CGisView : public CGrfView
{
	DECLARE_GWDYNCREATE(CGisView)
protected:
	CGisView(CObject& parameter) noexcept;

private:
	CGisDoc& m_document;
	CAidView<CGisView>* m_pAidView;

public:
	void RefreshPreviewCenter() const override;
	void SetAidView(CView* pAidView) override { m_pAidView = (CAidView<CGisView>*)pAidView; };
	//static HHOOK g_mousehook;
	//static HHOOK g_procrethook;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGisView)
public:
	void OnInitialUpdate() override; // called first time after construct
protected:
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
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
	afx_msg void OnFrameSphere();
	afx_msg void OnFrameGraticule();	
	afx_msg void OnPreviewPeojectionChanged();
	afx_msg void OnPreviewConfig();
	afx_msg void OnPreviewCenterChanged();

	DECLARE_EVENTSINK_MAP()
		//}}AFX_MSG	
	afx_msg LONG OnPositionPOU(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnInjectATT(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnHighlightATT(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnShowRecInfo(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnLayerActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPOUActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGeomActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreDeleteGeom(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnChangeBackground(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnAlphaTransparency(UINT WPARAM, LONG LPARAM);	
	DECLARE_MESSAGE_MAP()

public:
	static void WINAPI StaticCallback(void* point, CStringA strCommand, CStringA strArgument)
	{
		if(point != nullptr)
		{
			CGisView* pGisView = (CGisView*)point;
			return pGisView->GotMessageFromPipe(strCommand, strArgument);
		}
	}

	void GotMessageFromPipe(CStringA strCommand, CStringA strArgument);

	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ProcretProc(int nCode, WPARAM wParam, LPARAM lParam);
	static UINT ThreadSetBackground(LPVOID pParam);
};
