#pragma once

#include "Topoer.h"

class CNode;
class CEdge;
class CDatainfo;

class __declspec(dllexport) CRoadTopoer : public CTopoer
{
public:
	CRoadTopoer(CLayerTree& layertree);
	~CRoadTopoer() override;
public:
	BYTE Gettype() const override { return 1; }
public:
	CNode* ApplyNode() const override;
	CEdge* ApplyEdge() const override;

public:
	void ActivateNode(CWnd* pWnd, const CViewinfo& viewinfo, CNode* pNode) const override;
	void ActivateEdge(CWnd* pWnd, const CViewinfo& viewinfo, CEdge* pEdge) const override;
	void InactivateAll(CWnd* pWnd, const CViewinfo& viewinfo) const override;
public:
	void BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo) override;
	void BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo) override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	void ReleaseWeb(pbf::writer& writer) const override {};
	
	void ExportPC(CString strFile, const CDatainfo& datainfo) override;
};
