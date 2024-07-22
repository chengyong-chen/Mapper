#pragma once

#include <map>

static const BYTE NODE_TRAFFICLIGHT=0X01;
static const BYTE NODE_WAYEXIT=0X02;
static const BYTE NODE_WAYENTRY=0X04;

#include "Node.h"
#include "Matrix.h"


class CDatainfo;
class CODBCRecordset;

class __declspec(dllexport) CRoadNode : public CNode
{
public:
	CRoadNode();
	~CRoadNode() override;

public:
	BYTE m_bAttribute1;
	CMatrix m_Forbid;

public:
	CNode* Clone() override;

public:
	void SetForbid(DWORD dwEdge1, DWORD dwEdge2, bool bValue) const;
public:
	bool CanThruEdge(const DWORD& dwEdgeId) const;
	bool CanEdgeThru(const DWORD& dwEdgeId) const;

public:
	void RemoveRelatingEdge(const DWORD& dwEdge) override;
	void InsertRelatingEdge(const DWORD& dwEdge) override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion);
	void ExportPC1(CFile& file, DWORD& datePos) const override;
	void ExportPC2(std::map<DWORD, DWORD>& mapIdtoIndexEdge, CFile& file) const override;

	virtual bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo);
	virtual bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const;
};
