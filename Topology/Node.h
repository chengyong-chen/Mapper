#pragma once

#include <map>
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CTopology;
class CEdge;
class CViewinfo;

class __declspec(dllexport) CNode 
{
public:
	CNode();
	virtual ~CNode();
	struct EdgeInfo 
	{
		CEdge* edge;
		float Qi;
	};

public:
	DWORD m_dwId;
	CPoint m_Point;
	CDWordArray m_edgelist;

public:
	mutable	bool m_bModified;

public:
	virtual CNode* Clone();

public:
	CPoint GetPoint() const
	{
		return m_Point;
	}
	int GetEdgeCount() const
	{
		return m_edgelist.GetSize();
	}
	int GetEdgeTypeCount() const
	{
		return m_edgelist.GetSize();
	}

	DWORD GetNextEdge(const DWORD& dwEdge) const;
	BYTE GetEdgeIndex(const CEdge* pEdge) const;
	BYTE GetEdgeIndex(const DWORD& dwEdge) const;
	float EvaluatQi(const CPoint& point) const;

public:
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, Gdiplus::Brush* brush, CSize& radius, CRect inRect, const CTopology& topology);
	virtual void DrawActive(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CTopology& topology, CSize radius);

	virtual void CenterPoint(const CTopology& topology);
	virtual void SortEdge(const CTopology& topology, int radius);

	virtual void ReplaceRelatingEdge(DWORD dwId1, DWORD dwId2);
public:
	virtual void RemoveRelatingEdge(const DWORD& dwEdge);
	virtual void InsertRelatingEdge(const DWORD& dwEdge);

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseWeb(pbf::writer& writer) const;
	virtual void ExportPC1(CFile& file, DWORD& datePos) const;
	virtual void ExportPC2(std::map<DWORD, DWORD>& mapIdtoIndexEdge, CFile& file) const;

public:
	virtual bool Pick(const CViewinfo& viewinfo, const CPoint& cliPoint, int radius) const;
	virtual void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CTopology& topology) const;
};

typedef CTypedPtrList<CObList, CNode*> CNodeList;
