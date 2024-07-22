#pragma once

using namespace std;

#include <string>
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
}
class BinaryStream;

enum LICENCE
{
	PUBLISHING = 0X10000000,
	DATABASEMAP = 0X0100000,
	DATABASEINFO = 0X00100000,
	MAPINTERNET = 0X00010000,
	BACKGROUND = 0X00001000
};

__declspec(dllexport) void ReverseOrder(WORD& x);
__declspec(dllexport) void ReverseOrder(DWORD& x);
__declspec(dllexport) void ReverseOrder(int& x);
__declspec(dllexport) void ReverseOrder(unsigned int& x);
__declspec(dllexport) void ReverseOrder(long& x);
__declspec(dllexport) void ReverseOrder(short& x);
__declspec(dllexport) void ReverseOrder(float& x);
__declspec(dllexport) double ReverseOrder(double x);
__declspec(dllexport) void ReverseOrder(CPoint* pPoints, unsigned int nPoints);

__declspec(dllexport) void AFXAPI AfxToRelatedPath(CString strDocPath, CString& strPath);
__declspec(dllexport) void AFXAPI AfxToFullPath(CString strPath, CString& strFile);
__declspec(dllexport) void AFXAPI AfxToServerPath(CString strSharedFold, CString& strPath);

//__declspec(dllexport) long AFXAPI round(float x);
//__declspec(dllexport) long AFXAPI round(double x);

__declspec(dllexport) DWORD AFXAPI AfxGetCurrentArVersion();
__declspec(dllexport) DWORD AFXAPI AfxGetSoftVersion();
__declspec(dllexport) DWORD AfxGetAuthorizationKey();
__declspec(dllexport) void AfxIncreaseCounter();
__declspec(dllexport) CStringA AFXAPI GetVersionInfo(HMODULE hModule, CStringA strEntry);

__declspec(dllexport) CString AfxGetProfileString(HKEY hkeySpace, LPCTSTR lpszRegisterKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
__declspec(dllexport) BOOL AfxWriteProfileString(HKEY hkeySpace, LPCTSTR lpszRegisterKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
__declspec(dllexport) DWORD AFXAPI PackStrPC(CString& string, CFile* pFile);
__declspec(dllexport) DWORD AFXAPI PackStrPC(CStringA& string, CFile* pFile);
__declspec(dllexport) UINT AFXAPI ReadStringLength(BYTE*& bytes);
__declspec(dllexport) CString AFXAPI UnpackStrPC(BYTE*& bytes);
__declspec(dllexport) OLECHAR* MultiByteToUnicode(UINT CodePage, const char* szA);
__declspec(dllexport) char* AFXAPI UnicodeToMultiByte(WCHAR* string);
__declspec(dllexport) OLECHAR* UTF8ToUnicode(const char* szA);
__declspec(dllexport) void AFXAPI SerializeStrCE(CArchive& ar, CString string);
__declspec(dllexport) void AFXAPI SerializeStrCE(CArchive& ar, CStringA string);
__declspec(dllexport) DWORD AFXAPI PackStrCE(const CString& string, CFile& file);
__declspec(dllexport) void SaveAsAscii(CFile& file, const char* string);
__declspec(dllexport) void SaveAsUTF8(CFile& file, CString string);
__declspec(dllexport) void SaveAsUTF8(CFile& file, CStringA string);
__declspec(dllexport) void SaveAsUTF8(BinaryStream& stream, CString string);
__declspec(dllexport) void SaveAsUTF8(BinaryStream& stream, CStringA string);
__declspec(dllexport) void SaveAsEscapedASCII(boost::json::object& json, const char* name, CString string);
__declspec(dllexport) std::string StringToEscapedASCII(CString string);
__declspec(dllexport) BOOL DeleteDirectory(LPCTSTR DirName);
__declspec(dllexport) CString ConvertToOctalString(CString string);
__declspec(dllexport) float PointToPixel(float numeral, Gdiplus::Graphics& g);
__declspec(dllexport) void PPtoDPtoLP(Gdiplus::Graphics& g, CSize& size);
__declspec(dllexport) void DPtoLP(Gdiplus::Graphics& g, CSize& size);
__declspec(dllexport) BYTE* CStringToUTF8(const CString& string, unsigned short& nByteCount);
__declspec(dllexport) BYTE* CStringAToUTF8(const CStringA& string, unsigned short& nByteCount);
__declspec(dllexport) DWORD PlayMCI(CString strFile);
__declspec(dllexport) int GetIndexFromOBList(CObList& oblist, const CObject* object);
__declspec(dllexport) BYTE FAR* Compress(BYTE FAR* pBytes, unsigned long length, bool base64, unsigned long FAR& newLength);

__declspec(dllexport) DWORD HextoDec(CString strHex);
__declspec(dllexport) CString DectoHex(int nDec);

__declspec(dllexport) CRect InflateRect(CRect rect, int cx, int cy);
__declspec(dllexport) void DrawGdiplusBitmapOnGDI(CDC& dc, CPoint& point, Gdiplus::Bitmap* bitmap);
__declspec(dllexport) void DrawGDIHBitmapOnGDI(CDC& dc, CPoint& point, HBITMAP bitmap);
__declspec(dllexport) HBITMAP CaptureScreen(CWnd* pWnd, CRect& rect);
__declspec(dllexport) HBITMAP CaptureScreen(CDC& dc, CRect& rect);
__declspec(dllexport) void stringJSON(std::string& input);

__declspec(dllexport) std::string EscapedASCII(CString string);
__declspec(dllexport) std::string escapeJSON(const std::string& input);

template<typename T>
bool Within(const T& x, const T& x1, const T& x2)
{
	return (x>=x1&&x<=x2)||(x>=x2&&x<=x1);
}

#define roundf(value, decimal) (float)((int)(value*decimal))/decimal
