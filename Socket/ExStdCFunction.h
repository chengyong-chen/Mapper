#pragma once

//#include <set>
// dll file
#if defined _WINDLL
#define  STD_DLL_FUNCTION		extern "C"  __declspec(dllexport)
#define  AFX_EXT_DLL_FUNCTION   __declspec(dllexport)
// exe file.
#else
#define  STD_DLL_FUNCTION  extern "C"  __declspec(dllimport)
#define  AFX_EXT_DLL_FUNCTION   __declspec(dllimport)
#endif
/////////////////////

class CMessageReceived;

STD_DLL_FUNCTION DWORD GetUserId();
STD_DLL_FUNCTION void Log(LPCSTR szText);
STD_DLL_FUNCTION int GetLocalHostInfos(CString& host, CString& name, CString& aliase, CString& ip);

AFX_EXT_DLL_FUNCTION int NormalBlockingHook(void);
AFX_EXT_DLL_FUNCTION CString GetCurrentDir();
AFX_EXT_DLL_FUNCTION bool BrowseForFolder(CString& sFolderPath, CString sTitle);
AFX_EXT_DLL_FUNCTION void GetSysRegisteredInfor(CString& strInfor);
AFX_EXT_DLL_FUNCTION void SetMsgOutHandle(HWND hWnd);
AFX_EXT_DLL_FUNCTION bool VariantTimeToYYMMDDhhmmString(DATE vDate, CString& strDate);
AFX_EXT_DLL_FUNCTION bool WriteReceivedMsgToDataBase(CMessageReceived* pMsg, ADOCG::_ConnectionPtr& pDBConnect);
AFX_EXT_DLL_FUNCTION bool OpenCenterDataBase(ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR strConnectionString = nullptr, LPCTSTR lpszUserName = nullptr, LPCTSTR lpszPassWord = nullptr);
AFX_EXT_DLL_FUNCTION bool CloseCenterDatabase(const ADOCG::_ConnectionPtr& pDBConnect);
AFX_EXT_DLL_FUNCTION void ImportRouteFileToDatabase(const ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR lpszSourceFile, DWORD dwIdParam);
AFX_EXT_DLL_FUNCTION void Log(const CMessageReceived& msg);
AFX_EXT_DLL_FUNCTION UINT GetRecordsCount(LPCTSTR lpszTableName, const ADOCG::_ConnectionPtr& pDBConnec);
AFX_EXT_DLL_FUNCTION void Log_or_AfxMessageBox(const _bstr_t& bsText, UINT uType = MB_OK);
AFX_EXT_DLL_FUNCTION void Log_or_AfxMessageBox(LPCTSTR szText, UINT uType = MB_OK);
