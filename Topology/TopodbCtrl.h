#pragma once
#include <list>

namespace Undoredo {
	class CActionStack;
};
class TopoCtrl;
class CTopodb;

class __declspec(dllexport) CTopodbCtrl : public CTopoCtrl
{
public:
	CTopodbCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, Undoredo::CActionStack& actionstack);

	// Attributes
public:
	CDatabase* m_pDatabase;
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopodbCtrl)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

public:
	CTopodb* GetSelTopo() const;

	// Implementation
public:
	~CTopodbCtrl() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTopodbCtrl)
	afx_msg void OnNewTrafficTopo();
	afx_msg void OnNewRegionTopo();
	afx_msg void OnSetTables();
	afx_msg void OnInitialize();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
