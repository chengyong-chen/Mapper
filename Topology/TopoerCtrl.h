#pragma once

class CTopoCtrl;
class CTopoer;
class CLayerTree;
class CDatainfo;
class CLayerTreeCtrl;

class __declspec(dllexport) CTopoerCtrl : public CTopoCtrl
{
public:
	CTopoerCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, CLayerTree& layertree, CLayerTreeCtrl& layerTreeCtrl, Undoredo::CActionStack& actionstack);

public:
	CLayerTree& m_layertree;
	CLayerTreeCtrl& m_layerTreeCtrl;

public:
	CTopoer* GetSelTopo() const;

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoerCtrl)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CTopoerCtrl() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTopoerCtrl)
	afx_msg void OnDelete();
	afx_msg void OnNewTrafficTopo();
	afx_msg void OnNewRegionTopo();
	afx_msg void OnCreatePolygons();
	afx_msg void OnCreateLinestrings();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
