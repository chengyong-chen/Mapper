// Mapeasy.cpp : CMapeasyApp 和 DLL 注册的实现。

#include "stdafx.h"
#include "Mapeasy.h"
#include "comcat.h" 
#include "objsafe.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMapeasyApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x8DFA0DD8, 0xD9EE, 0x4868, { 0xAE, 0xC8, 0xD, 0xF0, 0xAD, 0xA1, 0x4, 0x33 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CMapeasyApp::InitInstance - DLL 初始化

BOOL CMapeasyApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if(bInit)
	{
		// TODO: 在此添加您自己的模块初始化代码。
	}

	if(!AfxSocketInit())
	{
		AfxMessageBox("不支持Socket!");
	}

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	return bInit;
}



// CMapeasyApp::ExitInstance - DLL 终止
int CMapeasyApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return COleControlModule::ExitInstance();
}


// 创建组件种类 
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
    ICatRegister* pcr = nullptr ;
    HRESULT hr = S_OK ;

    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if(FAILED(hr))
        return hr;

    // Make sure the HKCR\Component Categories\{..catid...}
    // key is registered.
    CATEGORYINFO catinfo;
    catinfo.catid = catid;
    catinfo.lcid = 0x0409 ; // english

    // Make sure the provided description is not too long.
    // Only copy the first 127 characters if it is.
    int len = wcslen(catDescription);
    if(len>127)
        len = 127;
    wcsncpy(catinfo.szDescription, catDescription, len);
    // Make sure the description is null terminated.
    catinfo.szDescription[len] = '\0';

    hr = pcr->RegisterCategories(1, &catinfo);
    pcr->Release();

    return hr;
}

// 注册组件种类
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    // Register your component categories information.
    ICatRegister* pcr = nullptr ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if(SUCCEEDED(hr))
    {
       // Register this category as being "implemented" by the class.
       CATID rgcatid[1] ;
       rgcatid[0] = catid;
       hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
    }
    if(pcr != nullptr)
        pcr->Release();
    return hr;
}

// 卸载组件种类
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    ICatRegister* pcr = nullptr ;
    HRESULT hr = S_OK ;

    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if(SUCCEEDED(hr))
    {
       // Unregister this category as being "implemented" by the class.
       CATID rgcatid[1] ;
       rgcatid[0] = catid;
       hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
    }

    if(pcr != nullptr)
        pcr->Release();

    return hr;
}


// DllRegisterServer - 将项添加到系统注册表
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if(!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 将项从系统注册表中移除
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if(!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}