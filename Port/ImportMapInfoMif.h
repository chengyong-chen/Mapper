#pragma once

#include "Global.h"
#include "ImportMapInfo.h"

class CLayerTree;


class __declspec(dllexport) CImportMapInfoMif : public CImportMapInfo
{
public:
	CImportMapInfoMif(CDatainfo& datainfo);

public:
	virtual BOOL Import(LPCTSTR lpszPath) override;
	BOOL ReadMif(CLibrary& library,CLayer* layer,LPCTSTR lpszShapePath,CDaoDatabase* pDataBase);
	BOOL CreateTable(LPCTSTR lpszShapePath,CDaoDatabase* pDataBase);

public:
	virtual void ReadPen(CStdioFile& file,CGeom* pGeom) override;
	virtual void ReadBrush(CStdioFile& file,CGeom* pGeom) override;
	virtual void ReadSpot(CStdioFile& file,CGeom* pGeom) override;
	virtual void ReadType(CStdioFile& file,CGeom* pGeom) override;

private:
	void DistinguishLanduse(CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree);
	void DistinguishRoad(LPCTSTR lpszShapePath,CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree);
	void DistinguishAdmArea(CLayer* layer,CDaoDatabase* pMDBDataBase,CLayerTree& layertree);
};