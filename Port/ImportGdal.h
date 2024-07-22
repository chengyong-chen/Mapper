#pragma once
#include <list>
#include "Import.h"

class CProjection;
class CSpot;
class CLine;
class CFill;
class CHint;
class CDaoDatabase;
class CLibrary;

class GDALDataset;
class OGRFeatureDefn;
class OGRGeometry;
class OGRLineString;
class OGRLinearRing;
class OGRSpatialReference;
class OGRDataSource;
class OGRFeature;
class OGRStyleMgr;
class OGRPoint;
class CLayerTree;

class __declspec(dllexport) CImportGdal : public CImport
{
public:
	CImportGdal(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId);
	~CImportGdal() override;
public:
	int hiddenlevel = -1;
public:
	virtual BOOL Import(LPCTSTR lpszPath, CStringList& files, bool ignoreProjection, CString& dbname, bool into = false);
	virtual BOOL Import(CString& file, CString& dbname, bool into = false);
	virtual CProjection* GetMainProjection(LPCTSTR lpszFolder, CStringList& files);
	virtual CProjection* GetProjection(CString& strFile);
	virtual void ReadLayer(LPCTSTR lpszFile, CDaoDatabase* pDataBase, bool ignoreProjection, bool into = false);

public:
	CDaoDatabase* CreateDatabase(CString strPath, CString strName) const;

	static bool CreateTable(OGRFeatureDefn* poFDefn, CString strTable, CDaoDatabase* pDataBase);

	static std::list<std::pair<CString, CString>> GetAllFields(CDaoDatabase* pDataBase, CString strTable);

	static CString GetAIDField(CDaoDatabase* pDataBase, CString strTable);

	static CString GetNameField(CDaoDatabase* pDataBase, CString strTable);

	static CString GetGeocodeField(CDaoDatabase* pDataBase, CString strTable);

	static CString GetMiddleXField(CDaoDatabase* pDataBase, CString strTable);

	static CString GetMiddleYField(CDaoDatabase* pDataBase, CString strTable); 
private:
	CGeom* ReadGeometry(CLibrary& library, OGRGeometry* poGeometry, OGRStyleMgr* poStyleMgr, double fPointToDoc);
	CGeom* ReadPoint(OGRPoint* poGeometry, OGRStyleMgr* poStyleMgr, CLibrary& library, double fPointToDoc);
	CPoly* ReadLineString(OGRLineString* poLineString);
	CPoly* ReadLinearRing(OGRLinearRing* poLinearRing);
	CSpot* ReadSpot(OGRStyleMgr* poStyleMgr, CLibrary& library) const;
	CLine* ReadStroke(OGRStyleMgr* poStyleMgr) const;

	static CFillCompact* ReadFill(OGRStyleMgr* poStyleMgr);

public:
	static CProjection* NewProjFromOGRSpatialReference(OGRSpatialReference* Reference, double& offsetX, double& offsetY);
	static BOOL Support(CString strFile);
};