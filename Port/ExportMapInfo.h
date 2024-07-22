#pragma once

class CLayerList;
__declspec(dllexport) BOOL ExportMapInfoTab(const CDatainfo& datainfo, CLayerList* playerlist, LPCTSTR lpszShapePath, const unsigned int& tolerance);
__declspec(dllexport) BOOL ExportMapInfoMif(const CDatainfo& datainfo, CLayerList* playerlist, LPCTSTR lpszShapePath, const unsigned int& tolerance);
