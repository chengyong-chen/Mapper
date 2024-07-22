#pragma once

//for m_bAttribute2;
static const BYTE ROAD_SHAPE = 0X03; //0000 0011
static const BYTE ROAD_HEIGHT = 0X0C; //0000 1100
static const BYTE ROAD_GRADIENT = 0X30; //0011 0000
static const BYTE ROAD_LANEWIDTH = 0XC0; //1100 0000

#include "Edge.h"

class CTopology;
class CODBCRecordset;

class __declspec(dllexport) CRoadEdge : public CEdge
{
public:
	CRoadEdge();
	~CRoadEdge() override;

public:
	float m_fLength;

	BYTE m_modeRestrictSpeed;
	BYTE m_designedSpeed;
	BYTE m_restrictSpeed;

	BYTE m_modeToll;
	BYTE m_cashToll;

public:
	CEdge* Clone() override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ExportPC(std::map<DWORD, DWORD>& mapIdtoIndexNode, std::map<DWORD, DWORD>& mapIdtoIndexPoly, CFile& file) override;

	virtual bool GetValues(CDatabase* pDatabase, const CString& tableEdge, const CDatainfo& datainfo, const DWORD& dwId);
	virtual bool GetValues(CODBCRecordset& rs);
	virtual bool PutValues(CODBCRecordset& rs) const;

public:
	void CalLength(CView* pView, const CDatainfo& datainfo, const CTopology& topology);

public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLine& line, CRect inRect, const CTopology& topology) override;
};
