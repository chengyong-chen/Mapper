#pragma once

#include <map>

using namespace std;
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CPoly;
class CTopology;
class CNode;
class CPolygon;
class CLayer;
class CLine;
class CViewinfo;

class __declspec(dllexport) CEdge
{
public:
	CEdge();
	virtual ~CEdge();

public:
	DWORD m_dwId;
	DWORD m_dwFromNode;
	DWORD m_dwToNode;

public:
	CRect m_Rect;

	WORD m_wLayer;
	DWORD m_dwPoly;
	CPoly* m_pPoly;

public:
	bool m_bActive;
	mutable	bool m_bModified;

public:
	virtual CEdge* Clone();
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseWeb(const CTopology& topology, pbf::writer& writer);
	virtual void ExportPC(std::map<DWORD, DWORD>& mapIdtoIndexNode, std::map<DWORD, DWORD>& mapIdtoIndexPoly, CFile& file);

	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLine& line, CRect inRect, const CTopology& topology);
	virtual void DrawActive(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CTopology& topology, float width);

public:
	CPoint GetStartPoint(const CTopology& topology) const;
	CPoint GetEndPoint(const CTopology& topology) const;
	CPoly* CreatePoly(const CTopology* pTopology, bool direction) const;

public:
	bool Pick(const CViewinfo& viewinfo, const CPoint& cliPoint, int width, const CTopology& topology) const;
	bool Pick(CPoly& docPolygon, const CTopology& topology) const;
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CTopology& topology) const;
};

typedef CTypedPtrList<CObList, CEdge*> CEdgeList;
