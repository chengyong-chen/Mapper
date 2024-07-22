#pragma once

#include "Topo.h"

class Node;
class CRoadNode;
class CEdge;
class CLayerTree;
class CLayer;
class CDatainfo;

class __declspec(dllexport) CTopoer abstract : public CTopology
{
public:
	CTopoer(CLayerTree& layertree);
	~CTopoer() override;

public:
	CLayerTree& m_layertree;

public:
	virtual DWORD ApplyEdgeId() const;
	virtual DWORD ApplyNodeId() const;
	CEdge* AddAEdge(const CLayer* layer, const CPoly* poly);

public:
	CLayer* GetEdgeLayer(const DWORD& dwEdgeId) const override;
	CPoly* GetEdgePoly(const DWORD& dwEdgeId) const override;

public:
	void RemovePoly(CEdge* pEdge) override;

public:
	bool PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId) override;
	bool PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2) override;
};

typedef CTypedPtrList<CObList, CTopoer*> CTopoerList;
