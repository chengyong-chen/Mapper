#pragma once

#include "../Tool/Tool.h"

#include "../Style/Library.h"
#include "../Layer/LayerTree.h"
#include "../Dataview/Datainfo.h"
#include "../Action/ActionStack.h"

class CGeomMapFile;
class CGridMapFile;
class CLayerTreeCtrl;
class CDeck;

class CGrfDoc : public COleDocument
{
protected: // create from serialization only
	CGrfDoc();
	DECLARE_DYNCREATE(CGrfDoc)

public:
	CLayerTree m_layertree;
	CDatainfo m_Datainfo;

public:
	DWORD m_dwMaxGeomId;

public:
	CGridMapFile* m_pGridMapFile;
	CGeomMapFile* m_pGeomMapFile;

public:
	CLayerTreeCtrl* m_pLayerTreeCtrl;
	CDeck* m_pDeck;

public:
	CTool* m_pTool;
	CTool* m_oldTool;

public:
	Undoredo::CActionStack m_actionStack;

public:
	virtual void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);
	virtual void DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);

	void FindLocation(LOCATION& location, double& lng, double& lat);
	CGeom* GetGeom(const DWORD& dwId, const CRect& rect) const;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrfDoc)
public:
	void SetTitle(LPCTSTR lpszTitle) override;
	BOOL OnOpenDocument(LPCTSTR lpszPathName) override;

protected:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion); // overridden for document i/o
	//}}AFX_VIRTUAL

	// Implementation
	~CGrfDoc() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrfDoc)	
	afx_msg void OnViewLegend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
