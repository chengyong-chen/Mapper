#pragma once

#include "Import.h"

class CGeom;
class CText;
class CLayerTree;
class __declspec(dllexport) CImportDxf : public CImport
{
public:
	CImportDxf(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId);

public:
	bool ReadLayerSection(FILE* file, CLayerTree& layertree) const;
	bool ReadData(FILE* file, CLayerTree& layertree);

protected:
	BOOL ReadMark(FILE* file, CLayerTree& layertree);
	BOOL ReadLine(FILE* file, CLayerTree& layertree);
	BOOL ReadVertex(FILE* file, CPoint& point) const;
	BOOL ReadPoly(FILE* file, CLayerTree& layertree);
	BOOL ReadText(FILE* file, CLayerTree& layertree, float fPointToDoc);

public:
	BOOL Import(LPCTSTR lpszPath) override;
};
