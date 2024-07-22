#pragma once

#include "Import.h"

class CLayerTree;
class __declspec(dllexport) CImportArcInfo : public CImport
{
public:
	CImportArcInfo(CDatainfo& datainfo);

public:
	virtual BOOL Import(LPCTSTR lpszPath) override;
};

class __declspec(dllexport) CImportArcInfo250000 : public CImportArcInfo
{
public:
	CImportArcInfo250000(CDatainfo& datainfo);

public:
	virtual BOOL Import(LPCTSTR lpszPath) override;
};

class __declspec(dllexport) CImportArcInfo1000000 : public CImportArcInfo
{
public:
	CImportArcInfo1000000(CDatainfo& datainfo);

public:
	virtual BOOL Import(LPCTSTR lpszPath) override;
};