#pragma once

enum FilterType : BYTE
{
	None = 0,
};

__declspec(dllexport) void AFXAPI operator <<(CArchive& ar, FilterType& filter);
__declspec(dllexport) void AFXAPI operator >>(CArchive& ar, FilterType& filter);
