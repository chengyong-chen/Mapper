#pragma once

#include "Layer.h"


#include "../Style/Library.h"
#include "../Public/Tree.h"
class CLayer;
class CODBCDatabase;
class CODBCDatabase;
class CDatainfo;
class AFX_EXT_CLASS CLayerTree : public CTree<CLayer>
{
public:
	CLayerTree();
	virtual ~CLayerTree();

public:
	CLibrary m_library;

public:
	std::map<CString, CODBCDatabase*> m_GeoDBList;
	std::map<CString, CODBCDatabase*> m_AttDBList;

public:
	CODBCDatabase* GetAttDatabase(CString strName) const;
	CODBCDatabase* GetGeoDatabase(CString strName) const;

public:
	void Initialize();
	void Wipeout();
	void Normalize();
	//void AddHead(CLayer* pLayer);
	//void AddTail(CLayer* pLayer);
	//void Add(CLayer* pLayer);

	CGeom* GetGeomById(DWORD dwGeomId) const;
	CGeom* GetGeomById(WORD wLayerId, DWORD dwGeomId) const;
	CLayer* GetLayerById(WORD dwId) const;
	CLayer* GetLayerByGeom(CGeom* pGeom) const;
	CLayer* GetByName(CString strName) const;
	bool Replace(WORD wId, CLayer* pLayer);
	void Rematch();
	WORD ApplyId() const;
	void MinMaxLevel(BYTE& minLevelObj, BYTE& maxLevelObj, BYTE& minLevelTag, BYTE& maxLevelTag) const;

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseWeb(CFile& file, const CDatainfo& datainfo);
	virtual void ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo);
	virtual void ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool includeProperties);
	virtual void ReleaseWeb(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool includeProperties) const;

	virtual void ReleaseWvt(CFile& file, const CDatainfo& datainfo);
	virtual void ReleaseWvt(BinaryStream& stream, const CDatainfo& datainfo) ;
	virtual void ReleaseWvt(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel);
	virtual void ReleaseWvt(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel);
	virtual bool Releasable(CLayer* layer) const;

	//	void ReplaceSpot();
	void ExportGrid(CString strGrid, CSize docGrid, CSize docOffset, CSize docCanvas);
	void ExportPCGeom(CString strGeom);
	void ExportCEGeom(CString strGeom);

	friend class CLayer;
};