#pragma once

#include "../Dataview/ViewMonitor.h"
#include "../Dataview/ViewPrinter.h"
#include "../Tool/EditGeom.h"

#include "../Framework/gfx.h"
#include "../Framework/AidView.hpp"

class CLayerTree;
class CLayer;
class CMyPrint;

class CStatusBarEx;
class CGrfDoc;

class CGrfView : public CView
{
	DECLARE_GWDYNCREATE(CGrfView)
protected:
	CGrfView(CObject& parameter) noexcept;
private:
	CGrfDoc& m_document;
	CAidView<CGrfView>* m_pAidView;

public:
	CViewMonitor m_viewMonitor;
	CViewPrinter m_viewPrinter;
	CEditGeom m_editgeom;

public:
	CLayer* GetActiveLayer() const;
	static CStatusBarEx* GetStatusBar();

public:
	HANDLE m_hDrawOver;

protected:
	CTypedPtrList<CObList, CGeom*> m_hlGeomList;

public:
	virtual CRect GetVisibleDocRect() const;
	virtual void SetAidView(CView* pAidView) { m_pAidView = (CAidView<CGrfView>*)pAidView; };
	virtual void InactivateAll();
	virtual void UnhighligtAll();
	virtual void ChangeViewLevel(float fLevel);
	virtual void ChangeViewScale(float fScale);
	virtual void RefreshPreviewCenter() const
	{
	};
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrfView)
public:
	void OnDraw(CDC* pDC) override; // overridden to draw this view
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	void OnInitialUpdate() override; // called first time after construct
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
protected:
	void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder) override;
	BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnPrint(CDC* pDC, CPrintInfo* pInfo) override;
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGrfView() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrfView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelectAllByType(UINT dwId);
	afx_msg void OnSelectAllByName();
	afx_msg void OnSelectAllInverse();
	afx_msg void OnSelectAllSameStyle();
	afx_msg void OnSelectAllIdentical();
	afx_msg void OnSelectAllDiscard();
	afx_msg void OnViewGrid();
	afx_msg void OnMagnifyZoomin();
	afx_msg void OnMagnifyZoomout();
	afx_msg void OnMagnifyFitinPage();
	afx_msg void OnMagnifyFitActive();
	afx_msg void OnMagnifyPaper1to1();
	afx_msg void OnMagnifyScreen1to1();
	afx_msg void OnGeomProperty();
	afx_msg void OnEditInsertOle();
	afx_msg void OnArrange(UINT dwId);
	afx_msg void OnGeomJoin();
	afx_msg void OnGeomSetStyle();
	afx_msg void OnGeomSetSpot();
	afx_msg void OnGeomSetType();
	afx_msg void OnGeomSetHint();
	afx_msg void OnGeomRegress();
	afx_msg void OnShowCentroid(UINT dwId);
	afx_msg void OnShowTags(UINT dwId);
	//}}AFX_MSG  
	afx_msg LONG OnRemovGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnIsTopMost(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnLayerActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetViewinfo(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetPaperSize(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetLayerList(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetActiveRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetActiveArea(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnCaptureImage(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetGeomInflate(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnActiveGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnDocModified(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSetPrintRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnExportDib(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickActiveGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickAnchor(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickByRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickByCircle(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickByPoly(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSplitGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnLookupGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTransform(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTransformed(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMoveGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFixateView(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnNewGeomDrew(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnNewGeomBred(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnToRemoveEdge(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreRemoveGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreDeleteGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreRemoveLayer(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreDeleteLayer(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnApplyGeomID(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnKeyQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFlashGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTopoPick(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTopoPickByPolygon(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTopoSplit(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTopoCross(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnPublishPC(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPublishCE(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPublishWEB(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
	//	friend DWORD DrawProc(DRAWPARAM* pDrawParam);
};