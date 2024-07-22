#include "stdafx.h"
#include "Mapdcom.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

//#include <atlstr.h>
#include "..\Socket\MapSinkSocket.h"
#include "../Information/ODBCDatabase.h"
#include "../Atlas/Atlas1.h"
#include "../Atlas/MapItem.h"
 
CAtlas* IMapdcom::m_pAtlas = nullptr;

IMapdcom::IMapdcom()
{
	InitializeCriticalSection(&csAtlas);

	EnterCriticalSection(&csAtlas);
	if(m_pAtlas == nullptr)
	{
		this->OpenAtlas(L"China2005");
	}
	LeaveCriticalSection(&csAtlas);
}

IMapdcom::~IMapdcom()
{
	EnterCriticalSection(&csAtlas);
	if(m_pAtlas != nullptr)
	{
		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;
		
		delete m_pAtlas;
		m_pAtlas = nullptr;
	}
	LeaveCriticalSection(&csAtlas);

	DeleteCriticalSection(&csAtlas);
}

STDMETHODIMP IMapdcom::OpenAtlas(BSTR bstrAtlas)
{
	EnterCriticalSection(&csAtlas);
	
	if(m_pAtlas != nullptr)
	{
		LeaveCriticalSection(&csAtlas);
		return S_OK;
	}
	else
	{		
		CString strProject(bstrAtlas);

		CString strMapPath = this->GetProfileString(strProject,_T("Map"),_T("Path"),nullptr);
		if(strMapPath.IsEmpty() == FALSE)
		{
			if(_taccess(strMapPath,00) == -1)
			{
			//	CString string;
			//	string.Format(_T("软件无法找到 %s 路径!\n不能使用本软件!"),strMapPath);
			//	AfxMessageBox(string);
		
				LeaveCriticalSection(&csAtlas);
				return S_FALSE;
			}

			SetCurrentDirectory(strMapPath);

			CAtlas* pAtlas = new CAtlas;

			CAtlString  strAtlas(bstrAtlas);
			strAtlas += _T(".Atl");
			if(pAtlas->Open(strAtlas,CDatabase::openReadOnly) == false)
			{
				delete pAtlas;
				pAtlas = nullptr;

			//	AfxMessageBox("End");
			//	AfxMessageBox(_T("工程文件不存在或不能读，不能运行!"));

				LeaveCriticalSection(&csAtlas);
				return S_FALSE;
			}
			
			POSITION pos = pAtlas->m_PageItemList.GetHeadPosition();
			while(pos != nullptr)
			{
				CPageItem* pPageItem = pAtlas->m_PageItemList.GetNext(pos);
				if(pPageItem->m_wType == CPageItem::typeMap)
				{
					pPageItem->Open();
				}
			}

			m_pAtlas = pAtlas;

			LeaveCriticalSection(&csAtlas);
			return S_OK;
		}
		else
		{
			LeaveCriticalSection(&csAtlas);
			return S_FALSE;
		}
	}
}

BYTE* IMapdcom::OpenMap(USHORT wMapId, DWORD& dwLength)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pPageItem = m_pAtlas->GetItem(wMapId);
		if(pPageItem == nullptr)
		{
			return nullptr;
		}
		
		if(pPageItem->m_pMapItem != nullptr)
		{
			dwLength = pPageItem->m_pMapItem->m_varHead.parray->rgsabound[0].cElements;

			BYTE* pHead = new BYTE[16+dwLength]; 
			
			void* pSour;
			SafeArrayAccessData(pPageItem->m_pMapItem->m_varHead.parray, &pSour);
			
			memcpy(pHead+16,pSour,dwLength); 

			SafeArrayUnaccessData(pPageItem->m_pMapItem->m_varHead.parray);

			return pHead;
		}
	}

	return nullptr;
}

STDMETHODIMP IMapdcom::GetMapData(WORD wMapId, WORD nZoom, long left, long top, long right, long bottom, VARIANT* varData,CObList& vestigelist)
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
			return pPageItem->m_pMapItem->GetData(nZoom,CRect(left,top,right,bottom),varData,vestigelist);
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::GetMapId(BSTR bstrMap, USHORT* wMapId)
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

STDMETHODIMP IMapdcom::GetParentMap(USHORT wMapId, USHORT* wParentMapId)
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

STDMETHODIMP IMapdcom::GetSubMap(USHORT wMapId, DOUBLE X, DOUBLE Y, USHORT* wSubMapId)
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

STDMETHODIMP IMapdcom::MatchMap(DOUBLE fLng, DOUBLE fLat, USHORT* wMapId)
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

CString IMapdcom::GetProfileString(LPCTSTR lpszProject, LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault)
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

UINT IMapdcom::GetProfileInt(LPCTSTR lpszProject,LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault)
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

void IMapdcom::CreateVestigeList(WORD wMapId,CObList& oblist)
{
	if(m_pAtlas != nullptr)
	{
		CPageItem* pPageItem = m_pAtlas->GetItem(wMapId);
		if(pPageItem == nullptr)
		{
			return;
		}
		if(pPageItem->m_pMapItem != nullptr)
		{
			for(CTree<CLayer>::forwiterator it = pPageItem->m_pMapItem->m_layertree.forwbegin(); it!=pPageItem->m_pMapItem->m_layertree.forwend();++it)
			{
				CLayer* layer = *it;
				CMapSinkSocket::CLayerVestige* vestige = new CMapSinkSocket::CLayerVestige;
				vestige->wLayerID = layer->m_wId;
				oblist.AddTail(vestige);
			}
		}
	}
}