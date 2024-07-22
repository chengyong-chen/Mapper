#pragma once

class CLayerTree;

__declspec(dllexport) BOOL ExportTxt(LPCTSTR pathname, CLayerTree& layertree, const Gdiplus::Matrix& matrix);
