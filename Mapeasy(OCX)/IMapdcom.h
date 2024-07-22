#pragma once

#include "..\Socket\MapClientSocket.h"

class IMapdcom
{
public:
	IMapdcom(void);
	virtual ~IMapdcom(void);

public:
	STDMETHODIMP OpenAtlas(BSTR bstrAtlas);
	STDMETHODIMP OpenMap(WORD wMapId, VARIANT* varHead);
	STDMETHODIMP GetMapData(WORD wMapId, WORD nZoom, long left, long top, long right, long bottom, VARIANT* varData);
	STDMETHODIMP GetMapId(BSTR bstrMap, USHORT* wMapId);
	STDMETHODIMP GetParentMap(WORD wMapId, WORD* wParentMapId);
	STDMETHODIMP GetSubMap(USHORT wMapId, double X, double Y, WORD* wSubMapId);
	STDMETHODIMP MatchMap(double fLng, double fLat, WORD* wMapId);

public:
	CMapClientSocket m_socket;
	bool Connect(CString strServer,int nPort,CWnd* pParent);
	void Close();
public:
	STDMETHODIMP Longin(CString strUser,CString strPWD);
public:
	static HANDLE m_hLoged;
	static HANDLE m_hAtlasOpened;
	static HANDLE m_hMapOpened;
	static HANDLE m_hMapData;
	static HANDLE m_hGetMapId;
	static HANDLE m_hGetParentMap;
	static HANDLE m_hGetSubMap;
	static HANDLE m_hMatchMap;
	

public:
	static VARIANT m_varHead;
	static VARIANT m_varData;
	static WORD m_wMapId;
	static WORD m_wParentMapId;
	static WORD m_wSubMapId;
	static WORD m_wMatchMapId;
	
public:
	static CRITICAL_SECTION csHead;
	static CRITICAL_SECTION csData;

public:
	static bool m_bAtlasOpened;
public:
	static void CallbackRecvFromSocket(void* lp,CMapClientSocket* pSocket,DWORD dwParent);
};
