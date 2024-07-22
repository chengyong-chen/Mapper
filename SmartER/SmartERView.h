#pragma once

#include "../Dataview/ViewMonitor.h"
#include "../Dataview/ViewPrinter.h"

#include "..\Framework\gfx.h"
class CSmartERDoc;

class CSmartERView : public CScrollView
{
	DECLARE_GWDYNCREATE(CSmartERView)
protected: // create from serialization only
	CSmartERView(CObject& parameter);
private:
	CSmartERDoc& m_document;

// Attributes
public:
	CViewMonitor m_viewMonitor;
	CViewPrinter m_viewPrinter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartERView)
	public:
	virtual void OnDraw(CDC* pDC) override;  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
	virtual void OnInitialUpdate() override; // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmartERView();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnArrange(UINT dwId);
	afx_msg void OnAlign(UINT nId);
	DECLARE_MESSAGE_MAP()
};

