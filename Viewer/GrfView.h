#pragma once

#include "../Tool/EditGeom.h"

#include "../Dataview/ViewMonitor.h"
#include "../Dataview/ViewPrinter.h"

#include "../Framework/gfx.h"
class CLayerTreeCtrl;
class CLayer;
class CGrfDoc;
class CGrfView;
class CDDraw;
class CMyPrint;

class CGrfView : public CView
{
	DECLARE_GWDYNCREATE(CGrfView)
protected:
	CGrfView(CObject& parameter);
private:
	CGrfDoc& m_document;

public:
	CViewMonitor m_viewMonitor;
	CViewPrinter m_viewPrinter;
	static short m_nAngle;
	CTypedPtrList<CObList, CGeom*> m_hlGeomList;

public:
	void ZoomView(float fRatio, const CPoint& docPoint, const CPoint& cliPoint);
	void ChangeViewScale(float fScale);
	void ChangeViewLevel(float fLevel);
	BOOL ChangeViewZoom(float xZoom, float yZoom, const CPoint& docPoint);
	void SetViewSize(const CPoint& docPoint, const CPoint& cliPoint, BOOL bScroll);
	void RotateView(short nAngle, const CPoint& docPoint, const CPoint& cliPoint);
	virtual void UnhighligtAll();

public:
	static CLIPFORMAT m_ClipboardFormat; // custom clipboard format

	CGeom* m_pFlash;
	int m_nDelay;
	CPoint m_pFixed;

	CRect m_PrtRect;

	CString m_strPOU;
public:
	CLayer* GetActiveLayer() const;
public:
	HANDLE m_hDrawOver;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrfView)
public:
	virtual BOOL KillTimer(UINT nIDEvent);
	void OnDraw(CDC* pDC) override; // overridden to draw this view
	void OnInitialUpdate() override; // called first time after construct
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
protected:
	BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnPrint(CDC* pDC, CPrintInfo* pInfo) override;
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder) override;
	BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGrfView() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

public:
	virtual void SetEagleRect();
	// Generated message map functions
protected:
	//{{AFX_MSG(CGrfView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnToolZoomIn();
	afx_msg void OnToolZoomOut();
	afx_msg void OnToolPrint();
	afx_msg void OnToolPan();
	afx_msg void OnToolRadius();
	afx_msg void OnToolBound();
	afx_msg void OnToolRegion();
	afx_msg void OnToolLength();
	afx_msg void OnToolArea();

	afx_msg void OnGpsDispatch();
	afx_msg void OnToolMarquee();
	afx_msg void OnLevelSliderPosChanged();
	afx_msg void OnScaleSliderPosChanged();
	afx_msg void OnLevelGo();
	afx_msg void OnScaleGo();
	//}}AFX_MSG
	afx_msg LONG OnGetViewinfo(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGeoPointInMap(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnLookupGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetPaperSize(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnSetPrintRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnCanZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFixateView(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFixateRect(UINT WPARAM, LONG LPARAM);

	afx_msg LONG OnRgnQuery(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnFlashGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnKeyQuery(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
