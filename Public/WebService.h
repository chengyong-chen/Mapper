#pragma once

using namespace std;
#include <functional>
#include <variant>
#include <string>
#include <map>

class CHttpFile;

class __declspec(dllexport) CWebService
{
public:
	static CString MakeJsonRequestHeadString(CString client, CString username, CString password, int length);
	static CString MakeSOAPRequestHeadString(int length);
	static void CallService(std::string url, CString tenant, CString username, CString password, std::string payload, function<void(CString)> failed, function<void(std::string)> success);
	static void CallService(std::string url, CString tenant, CString username, CString password, std::map<CString, std::variant<std::string, int>> parameters, std::map<CString, CString> files, function<void(CString)> failed, function<void(std::string)> success);
	static void CallService(std::string url, CString tenant, CString username, CString password, std::map<CString, std::variant<std::string, int>> parameters, std::vector<std::tuple<CStringA, const BYTE*, int>> bytes, function<void(CString)> failed, function<void(std::string)> success);
	static char* BitmapToString64(Gdiplus::Bitmap* bitmap);
		

private:
	static void CallService(std::string url, CString client, CString username, CString password, function<void(CString)> failed, function<void(std::string)> success, function<void(CHttpFile*)> contenttype, function<int()> buffersize, function<void(CHttpFile*)> writeContent);
	static char* StringToMultiByte(CString str);
	static int GetMultiByteLength(CString str);
	static int GetMultiByteLength(std::variant<std::string, int> var);
	static std::string VariantToString(std::variant<std::string, int> var);
	static char* GetFileMimeType(HANDLE hFile);
	static char* CreateBoundary();
	static char* Unicode2Ansi(LPCWSTR szStr);
	static bool IsInternetAvailable();
};
