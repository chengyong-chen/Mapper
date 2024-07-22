#pragma once

#include "GisDoc.h"

class CBlock;

class CBlkDoc : public CGisDoc
{
protected: 
	CBlkDoc();
	DECLARE_DYNCREATE(CBlkDoc)

public:
	int m_nXCount;
	int m_nYCount;
	int m_nWidth;
	int m_nHeight;
	
public:
	virtual void Draw(CView* pView,Graphics& g,DrawArgu& drawArgu) override;
	virtual void DrawTag(CView* pView,Graphics& g,DrawArgu& drawArgu) override;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlkDoc)
	public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion) override;   // overridden for document i/o
	//}}AFX_VIRTUAL
	
// Implementation
public:
	virtual      ~CBlkDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CBlkDoc)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
