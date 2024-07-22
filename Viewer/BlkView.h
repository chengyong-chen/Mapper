#pragma once


#include "GisView.h"

class CBlkDoc;
class CBlock;

class CBlkView : public CGisView
{
protected:
	CBlkView();
	DECLARE_DYNCREATE(CBlkView)
	
// Attributes
public:
	CBlkDoc* GetDocument() { return (CBlkDoc*)m_pDocument;}	
private:
	// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlkView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBlkView();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CBlkView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};