#pragma once

#include "Topo.h"
#include "../Public/Global.h"

class CNode;
class CEdge;
class CRectF;

class __declspec(dllexport) CTopodb abstract: public CTopology
{
public:
	CTopodb();
	CTopodb(CDatabase* pDatabase);

	~CTopodb() override;

public:
	CDatabase* m_pDatabase;

	CString m_tableEdge;
	CString m_tableNode;
	CString m_tablePolyData;
	CString m_tablePolyAttribute;

	CString m_strFilter;

public:
	CRectF m_rectCurrent;

public:
	virtual void SetTables();
	virtual void Initialize(const CDatainfo& datainfo);

public:
	virtual DWORD ApplyEdgeId() const;
	virtual DWORD ApplyNodeId() const;
	bool IsPolyExisting(const CPoly* pPoly) const override;

public:
	CPoly* GetEdgePoly(const DWORD& dwEdgeId) const override;

public:
	void RemovePoly(CEdge* pEdge) override;

public:
	virtual void OnRectChanged(CRectF& mapRect, const CDatainfo& datainfo);

public:
	bool PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId) override;
	bool PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2) override;

public:
	void NewNodeCreated(CNode* pNode) override;
	void NewEdgeCreated(CEdge* pEdge) override;
	void PreRemoveNode(CNode* pNode) override;
	void PreRemoveEdge(CEdge* pEdge) override;
	void BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo) override;
	void BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo) override;

public:
	DWORD DuplicateRec(const CString& strTable, DWORD dwId, DWORD dwNewId);
	DWORD GetMaxId(const CString& strTable) const;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ExportPC(CString strFile, const CDatainfo& datainfo) override;
};

typedef CTypedPtrList<CObList, CTopodb*> CTopodbList;
