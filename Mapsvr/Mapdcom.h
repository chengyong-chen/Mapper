#pragma once

#include "resource.h"       // main symbols
#include "..\Socket\MapSinkSocket.h"
class CAtlas;

/////////////////////////////////////////////////////////////////////////////
// IMapdcom
class IMapdcom 
{
public:
	IMapdcom();
	~IMapdcom();


protected:
	static CAtlas* m_pAtlas;
	CRITICAL_SECTION csAtlas;

public:
	CString GetProfileString(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault = nullptr);
	UINT GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault);

public:
	STDMETHODIMP OpenAtlas(BSTR bstrAtlas);
	BYTE* OpenMap(WORD wMapId,DWORD& nLength);
	STDMETHODIMP GetMapData(WORD wMapId, WORD nZoom, long left, long top, long right, long bottom, VARIANT* varData,CObList& vestigelist);
	STDMETHODIMP GetMapId(BSTR bstrMap, WORD* wMapId);
	STDMETHODIMP GetParentMap(WORD wMapId, WORD* wParentMapId);
	STDMETHODIMP GetSubMap(WORD wMapId, double X, double Y, WORD* wSubMapId);
	STDMETHODIMP MatchMap(double fLng, double fLat, WORD* wMapId);

	void CreateVestigeList(WORD wMapId,CObList& oblist);
};
