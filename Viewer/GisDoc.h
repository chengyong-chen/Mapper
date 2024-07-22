#pragma once

#include "GrfDoc.h"

#include "../Hyperlink/Link.h"
#include "../Information/POUList.h"
#include "../Information/POUListCtrl.h"

using namespace std;
#include <list>

class CRoadTopo;
class CTopoRoute;
class CTheme;

class CGisDoc : public CGrfDoc
{
protected:
	CGisDoc();
	DECLARE_DYNCREATE(CGisDoc)

public:
	CPOUList m_poulist;
	CPOUListCtrl m_POUListCtrl;

	std::list<CLink*> m_linklist;

public:
	void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect) override;
	void DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect) override;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGisDoc)
protected:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override; // overridden for document i/o
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGisDoc() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGisDoc)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
