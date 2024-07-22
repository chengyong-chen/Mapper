#pragma once

#include "GisDoc.h"

class CPyrDoc : public CGisDoc
{
protected: // create from serialization only
	CPyrDoc();
	DECLARE_DYNCREATE(CPyrDoc)

public:
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPyrDoc)
	protected:
	virtual BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL
	
// Implementation
public:
	virtual      ~CPyrDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CPyrDoc)		
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
