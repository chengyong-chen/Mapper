#pragma once

#include "../Geometry/Path.h"
#include "../Geometry/Collection.h"

class CTopology;
class CPath;
class CPoly;
class CRegionTopoer;
class CViewinfo;

class __declspec(dllexport) CPolying : public CPath
{
public:
	CPolying(CRegionTopoer& topology);
	~CPolying();

public:
	CRegionTopoer& m_topology;
	CDWordArray m_edgelist;

	virtual BYTE Gettype() const { return 61; };

public:
	bool IsEdgeInvolved(const CTopology& topology, const DWORD& dwEdgeId);

public:
	void AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const override;
	void AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const override;

public:	
	void RecalRect() override;
	CPolying* RemoveEdge(CRegionTopoer& topology, const DWORD& dwEdge);

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
public:
	using CPath::ReleaseWeb;
	void ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex);

public:
	void EdgeBred(DWORD dwEdgeId, DWORD dwBredId);
	void EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2);
};

class __declspec(dllexport) CIngroup : public CCollection<CPolying>
{
public:
	CIngroup(CRegionTopoer& regiontopoer);
	~CIngroup();

public:
	CRegionTopoer& m_topology;

public:
	BYTE Gettype() const override { return 62; };
public:
	void RemoveEdge(CRegionTopoer& topology, const DWORD& dwEdge);

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
public:
	void ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex);

public:
	void EdgeBred(DWORD dwEdgeId, DWORD dwBredId);
	void EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2);
};
