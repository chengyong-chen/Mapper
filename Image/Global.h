#pragma once

__declspec(dllexport) HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal);
__declspec(dllexport) BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB);
__declspec(dllexport) BOOL WriteBme(CString szFile, HANDLE hDIB);
