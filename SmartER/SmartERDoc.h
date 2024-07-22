#pragma once


#include "../Dataview/Datainfo.h"
#include "../DataView/ViewMonitor.h"

class CSmartERDoc : public CDocument
{
protected: // create from serialization only
	CSmartERDoc();
	DECLARE_DYNCREATE(CSmartERDoc)

// Attributes
public:
public:
	CDatainfo m_Datainfo;

// Operations
public:
	virtual void Draw(CView* pView,CViewinfo& viewinfo,Graphics& g,CRect& invalidRect) override;
// Overrides
public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;
// Implementation
public:
	virtual ~CSmartERDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSmartERDoc)	
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};
