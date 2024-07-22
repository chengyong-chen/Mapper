#include "StdAfx.h"
#include "IMapdcom.h"
#include "..\Coding\Instruction.h"
#include "..\Socket\Balloon.h"

#include <comutil.h>
#pragma comment(lib, "comsupp.lib")

HANDLE IMapdcom::m_hLoged = nullptr;
HANDLE IMapdcom::m_hAtlasOpened = nullptr;
HANDLE IMapdcom::m_hMapOpened = nullptr;
HANDLE IMapdcom::m_hMapData = nullptr;
HANDLE IMapdcom::m_hGetMapId = nullptr;
HANDLE IMapdcom::m_hGetParentMap = nullptr;
HANDLE IMapdcom::m_hGetSubMap = nullptr;
HANDLE IMapdcom::m_hMatchMap = nullptr;

bool IMapdcom::m_bAtlasOpened = false;

VARIANT IMapdcom::m_varHead;
VARIANT IMapdcom::m_varData;
WORD IMapdcom::m_wMapId = 0;
WORD IMapdcom::m_wParentMapId = 0;
WORD IMapdcom::m_wSubMapId = 0;
WORD IMapdcom::m_wMatchMapId = 0;


CRITICAL_SECTION IMapdcom::csHead;
CRITICAL_SECTION IMapdcom::csData;

IMapdcom::IMapdcom(void)
{
	if(m_hLoged == nullptr)
	{
		m_hLoged = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}
	if(m_hAtlasOpened == nullptr)
	{
		m_hAtlasOpened = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}
	if(m_hMapOpened == nullptr)
	{
		m_hMapOpened = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}
	if(m_hMapData == nullptr)
	{
		m_hMapData = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	if(m_hGetMapId == nullptr)
	{
		m_hGetMapId = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	if(m_hGetParentMap == nullptr)
	{
		m_hGetParentMap = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	if(m_hGetSubMap == nullptr)
	{
		m_hGetSubMap = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	if(m_hMatchMap == nullptr)
	{
		m_hMatchMap = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	m_varHead.parray = nullptr;
	m_varData.parray = nullptr;

	m_bAtlasOpened = false;

	InitializeCriticalSection(&csHead);
	InitializeCriticalSection(&csData);
}

IMapdcom::~IMapdcom(void)
{
	if(m_hLoged != nullptr)
	{
		CloseHandle(m_hLoged);
		m_hLoged = nullptr;
	}

	if(m_hAtlasOpened != nullptr)
	{
		CloseHandle(m_hAtlasOpened);
		m_hAtlasOpened = nullptr;
	}

	if(m_hMapOpened != nullptr)
	{
		CloseHandle(m_hMapOpened);
		m_hMapOpened = nullptr;
	}

	if(m_hMapData != nullptr)
	{
		CloseHandle(m_hMapData);
		m_hMapData = nullptr;
	}

	if(m_hGetMapId != nullptr)
	{
		CloseHandle(m_hGetMapId);
		m_hGetMapId = nullptr;
	}
	if(m_hGetParentMap != nullptr)
	{
		CloseHandle(m_hGetParentMap);
		m_hGetParentMap = nullptr;
	}
	if(m_hGetSubMap != nullptr)
	{
		CloseHandle(m_hGetSubMap);
		m_hGetSubMap = nullptr;
	}
	if(m_hMatchMap != nullptr)
	{
		CloseHandle(m_hMatchMap);
		m_hMatchMap = nullptr;
	}

	DeleteCriticalSection(&csHead);
	DeleteCriticalSection(&csData);
}

bool IMapdcom::Connect(CString strServer,int nPort,CWnd* pParent)
{
	if(m_socket.ConnectToServer((LPTSTR)(LPCTSTR)strServer, nPort) == FALSE)
	{
		AfxMessageBox(_T("服务器连接失败!"));
		return false;
	}
	else
	{
		m_socket.SetCallbackFunction(CallbackRecvFromSocket);
		m_socket.SetParent(pParent);

		return true;
	}
}

void IMapdcom::Close()
{
	if(m_socket.IsConnectionValid() == TRUE)
	{
		STRUCT_CLIENT_LOGOFF* lp = new STRUCT_CLIENT_LOGOFF();
		m_socket.SendMsg((char*)lp, sizeof(*lp));	
	}
	
	m_socket.DisConnect();
}

STDMETHODIMP IMapdcom::Longin(CString strUser,CString strPWD)
{
	if(m_socket.IsLogin() == false)
	{
		ResetEvent(m_hLoged);

		STRUCT_CLIENT_LOGIN* lp = new STRUCT_CLIENT_LOGIN((LPTSTR)(LPCTSTR)strUser, (LPTSTR)(LPCTSTR)strPWD);
		m_socket.SendMsg((char*)lp, sizeof(*lp));	
		
		WaitForSingleObject(m_hLoged, 600000);
		
		return m_socket.IsLogin() == true ? S_OK : S_FALSE;
	}
	else
		return S_OK;
}


STDMETHODIMP IMapdcom::OpenAtlas(BSTR bstrAtlas)
{
	if(m_socket.IsLogin() == true)
	{
		ResetEvent(m_hAtlasOpened);

		char* lpAtlas = _com_util::ConvertBSTRToString(bstrAtlas);
		STRUCT_MAPCLIENT_OPENATLAS* lp = new STRUCT_MAPCLIENT_OPENATLAS((LPTSTR)lpAtlas);
		m_socket.SendMsg((char*)lp, sizeof(*lp));	
		
		WaitForSingleObject(m_hAtlasOpened, 600000);

		return m_bAtlasOpened == true ? S_OK : S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::OpenMap(WORD wMapId, VARIANT* varHead)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hMapOpened);

		STRUCT_MAPCLIENT_OPENMAP* lp = new STRUCT_MAPCLIENT_OPENMAP(wMapId);
		m_socket.SendMsg((char*)lp, sizeof(*lp));	
		
		if(WaitForSingleObject(m_hMapOpened, 600000) == WAIT_OBJECT_0)
		{
			EnterCriticalSection(&csHead);
			
			SAFEARRAYBOUND bound;
			bound.cElements = m_varHead.parray->rgsabound[0].cElements; 
			bound.lLbound = 0;

			varHead->vt = VT_UI1 | VT_ARRAY;
			varHead->parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

			void* pSour;
			SafeArrayAccessData(m_varHead.parray, &pSour);

			void* pDest;
			SafeArrayAccessData(varHead->parray, &pDest);
			
			memcpy(pDest,pSour,bound.cElements); 

			SafeArrayUnaccessData(varHead->parray);

			SafeArrayUnaccessData(m_varHead.parray);

			LeaveCriticalSection(&csHead);
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
	else
	{
		return S_FALSE;
	}
}

STDMETHODIMP IMapdcom::GetMapData(USHORT wMapId, USHORT nZoom, LONG left, LONG top, LONG right, LONG bottom, VARIANT* varData)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hMapData);

		STRUCT_MAPCLIENT_MAPDATA* lp = new STRUCT_MAPCLIENT_MAPDATA(wMapId,nZoom,left,top,right,bottom);
		m_socket.SendMsg((char*)lp, sizeof(*lp));	
		
		if(WaitForSingleObject(m_hMapData, 600000) == WAIT_OBJECT_0)
		{
			EnterCriticalSection(&csData);
			
			SAFEARRAYBOUND bound;
			bound.cElements = m_varData.parray->rgsabound[0].cElements; 
			bound.lLbound = 0;

			varData->vt = VT_UI1 | VT_ARRAY;
			varData->parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

			void* pSour;
			void* pDest;
			SafeArrayAccessData(m_varData.parray, &pSour);
			SafeArrayAccessData(varData->parray, &pDest);
			
			memcpy(pDest,pSour,bound.cElements); 

			SafeArrayUnaccessData(varData->parray);
			SafeArrayUnaccessData(m_varData.parray);

			LeaveCriticalSection(&csData);
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::GetMapId(BSTR bstrMap, USHORT* wMapId)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hGetMapId);

		char* lpMap = _com_util::ConvertBSTRToString(bstrMap);
		STRUCT_MAPCLIENT_GETMAPID* lp = new STRUCT_MAPCLIENT_GETMAPID(lpMap);
		m_socket.SendMsg((char*)lp, sizeof(STRUCT_MAPCLIENT_GETMAPID));	
		
		if(WaitForSingleObject(m_hGetMapId, 600000) == WAIT_OBJECT_0)
		{
			*wMapId = m_wMapId;
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::GetParentMap(USHORT wMapId, USHORT* wParentMapId)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hGetParentMap);

		STRUCT_MAPCLIENT_GETPARENTMAP* lp = new STRUCT_MAPCLIENT_GETPARENTMAP(wMapId);
		m_socket.SendMsg((char*)lp, sizeof(STRUCT_MAPCLIENT_GETPARENTMAP));	
		
		if(WaitForSingleObject(m_hGetParentMap, 600000) == WAIT_OBJECT_0)
		{
			*wParentMapId = m_wParentMapId;
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::GetSubMap(USHORT wMapId, DOUBLE X, DOUBLE Y, USHORT* wSubMapId)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hGetSubMap);

		STRUCT_MAPCLIENT_GETSUBMAP* lp = new STRUCT_MAPCLIENT_GETSUBMAP(wMapId,X,Y);
		m_socket.SendMsg((char*)lp, sizeof(STRUCT_MAPCLIENT_GETSUBMAP));	
		
		if(WaitForSingleObject(m_hGetSubMap, 600000) == WAIT_OBJECT_0)
		{
			*wSubMapId = m_wSubMapId;
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

STDMETHODIMP IMapdcom::MatchMap(DOUBLE fLng, DOUBLE fLat, USHORT* wMapId)
{
	if(m_socket.IsLogin() == true && m_bAtlasOpened == true)
	{
		ResetEvent(m_hMatchMap);

		STRUCT_MAPCLIENT_MATCHMAP* lp = new STRUCT_MAPCLIENT_MATCHMAP(fLng,fLat);
		m_socket.SendMsg((char*)lp, sizeof(STRUCT_MAPCLIENT_MATCHMAP));	
		
		if(WaitForSingleObject(m_hMatchMap, 600000) == WAIT_OBJECT_0)
		{
			*wMapId = m_wMatchMapId;
			return S_OK;
		}
		else
			return S_FALSE;
	}
	else
		return S_FALSE;
}

void IMapdcom::CallbackRecvFromSocket(void* lp,CMapClientSocket* pSocket,DWORD dwParent)
{
	if(lp == nullptr) 
		return;

	STRUCT_MSG_HEADER hdr;
	memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));

	switch(hdr.dwCmdId)
	{
	case SERVER_LOGIN_FAILED:
		{
			STRUCT_SERVER_REPLY_LOGIN msg;
			memcpy((char*)&msg, lp, hdr.dwTotalLen);

			pSocket->SetLogin(FALSE);
			pSocket->Terminate();

			SetEvent(m_hLoged);
		}
		break;
	case SERVER_LOGIN_SUCCEED:
		{
			pSocket->SetLogin(TRUE);

			SetEvent(m_hLoged);
		}
		break;
	case SERVER_LOGOFF:
		{
			pSocket->SetLogin(FALSE);
			pSocket->Terminate();
//			::SendMessage(pParents,WM_VEHICLECONTROL,SERVER_DOWN,0);
			AfxMessageBox(_T("GPS服务器要求你退出!"));
			return;
		}
		break;
	case SERVER_DOWN:
		{
			pSocket->SetLogin(FALSE);
			pSocket->Terminate();
		//	::SendMessage(pParents,WM_VEHICLECONTROL,SERVER_DOWN,0);
			AfxMessageBox(_T("GPS服务器关闭!"));
		}
		break;
	case MAPSERVER_OPENATLAS_SUCCEED:
		{
			m_bAtlasOpened = true;
			SetEvent(m_hAtlasOpened);
		}
		break;
	case MAPSERVER_OPENATLAS_FAILED:
		{
			m_bAtlasOpened = false;
			SetEvent(m_hAtlasOpened);
		}
		break;
	case MAPSERVER_OPENMAP_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_OPENMAP msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_OPENMAP));
			if(m_varHead.parray != nullptr)
			{
				SafeArrayDestroy(m_varHead.parray);
				m_varHead.parray = nullptr;
			}
			SAFEARRAYBOUND bound;
			bound.cElements = msg.dwTotalLen-sizeof(STRUCT_MAPSERVER_REPLY_OPENMAP); 
			bound.lLbound = 0;

			m_varHead.vt = VT_UI1 | VT_ARRAY;
			m_varHead.parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

			void* pDest;
			SafeArrayAccessData(m_varHead.parray, &pDest);
			memcpy(pDest,(BYTE*)lp+sizeof(STRUCT_MAPSERVER_REPLY_OPENMAP),bound.cElements); 
			SafeArrayUnaccessData(m_varHead.parray);

			SetEvent(m_hMapOpened);
		}
		break;
	case MAPSERVER_OPENMAP_FAILED:
		{
			SetEvent(m_hMapOpened);
		}
		break;
	case MAPSERVER_MAPDATA_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_MAPDATA msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_MAPDATA));

			EnterCriticalSection(&csData);

			if(m_varData.parray != nullptr)
			{
				SafeArrayDestroy(m_varData.parray);
				m_varData.parray = nullptr;
			}
			SAFEARRAYBOUND bound;
			bound.cElements = msg.dwTotalLen-sizeof(STRUCT_MAPSERVER_REPLY_MAPDATA); 
			bound.lLbound = 0;

			m_varData.vt = VT_UI1 | VT_ARRAY;
			m_varData.parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

			void* pDest;
			SafeArrayAccessData(m_varData.parray, &pDest);
			memcpy(pDest,(BYTE*)lp+sizeof(STRUCT_MAPSERVER_REPLY_MAPDATA),bound.cElements); 
			SafeArrayUnaccessData(m_varData.parray);
			
			LeaveCriticalSection(&csData);

			SetEvent(m_hMapData);
		}
		break;
	case MAPSERVER_MAPDATA_FAILED:
		{
			SetEvent(m_hMapData);
		}
		break;
	case MAPSERVER_GETMAPID_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_MAPID msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
			m_wMapId = msg.wMapId;
			
			SetEvent(m_hGetMapId);
		}
		break;
	case MAPSERVER_GETMAPID_FAILED:
		{
			SetEvent(m_hGetMapId);
		}
		break;
	case MAPSERVER_GETPARENTMAP_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_MAPID msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
			m_wParentMapId = msg.wMapId;
			
			SetEvent(m_hGetParentMap);
		}
		break;
	case MAPSERVER_GETPARENTMAP_FAILED:
		{
			SetEvent(m_hGetParentMap);
		}
		break;
	case MAPSERVER_GETSUBMAP_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_MAPID msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
			m_wSubMapId = msg.wMapId;
			
			SetEvent(m_hGetSubMap);
		}
		break;
	case MAPSERVER_GETSUBMAP_FAILED:
		{
			SetEvent(m_hGetSubMap);
		}
		break;
	case MAPSERVER_MATCHMAP_SUCCEED:
		{
			STRUCT_MAPSERVER_REPLY_MAPID msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_MAPSERVER_REPLY_MAPID));
			m_wMatchMapId = msg.wMapId;
			
			SetEvent(m_hMatchMap);
		}
		break;
	case MAPSERVER_MATCHMAP_FAILED:
		{
			SetEvent(m_hMatchMap);
		}
		break;
	case MAPSERVER_BALLOON:
		{
			STRUCT_SERVER_BALLOON msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_SERVER_BALLOON));
			
			CBalloon*pBalloon = new CBalloon(150,100);
			pBalloon->SetText(msg.cContent);
			pBalloon->SetlifeTime(4);
			pBalloon->CreateAndShow();					
		}
		break;
	default:
		break;
	}
}