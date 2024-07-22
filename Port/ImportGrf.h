#pragma once

#include "Import.h"
class CLayer;
class CLayerTree;

class __declspec(dllexport) CImportGrf : public CImport
{
public:
	CImportGrf(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomID);

public:
	virtual BOOL Import(LPCTSTR lpszShapePath, CLayer* pLayer);
};
