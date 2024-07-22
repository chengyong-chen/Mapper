#pragma once

#include "Topodb.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CNode;
class CEdge;
class CViewinfo;

class __declspec(dllexport) CRoadTopodb : public CTopodb
{
public:
	CRoadTopodb(CDatabase* pDatabase);
	~CRoadTopodb() override;

public:
	void Initialize(const CDatainfo& datainfo) override;
	BYTE Gettype() const override { return 2; }

public:
	CNode* ApplyNode() const override;
	CEdge* ApplyEdge() const override;

public:
	void BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo) override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(pbf::writer& writer) const override {};
	void ExportPC(CString strFile, const CDatainfo& datainfo) override;
};
