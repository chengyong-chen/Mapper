#pragma once

#include "../layer/Layer.h"
class CLayerTree;
class CDatainfo;
class CViewExporter;
__declspec(dllexport) BOOL ExportEps(CLayerTree& layertree, const CDatainfo& datainfo, LPCTSTR pathname, const unsigned int& tolerance);
__declspec(dllexport) BOOL ExportPdf(CLayerTree& layertree, CViewExporter& viewinfo, LPCTSTR pathname, const unsigned int& tolerance);
__declspec(dllexport) CRect GetValidRect(CTree<CLayer>& layertree);
__declspec(dllexport) void GatherAllFonts(CTree<CLayer>& layertree, std::list<CStringA>& fontlist, bool bEnglish);
