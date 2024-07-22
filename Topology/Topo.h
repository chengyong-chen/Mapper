#pragma once
#include <map>
#include "../Style/ColorSpot.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
namespace Undoredo
{
	class CActionStack;
}
class CPoly;
class CNode;
class CEdge;
class CPolygon;
class CLayon;
class CLayer;
class CViewinfo;
class CLayer;
class CDatainfo;
class CActionStack;

class __declspec(dllexport) CTopology abstract
{
public:
	typedef std::map<DWORD, CNode*> CNodeMap;
	typedef std::map<DWORD, CEdge*> CEdgeMap;

public:
	CTopology();
	virtual ~CTopology();

public:
	virtual CNode* ApplyNode() const abstract;
	virtual CNode* ApplyNode(const CPoint& point) const;
	virtual CEdge* ApplyEdge() const abstract;
	virtual CNode* GetNode(const CPoint& docPoint) const;
	
public:
	CNodeMap m_mapIdNode;
	CEdgeMap m_mapIdEdge;

	mutable DWORD m_dwMaxEdgeId;
	mutable DWORD m_dwMaxNodeId;

public:
	CString m_strName;
	CRect m_Rect;

	bool m_bVisible;
	bool m_bModifyFlag;

public:
	CColorSpot m_colorNode;
	CColorSpot m_colorEdge;


public:
	virtual void Initialize(const CViewinfo& viewinfo)
	{
	};
	virtual BYTE Gettype() const=0;

	virtual bool IsPolyExisting(const CPoly* pPoly) const;
	bool IsModified() const
	{
		return m_bModifyFlag;
	};
	void SetModified()
	{
		m_bModifyFlag = true;
	};
public:
	virtual CNode* PickNode(const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius) const;
	virtual CEdge* PickEdge(const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius) const;
	virtual void PickEdges(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point, int nRadius) const;
	virtual void PickEdges(CWnd* pWnd, const CViewinfo& viewinfo, CPoly& polygon) const;

public:
	virtual void ActivateNode(CWnd* pWnd, const CViewinfo& viewinfo, CNode* pNode) const;
	virtual void ActivateEdge(CWnd* pWnd, const CViewinfo& viewinfo, CEdge* pEdge) const;
	virtual void InactivateAll(CWnd* pWnd, const CViewinfo& viewinfo) const;

public:
	DWORD GetEdgeIndex(const DWORD& dwEdgeId) const;
	DWORD GetNodeIndex(const DWORD& dwNodeId) const;

	CNode* GetNode(const DWORD& dwNodeId) const;
	CEdge* GetEdge(const DWORD& dwEdgeId) const;
	CEdge* GetEdge(const DWORD& dwNodeId1, const DWORD& dwNodeId2) const;
	CEdge* GetEdge(const WORD& wLayer, const DWORD& dwPloy) const;
	virtual CLayer* GetEdgeLayer(const DWORD& dwEdgeId) const { return nullptr; };
	virtual CPoly* GetEdgePoly(const DWORD& dwEdgeId) const { return nullptr; };
	virtual bool EdgeJoinable(const WORD& wLayer, const DWORD& dwPoly1, const DWORD& dwPoly2, const BYTE& mode) const;

public:
	virtual void FilterNode(const CDatainfo& datainfo)
	{
	};

	virtual void FilterEdge(const CDatainfo& datainfo)
	{
	};

public:
	virtual void PreRemovePoly(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwPolyId);
	virtual void PreDelete() {};				
	void DeleteIfOrphan(CNode* pNode);

public:
	virtual void RemoveEdge(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwId);
	virtual void RemoveNode(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwId);
	virtual void RemovePoly(CEdge* pEdge);

	virtual bool InsertEdge(CWnd* pWnd, const CViewinfo& viewinfo, const WORD& wLayer, const CPoly* pPoly, const unsigned int& tolerance);
	
	virtual bool CrossEdges(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& point, int nRadius, Undoredo::CActionStack& actionstack);
public:
	virtual bool PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId)=0;
	virtual bool PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2)=0;

public:
	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& inRect) const;
	virtual void Invalidate(CDocument* pDocument) const;
	virtual BOOL SetStyle();

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ExportPC(CString strFile, const CDatainfo& datainfo)
	{
	};
	virtual void ReleaseWeb(pbf::writer& writer) const =0;
public:
	virtual void OnClear(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo);

public:
	virtual void NewNodeCreated(CNode* pNode)
	{
	};

	virtual void NewEdgeCreated(CEdge* pEdge)
	{
	};

	virtual void PreRemoveNode(CNode* pNode)
	{
	};

	virtual void PreRemoveEdge(CEdge* pEdge)
	{
	};

	virtual void BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo);
	virtual void BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo);

public:
	mutable CNode* m_pActivateNode;

public:
	void CheckValidity();
};
