#pragma once
#include <list>
#include "Topoer.h"

class CPoly;
class CDatainfo;

class __declspec(dllexport) CRegionTopoer : public CTopoer
{
public:
	CRegionTopoer(CLayerTree& layertree, std::list<CLayon*>& layonlist);
	~CRegionTopoer() override;

public:
	std::list<CLayon*> m_layonlist;

public:
	BYTE Gettype() const override { return 3; }
	
	CNode* ApplyNode() const override;
	CEdge* ApplyEdge() const override;

public:
	void FilterNode(const CDatainfo& datainfo) override;
	void FilterEdge(const CDatainfo& datainfo) override;
public:
	bool PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId) override;
	bool PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2) override;

public: 
	void PreDelete() override;
	void RemoveEdge(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwId) override;
private:
	void DeletePolygon(CWnd* pWnd, const CViewinfo& viewinfo, CPolygon* pPolygon);

public:
	void CreateLinestrings(const CDatainfo& datainfo, CLayon* layon);
	void CreatePolygons(const CDatainfo& datainfo, CLayon* layon);

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(pbf::writer& writer) const override;
};
