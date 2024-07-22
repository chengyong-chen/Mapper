#pragma once


class CLayerTree;
__declspec(dllexport) BOOL ExportAI(CLayerTree& layertree, const CDatainfo& datainfo, LPCTSTR pathname, const unsigned int& tolerance);
