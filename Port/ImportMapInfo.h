#pragma once

#include "Global.h"
#include "Import.h"

//#include "../../Thirdparty\Mitab\mitab_capi.h"

class CProjection;
class CSpot;
class CLine;
class CFill;
class CType;
class CDaoDatabase;
class CLayerTree;


class __declspec(dllexport) CImportMapInfo : public CImport
{
public:
	CImportMapInfo(CDatainfo& datainfo);

public:
	void ReadLayer(CLayer* layer,LPCTSTR lpszShapePath,CDaoDatabase* pDataBase);
	
public:
//	CLine* ReadStroke(mitab_feature feature);
//	CFill* ReadFill(mitab_feature feature);
//	CType* ReadType(mitab_feature feature);
};