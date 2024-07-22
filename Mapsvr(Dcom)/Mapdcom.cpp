#include "stdafx.h"
#include "Mapsvr.h"
#include "Mapdcom.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <AtlStr.h>
#include "../Information/ODBCDatabase.h"
#include "../Atlas/Atlas1.h"
#include "../Atlas/MapItem.h"

extern __declspec(dllimport) IMPLEMENT_SERIAL(CPageItem, CObject, 0)
 
CAtlas* CMapdcom::m_pAtlas = nullptr;

STDMETHODIMP CMapdcom::OpenAtlas(BSTR bstrAtlas)
{
	if(m_pAtlas != nullptr)
	{
	//	AfxMessageBox("Alt OK");
		return S_OK;
	}
	else
	{	
		CAtlString strProject = bstrAtlas;
		CAtlString strMapPath = this->GetProfileString(strProject,_T("Map"),_T("Path"),nullptr);
		if(strMapPath.IsEmpty() == FALSE)
		{
			if(_taccess(strMapPath,00) == -1)
			{
			//	CString string;
			//	string.Format(_T("软件无法找到 %s 路径!\n不能使用本软件!"),strMapPath);
			//	AfxMessageBox(string);
				return S_FALSE;
			}

			SetCurrentDirectory(strMapPath);

			m_pAtlas = new CAtlas;

			CAtlString strAtlas = bstrAtlas;
			strAtlas += _T(".Atl");
			if(m_pAtlas->Open(strAtlas,CDatabase::openReadOnly) == false)
			{
				delete m_pAtlas;
				m_pAtlas = nullptr;

			//	AfxMessageBox("End");
			//	AfxMessageBox(_T("工程文件不存在或不能读，不能运行!"));
				return S_FALSE;
			}
			
			POSITION pos = m_pAtlas->m_PageItemList.GetHeadPosition();
			while(pos != nullptr)
			{
				CPageItem* pPageItem = m_pAtlas->m_PageItemList.GetNext(pos);
				if(pPageItem->m_wType == CPageItem::typeMap)
				{
					pPageItem->Open();
				}
			}
			
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
}

STDMETHODIMP CMapdcom::OpenMap(USHORT wMapId, VARIANT* varHead)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pPageItem = m_pAtlas->GetItem(wMapId);
		if(pPageItem == nullptr)
		{
			return S_FALSE;
		}
		
		if(pPageItem->m_pMapItem != nullptr)
		{
			SAFEARRAYBOUND bound;
			bound.cElements = pPageItem->m_pMapItem->m_varHead.parray->rgsabound[0].cElements; 
			bound.lLbound = 0;

			varHead->vt = VT_UI1 | VT_ARRAY;
			varHead->parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

			void* pSour;
			SafeArrayAccessData(pPageItem->m_pMapItem->m_varHead.parray, &pSour);

			void* pDest;
			SafeArrayAccessData(varHead->parray, &pDest);
			
			memcpy(pDest,pSour,bound.cElements); 

			SafeArrayUnaccessData(varHead->parray);

			SafeArrayUnaccessData(pPageItem->m_pMapItem->m_varHead.parray);

		//	*varHead = pPageItem->m_pMapItem->m_varHead;
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CMapdcom::GetMapData(USHORT wMapId, USHORT nZoom, LONG left, LONG top, LONG right, LONG bottom, VARIANT* varData)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pPageItem = m_pAtlas->GetItem(wMapId);
		if(pPageItem == nullptr)
		{
			return S_FALSE;
		}
		else if(pPageItem->m_pMapItem != nullptr)
		{
			return pPageItem->m_pMapItem->GetData(nZoom,CRect(left,top,right,bottom),varData);
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP CMapdcom::GetMapId(BSTR bstrMap, USHORT* wMapId)
{
	if(m_pAtlas != nullptr)
	{
		CString strMap(bstrMap);
		CPageItem* pPageItem = m_pAtlas->GetItemByMapName(strMap);
		if(pPageItem != nullptr)
		{
			*wMapId = pPageItem->m_wId;
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CMapdcom::GetParentMap(USHORT wMapId, USHORT* wParentMapId)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pParentItem = m_pAtlas->GetParentMap(wMapId);
		if(pParentItem != nullptr)
		{
			if(pParentItem->m_bLogistic == TRUE)
			{
				POSITION pos = m_pAtlas->m_PageItemList.Find(pParentItem);
				while(pos != nullptr)
				{
					CPageItem* pItem = m_pAtlas->m_PageItemList.GetPrev(pos);
					if(pItem->m_strFile == pParentItem->m_strFile && pItem->m_bLogistic == false)
					{
						pParentItem = pItem;
						break;
					}
				}
			}

			*wParentMapId = pParentItem->m_wId;
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CMapdcom::GetSubMap(USHORT wMapId, DOUBLE X, DOUBLE Y, USHORT* wSubMapId)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pSubItem = m_pAtlas->GetSubItem(wMapId,CPointF(X,Y));
		if(pSubItem != nullptr)
		{
			*wSubMapId = pSubItem->m_wId;
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CMapdcom::MatchMap(DOUBLE fLng, DOUBLE fLat, USHORT* wMapId)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pItem = m_pAtlas->MatchMap(nullptr,CPointF(fLng,fLat));
		if(pItem != nullptr)
		{
			*wMapId = pItem->m_wId;
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

CString CMapdcom::GetProfileString(LPCTSTR lpszProject, LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault)
{
	if(lpszSection != nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"),lpszProject);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase + lpszSection,0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,nullptr, &dwSize) == ERROR_SUCCESS)
			{
				CString strValue;
				LONG lRet = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,(LPBYTE)strValue.GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
				strValue.ReleaseBuffer();
				if(lRet == ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return strValue;
				}
			}

			RegCloseKey(hKey);
		}
	}

	return lpszDefault;
}

UINT CMapdcom::GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault)
{
	if(lpszSection != nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"),lpszProject);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase + lpszSection,0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr,nullptr, &dwSize) == ERROR_SUCCESS)
			{
				DWORD dwValue;
				DWORD dwType;
				DWORD dwCount = sizeof(DWORD);
				LONG lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, &dwType,(LPBYTE)&dwValue, &dwCount);

				if(lResult == ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return dwValue;
				}
			}

			RegCloseKey(hKey);
		}
	}

	return nDefault;
}