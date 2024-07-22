#pragma once

#include "GisView.h"
#include "Shockwaveflash.h"

class CPyrDoc;

class CPyrView : public CGisView
{
protected: // create from serialization only
	CPyrView();
	DECLARE_DYNCREATE(CPyrView)

// Attributes
public:
	CShockwaveflash m_GoogleMap;

public:
	CPyrDoc* GetDocument() { return (CPyrDoc*)m_pDocument;}

private:
	CString MakeInvokeXml(CString strFunc,CString strParam);
	// Overrides
	protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPyrView)
	virtual void OnInitialUpdate() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;	
public:
	virtual void OnFSCommond(LPCSTR commond, LPCSTR args) override;
	virtual void OnFlashCall(LPCTSTR args) override;
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CPyrView();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CPyrView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
