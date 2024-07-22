#pragma once
#include <list>
#include <afxcmn.h>
#include "Topo.h"
#include <list>
class CViewinfo;
class CTopology;


namespace Undoredo {
	class CActionStack;
};
class CDatainfo;

class __declspec(dllexport) CTopoCtrl abstract : public CListCtrl
{
public:
	CTopoCtrl(CDocument& document, const CDatainfo& datainfo, std::list<CTopology*>& topolist, Undoredo::CActionStack& actionstack);

	// Attributes
public:
	std::list<CTopology*>& m_topolist;
	CDocument& m_Document;	
	const CDatainfo& m_Datainfo;
	Undoredo::CActionStack& m_actionStack;
	// Operations
public:
	CTopology* GetSelTopo() const;
	void Create(CWnd* parent);
	void Built();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoCtrl)
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CTopoCtrl() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTopoCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStyle();
	afx_msg void OnDelete();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRemoveOrphanNodes();
	afx_msg void OnRemoveOrphanEdges();
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
