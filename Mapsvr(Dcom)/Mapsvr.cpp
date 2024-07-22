// Mapsvr.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f Mapsvrps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "Mapsvr.h"

#include "Mapsvr_i.c"
#include "Mapdcom.h"


const DWORD dwTimeOut = 50000; // time for EXE to be idle before shutting down
const DWORD dwPause = 10000; // time to wait for threads to finish up

// Passed to CreateThread to monitor the shutdown event
static DWORD WINAPI MonitorProc(void* pv)
{
    CExeModule* p = (CExeModule*)pv;
    p->MonitorShutdown();
    return 0;
}

LONG CExeModule::Unlock()
{
    //LONG l = CComModule::Unlock();
	LONG l = CComAutoThreadModule<>::Unlock();	//needed by auto-thread
    if(l == 0)
    {
        bActivity = true;
        SetEvent(hEventShutdown); // tell monitor that we transitioned to zero
    }
    return l;
}

//Monitors the shutdown event
void CExeModule::MonitorShutdown()
{
    while (1)
    {
        WaitForSingleObject(hEventShutdown, INFINITE);
        DWORD dwWait=0;
        do
        {
            bActivity = false;
            dwWait = WaitForSingleObject(hEventShutdown, dwTimeOut);
        } while (dwWait == WAIT_OBJECT_0);
        // timed out
        if(!bActivity && m_nLockCnt == 0) // if no activity let's really bail
        {
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
            CoSuspendClassObjects();
            if(!bActivity && m_nLockCnt == 0)
#endif
                break;
        }
    }
    CloseHandle(hEventShutdown);
    PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
}

bool CExeModule::StartMonitor()
{
    hEventShutdown = CreateEvent(nullptr, false, false, nullptr);
    if(hEventShutdown == nullptr)
        return false;
    DWORD dwThreadID;
    HANDLE h = CreateThread(nullptr, 0, MonitorProc, this, 0, &dwThreadID);
    return (h != nullptr);
}

CExeModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_CMapdcom, CMapdcom)
END_OBJECT_MAP()


LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
    while (p1 != nullptr && *p1 != nullptr)
    {
        LPCTSTR p = p2;
        while (p != nullptr && *p != nullptr)
        {
            if(*p1 == *p)
                return CharNext(p1);
            p = CharNext(p);
        }
        p1 = CharNext(p1);
    }
    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////
//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
    lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT

#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
    HRESULT hRes = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#else
    HRESULT hRes = CoInitialize(nullptr);
#endif
    _ASSERTE(SUCCEEDED(hRes));
    _Module.Init(ObjectMap, hInstance, &LIBID_MAPSVRLib);

    _Module.dwThreadID = GetCurrentThreadId();
    TCHAR szTokens[] = _T("-/");

    int nRet = 0;
    BOOL bRun = TRUE;
    LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
    while (lpszToken != nullptr)
    {
        if(lstrcmpi(lpszToken, _T("UnregServer"))==0)
        {
            _Module.UpdateRegistryFromResource(IDR_MAPSVR, FALSE);
            nRet = _Module.UnregisterServer(TRUE);
            bRun = FALSE;
            break;
        }
        if(lstrcmpi(lpszToken, _T("RegServer"))==0)
        {
            _Module.UpdateRegistryFromResource(IDR_MAPSVR, TRUE);
            nRet = _Module.RegisterServer(TRUE);
            bRun = FALSE;
            break;
        }
        lpszToken = FindOneOf(lpszToken, szTokens);
    }

    if(bRun)
    {
		::CoInitializeSecurity(nullptr,-1,nullptr,nullptr,RPC_C_AUTHN_LEVEL_NONE,RPC_C_IMP_LEVEL_IMPERSONATE,nullptr,EOAC_NONE,nullptr); 
	//	SOLE_AUTHENTICATION_SERVICE pacAuth;
	//	pacAuth.dwAuthnSvc = RPC_C_AUTHN_WINNT;
	//	pacAuth.dwAuthzSvc = RPC_C_AUTHZ_NAME;
	//	pacAuth.pPrincipalName = nullptr;
	//	pacAuth.hr = S_OK;
	//	::CoInitializeSecurity(nullptr,1,&pacAuth,nullptr,RPC_C_AUTHN_LEVEL_CONNECT,RPC_C_IMP_LEVEL_IMPERSONATE,nullptr,EOAC_NONE,nullptr); 
			
        _Module.StartMonitor();

#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
        hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);
        _ASSERTE(SUCCEEDED(hRes));
        hRes = CoResumeClassObjects();
#else
        hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
#endif
        _ASSERTE(SUCCEEDED(hRes));

        MSG msg;
        while (GetMessage(&msg, 0, 0, 0))
            DispatchMessage(&msg);

        _Module.RevokeClassObjects();
        Sleep(dwPause); //wait for any threads to finish
    }

    _Module.Term();
    CoUninitialize();
    return nRet;
}
