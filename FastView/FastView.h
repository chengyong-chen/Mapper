#pragma once

class CFastDoc;
class CFastView : public CScrollView
{
protected: // create from serialization only
	CFastView();
	DECLARE_DYNCREATE(CFastView)
public:
	SIZE      WindowSize;
	float     m_Zoom;
// Attributes
public:
	CFastDoc* GetDocument()           { return (CFastDoc*)m_pDocument; }

	// Operations
public:
	void DocToClient(const CRect& rect);
	void DocToClient(const CPoint& point);
	void ClientToDoc(const CPoint& point);
	void ClientToDoc(const CRect& rect);

	void SetViewWindowSize(CPoint docpoint,CPoint devipoint);
//	void  SaveParaments();
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastView)
	public:
	virtual void OnDraw(CDC* pDC) override;  // overridden to draw this view
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) override;	  
	protected:
	virtual void SetScrollSizes(int nMapMode, SIZE sizeTotal,const SIZE& sizePage, const SIZE& sizeLine) override;
	virtual void OnInitialUpdate() override; // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo) override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual     ~CFastView();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CFastView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
