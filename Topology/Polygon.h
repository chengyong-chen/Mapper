#pragma once

#include "Polying.h"

namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CTopology;
class CPath;
class CPoly;
class CViewinfo;
class CRegionTopoer;
class __declspec(dllexport) CPolygon : public CPolying
{
public:
	CPolygon(CRegionTopoer& topology);
	virtual BYTE Gettype() const { return 51; };

public:
	using CPath::PickIn;
	bool PickIn(const CPoint& point) const override;
public:
	void RecalCentroid() override;
public:
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;

public:
	double GetArea() const override;
	unsigned long long Distance2(const CPoint& point) const override;
	void RemoveEdge(const DWORD& dwEdge);
public:
	using CPath::ReleaseWeb;
	void ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex);

public:
	static CGeom* Apply(BYTE index, CRegionTopoer& regiontopoer);
};

class __declspec(dllexport) CGonroup : public CCollection<CPolygon>
{
public:
	CGonroup(CRegionTopoer& regiontopoer);
	~CGonroup();

public:
	CRegionTopoer& m_topology;

public:
	BYTE Gettype() const override { return 52; };
public:
	CPolygon* FindInvolvedEdge(const CTopology& topology, DWORD dwEdgeId, CPolygon* exclude);
public:
	Gdiplus::PointF GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const override;
	void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const override;
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
public:
	void ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex);

public:
	void EdgeBred(DWORD dwEdgeId, DWORD dwBredId);
	void EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2);
};
