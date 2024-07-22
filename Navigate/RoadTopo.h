#pragma once

class CPoly;
struct Probe;
class CFilePoly;

#include "../Public/Global.h"

class __declspec(dllexport) CRoadTopo : public CObject
{
	struct Node
	{
		CPoint m_Point;
		DWORD m_dwPos;
		BYTE m_nDimension;
	};

	struct Edge
	{
	public:
		CRect m_Rect;
		DWORD m_indexFromNode;
		DWORD m_indexToNode;
		DWORD m_indexPoly;

		float m_fLength;

		BYTE m_designedSpeed;
		BYTE m_restrictSpeed;
		BYTE m_cashToll;
		BYTE m_Mode;
	};

	DECLARE_SERIAL(CRoadTopo);

public:
	CRoadTopo();
	~CRoadTopo() override;

public:
	DWORD m_nNodeCount;//当创建自己的起始接点时数量加2，当创建自己的终止节点时数量再加2
	DWORD m_nEdgeCount;//当创建自己的起始边  时数量加3，当创建自己的终止  边时数量再加3
	DWORD m_nPolyCount;

public:
	Node* m_pNodes;//后面要多创建4个，2个给起点的临时创建的接点，2个给终点的临时创建的接点，
	Edge* m_pEdges; //在最后面要多创建6个，分别存放起始和终止点产生的临时边

	BYTE* m_pNodeDate;
	DWORD m_SizeNode;

	CFilePoly* m_pPolyFile;

public:
	DWORD m_dwEnterEdge; // 临时保存为上路的边，仅仅用来出来当开始边和结束边为同一条边的特殊情况
public:
	bool Load(CString strFile);

public:
	CPoly* GetEdgePoly(const DWORD& dwEdge) const;
	DWORD GetProximalEdge(const CPoint& point, CPoint& trend) const;
	DWORD GetProximalNode(const CPoint& point) const;

public:
	DWORD s_dwOlderEdge;
	CPoly* s_pNewlyPoly;
	CPoly* s_pCutedPoly1;
	CPoly* s_pCutedPoly2;

	DWORD e_dwOlderEdge;
	CPoly* e_pNewlyPoly;
	CPoly* e_pCutedPoly1;
	CPoly* e_pCutedPoly2;

	DWORD CreatStartTousy(const CPointF& geoPoint);
	DWORD CreatStopTousy(const CPointF& geoPoint);

	Probe* Dijkstra(DWORD dwHeadNode, DWORD dwTailNode);//求两点最短路径,循环计算各点到起点的距离
	void Release();

public:
	void ReplaceNodeEdge(DWORD dwNode, DWORD dwId1, DWORD dwId2) const;
	bool GetNodeForbid(DWORD dwNode, BYTE bRow, BYTE nCol) const;
	void SetNodeForbid(DWORD dwNode, BYTE bRow, BYTE nCol, bool bValue) const;

	DWORD GetNodeEdge(DWORD dwNode, BYTE index) const;
	BYTE GetNodeEdgeIndex(DWORD dwNode, const DWORD& dwEdge) const;
public:
	bool CanNodeTurn(DWORD dwNode, BYTE from, BYTE to);
	bool CanNodeThruEdge(DWORD dwNode, const DWORD& dwEdge);
	bool CanNodeEdgeThru(DWORD dwNode, const DWORD& dwEdge);
};
