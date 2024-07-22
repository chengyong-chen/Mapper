#pragma once

#include "resource.h"       // main symbols

class CAtlas;

/////////////////////////////////////////////////////////////////////////////
// CMapdcom
class ATL_NO_VTABLE CMapdcom : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMapdcom, &CLSID_CMapdcom>,
	public IDispatchImpl<IMapdcom, &IID_IMapdcom, &LIBID_MAPSVRLib>
{
public:
	CMapdcom()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MAPDCOM)

DECLARE_PROTECT_FINAL_CONSTRUCT()

DECLARE_CLASSFACTORY_AUTO_THREAD()				//needed by auto-thread

DECLARE_NOT_AGGREGATABLE(CMapdcom)	//needed by auto-thread

BEGIN_COM_MAP(CMapdcom)
	COM_INTERFACE_ENTRY(IMapdcom)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

protected:
	static CAtlas* m_pAtlas;

public:
	static CString GetProfileString(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault = nullptr);
	static UINT GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault);

	STDMETHOD(OpenAtlas)(BSTR bstrAtlas);
	STDMETHOD(OpenMap)(USHORT wMapId, VARIANT* varHead);
	STDMETHOD(GetMapData)(USHORT wMapId, USHORT nZoom, LONG left, LONG top, LONG right, LONG bottom, VARIANT* varData);
	STDMETHOD(GetMapId)(BSTR bstrMap, USHORT* wMapId);
	STDMETHOD(GetParentMap)(USHORT wMapId, USHORT* wParentMapId);
	STDMETHOD(GetSubMap)(USHORT wMapId, DOUBLE X, DOUBLE Y, USHORT* wSubMapId);
	STDMETHOD(MatchMap)(DOUBLE fLng, DOUBLE fLat, USHORT* wMapId);
};