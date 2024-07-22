// Socket.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "ExStdCFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE SocketDLL = {FALSE, nullptr};

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if(dwReason==DLL_PROCESS_ATTACH)
	{
		TRACE0("SOCKET.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if(!AfxInitExtensionModule(SocketDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(SocketDLL);
	}
	else if(dwReason==DLL_PROCESS_DETACH)
	{
		TRACE0("SOCKET.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(SocketDLL);
	}
	return 1; // ok
}

int NormalBlockingHook(void)
{
	MSG msg; /* lets us pull messages via PeekMessage */
	const int ret = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

	if(ret)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return ret;
}

// 得到当前程序的路径
CString GetCurrentDir()
{
	CString str;
	TCHAR buff[MAX_PATH];

	memset(buff, 0, MAX_PATH);
	//GetModuleFileName(AfxGetInstanceHandle(),buff,sizeof(buff)); 
	GetModuleFileName(nullptr, buff, sizeof(buff));

	str = buff;
	str = str.Left(str.ReverseFind(_T('\\')));

	return str;
}

//显示选择文件夹窗口
//参数 sFolderPath：用于返回用户选择的文件夹的路径
//参数 sTitle：用于指定选择文件夹窗口的标题
//返回值 ：操作结果，用户取消选择或操作失败返回false，否则true
bool BrowseForFolder(CString& sFolderPath, CString sTitle)
{
	BROWSEINFO bi;
	TCHAR Buffer[_MAX_PATH];
	//初始化入口参数bi
	bi.hwndOwner = nullptr;
	bi.pidlRoot = nullptr;
	bi.pszDisplayName = Buffer;
	bi.lpszTitle = sTitle;
	bi.ulFlags = 0;
	bi.lpfn = nullptr;
	const LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);
	//用户取消选择
	if(!pIDList)
		return false;
	SHGetPathFromIDList(pIDList, Buffer);
	sFolderPath = Buffer;
	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc)))
		return false;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();

	return true;
}
