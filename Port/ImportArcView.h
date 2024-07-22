#pragma once

#include "Global.h"

#include "Import.h"
#include "..\..\Thirdparty\ShapeLib\shapefil.h"

class CLayerTree;

class __declspec(dllexport) CImportArcView : public CImport
{
public:
	CImportArcView(CDatainfo& datainfo);
	
public:
	void ReadLayer(CLayer* layer,LPCTSTR lpszShape,CDaoDatabase* pDataBase);
	void SetRecordset(CDaoRecordset& rsMDB,CRecordset& rsDBF,CGeom* geom);
};