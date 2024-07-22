#pragma once

#define WM_HIGHLIGHTATT   (WM_USER + 501)
#define WM_FLASHGEOM   (WM_USER + 502)
#define WM_COMBOBOXSELCHANGE (WM_USER + 503)
#define WM_POUACTIVATED  (WM_USER + 504)

typedef struct PLACE
{
	WORD wMap;
	WORD wLayer;
	DWORD dwGeom;

	double dLng;
	double dLat;

	PLACE()
	{
		wMap = -1;
		wLayer = -1;
		dwGeom = -1;

		dLng = 0.0f;
		dLat = 0.0f;
	}
} PLACE;

__declspec(dllexport) void ParseWhere(CString strWhere, CString& strFields, CString& strValues);
__declspec(dllexport) CString AfxGetODBCConnectionString(CDatabase* pDatabase);
