#include "stdafx.h"
#include "Global.h"
#include "MapsvrDlg.h"
#include "ClientDlg.h"

#include "..\Socket\ListenSocket.h"
#include "..\Socket\MapSinkSocket.h" 

#include "..\Coding\Instruction.h"

void AcptClient(SOCKET socket,DWORD dwParent)
{
	if(socket != INVALID_SOCKET)
	{
		CMapSinkSocket* pClient = new CMapSinkSocket();
		pClient->SetCallbackFunction(ProcessClientRecv);
		pClient->SetParent(dwParent);

		pClient->InitConnection(socket);

		if(pClient->CreateThreads() == FALSE) 
		{
			pClient->Terminate();
			delete pClient;
			pClient = nullptr;
		}
		else if(dwParent != 0)
		{
			CMapsvrDlg* pMapsvrDlg = (CMapsvrDlg*)dwParent;
			EnterCriticalSection(&pMapsvrDlg->csClientlist);
			pMapsvrDlg->m_MapSinkList.AddTail(pClient);
			CString strUserCount;
			strUserCount.Format(_T("%d"),pMapsvrDlg->m_MapSinkList.GetCount());
			LeaveCriticalSection(&pMapsvrDlg->csClientlist);

			EnterCriticalSection(&pMapsvrDlg->csClientDlg);
			pMapsvrDlg->GetDlgItem(IDC_USERCOUNT)->SetWindowText(strUserCount);
			LeaveCriticalSection(&pMapsvrDlg->csClientDlg);
		}
	}
}

void ProcessClientRecv(void* lp, CMapSinkSocket* pSocket,CMapsvrDlg* pMapsvrDlg)
{
	try
	{
		if(lp == nullptr || pSocket == nullptr)
			return;

		STRUCT_MSG_HEADER hdr;
		memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));

		switch(hdr.dwCmdId)
		{
		case CLIENT_LOGIN:
			{
				STRUCT_CLIENT_LOGIN msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	

				STRUCT_SERVER_REPLY_LOGIN* lp = new STRUCT_SERVER_REPLY_LOGIN();
				if(FALSE)//��֤�ͻ��Ƿ�Ϸ�
				{
					lp->dwCmdId = SERVER_LOGIN_FAILED;
				}
				else
				{
					pSocket->m_strUser = msg.cName;
					lp->dwCmdId = SERVER_LOGIN_SUCCEED;
					if(pMapsvrDlg != nullptr)
					{
						pMapsvrDlg->InsertClient(pSocket);
					}
				}
				pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				pSocket->IsTerminated();
			}
			break;
		case CLIENT_LOGOFF:
			{
				STRUCT_CLIENT_LOGOFF msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	

				if(pMapsvrDlg != nullptr)
				{
					pMapsvrDlg->RemoveClient(pSocket);		
				}

				pSocket->DisConnect();
				delete pSocket;
				pSocket = nullptr;
			}
			break;		
		case MAPCLIENT_OPENATLAS:
			{
				STRUCT_MAPCLIENT_OPENATLAS msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				CString strAtlas = msg.cAtlas;

				STRUCT_MAPSERVER_REPLY_OPENATLAS* lp = new STRUCT_MAPSERVER_REPLY_OPENATLAS();

				BSTR bstrAtlas = strAtlas.AllocSysString();
				if(pMapsvrDlg->m_mapdocm.OpenAtlas(bstrAtlas) == S_OK)
				{
					lp->dwCmdId = MAPSERVER_OPENATLAS_SUCCEED;
				}
				else
				{
					lp->dwCmdId = MAPSERVER_OPENATLAS_FAILED;
				}

				pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				::SysFreeString(bstrAtlas); 
			}
			break;			
		case MAPCLIENT_OPENMAP:
			{
				STRUCT_MAPCLIENT_OPENMAP msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	

				STRUCT_MAPSERVER_REPLY_OPENMAP* lp = new STRUCT_MAPSERVER_REPLY_OPENMAP();
					
				DWORD nLength = 0;
				BYTE* pHead = pMapsvrDlg->m_mapdocm.OpenMap(msg.wMapId,nLength);
				if(pHead != nullptr)
				{
					lp->dwCmdId = MAPSERVER_OPENMAP_SUCCEED;
					lp->dwTotalLen += nLength;
					memcpy(pHead,(BYTE*)lp,sizeof(STRUCT_MAPSERVER_REPLY_OPENMAP));
					delete lp;
					pSocket->SendMessage((TCHAR*)pHead, sizeof(STRUCT_MAPSERVER_REPLY_OPENMAP)+nLength);

					POSITION pos = pSocket->vestigelist.GetHeadPosition();//���µĵ�ͼ����ҪΪSinkSocket���½��� �洢�Ѵ������������ID�б�
					while(pos != nullptr)
					{
						CMapSinkSocket::CLayerVestige* vestige = (CMapSinkSocket::CLayerVestige*)pSocket->vestigelist.GetNext(pos);
						delete vestige;
					}
					pSocket->vestigelist.RemoveAll();
					pSocket->m_wMapId = msg.wMapId;
					pMapsvrDlg->m_mapdocm.CreateVestigeList(msg.wMapId,pSocket->vestigelist);
				}
				else
				{
					lp->dwCmdId = MAPSERVER_OPENMAP_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
			}
			break;		
		case MAPCLIENT_MAPDATA:
			{
				STRUCT_MAPCLIENT_MAPDATA msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				
				STRUCT_MAPSERVER_REPLY_MAPDATA* lp = new STRUCT_MAPSERVER_REPLY_MAPDATA();
				VARIANT varData;
				if(pMapsvrDlg->m_mapdocm.GetMapData(msg.wMapId,msg.wZoom,msg.nLeft,msg.nTop,msg.nRight,msg.nBottom,&varData,pSocket->vestigelist) == S_OK)
				{
					lp->dwCmdId = MAPSERVER_MAPDATA_SUCCEED;
					lp->dwTotalLen += varData.parray->rgsabound[0].cElements;
					BYTE* pData = new BYTE[lp->dwTotalLen];
					memcpy(pData,(BYTE*)lp,sizeof(STRUCT_MAPSERVER_REPLY_MAPDATA));
	
					void* pSour;
					SafeArrayAccessData(varData.parray, &pSour);
					memcpy(pData+sizeof(STRUCT_MAPSERVER_REPLY_MAPDATA),(BYTE*)pSour,varData.parray->rgsabound[0].cElements); 
					SafeArrayUnaccessData(varData.parray);
							
					pSocket->SendMessage((TCHAR*)pData, lp->dwTotalLen);
					delete lp;

					POSITION pos = pSocket->vestigelist.GetHeadPosition();//Ϊ��һ�ε����ݻ�ȡ׼����ÿһ���vestige
					while(pos != nullptr)
					{
						CMapSinkSocket::CLayerVestige* vestige = (CMapSinkSocket::CLayerVestige*)pSocket->vestigelist.GetNext(pos);
						if(vestige != nullptr)
						{
							vestige->existentID.RemoveAll();
							vestige->lasttimeID->RemoveAll();
							CMap<DWORD,DWORD&,DWORD,DWORD&>* temporary = vestige->lasttimeID;
							vestige->lasttimeID = vestige->currentID;
							vestige->currentID = temporary;
						}
					}
				}
				else
				{
					lp->dwCmdId = MAPSERVER_MAPDATA_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
			}
			break;		
		case MAPCLIENT_GETMAPID:
			{
				STRUCT_MAPCLIENT_GETMAPID msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				CString strMap = msg.cMap;
				BSTR bstrMap = strMap.AllocSysString();
				WORD wMapId;
				if(pMapsvrDlg->m_mapdocm.GetMapId(bstrMap,&wMapId) == S_OK)
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETMAPID_SUCCEED;
					lp->wMapId = wMapId;
		
					pSocket->SendMessage((TCHAR*)lp, lp->dwTotalLen);
				}
				else
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETMAPID_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
				::SysFreeString(bstrMap); 
			}
			break;		
		case MAPCLIENT_GETPARENTMAP:
			{
				STRUCT_MAPCLIENT_GETPARENTMAP msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				WORD wMapId;
				if(pMapsvrDlg->m_mapdocm.GetParentMap(msg.wMapId,&wMapId) == S_OK)
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETPARENTMAP_SUCCEED;
					lp->wMapId = wMapId;
		
					pSocket->SendMessage((TCHAR*)lp, lp->dwTotalLen);
				}
				else
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETPARENTMAP_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
			}
			break;		
		case MAPCLIENT_GETSUBMAP:
			{
				STRUCT_MAPCLIENT_GETSUBMAP msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				WORD wMapId;
				if(pMapsvrDlg->m_mapdocm.GetSubMap(msg.wMapId,msg.fLng,msg.fLat,&wMapId) == S_OK)
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETSUBMAP_SUCCEED;
					lp->wMapId = wMapId;
		
					pSocket->SendMessage((TCHAR*)lp, lp->dwTotalLen);
				}
				else
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_GETSUBMAP_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
			}
			break;		
		case MAPCLIENT_MATCHMAP:
			{
				STRUCT_MAPCLIENT_MATCHMAP msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
				WORD wMapId;
				if(pMapsvrDlg->m_mapdocm.MatchMap(msg.fLng,msg.fLat,&wMapId) == S_OK)
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_MATCHMAP_SUCCEED;
					lp->wMapId = wMapId;
		
					pSocket->SendMessage((TCHAR*)lp, lp->dwTotalLen);
				}
				else
				{
					STRUCT_MAPSERVER_REPLY_MAPID* lp = new STRUCT_MAPSERVER_REPLY_MAPID();
					lp->dwCmdId = MAPSERVER_MATCHMAP_FAILED;
					pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
				}
			}
			break;		
		default:
			break;
		}
	}
	catch(CException* ex)
	{
		OutputDebugString(ex->m_strError+"\r\n");
		try
		{
			if(pMapsvrDlg != nullptr)
			{
				pMapsvrDlg->RemoveClient(pSocket);		


				EnterCriticalSection(&pMapsvrDlg->csClientlist);
				POSITION pos = pMapsvrDlg->m_MapSinkList.Find(pSocket);
				if(pos != nullptr && pMapsvrDlg != nullptr)
				{
					pMapsvrDlg->m_MapSinkList.RemoveAt(pos);
				}
				LeaveCriticalSection(&pMapsvrDlg->csClientlist);

			}

			pSocket->DisConnect();
			delete pSocket;

			if(pMapsvrDlg != nullptr && pMapsvrDlg->m_ClientDlg != nullptr)
			{
				EnterCriticalSection(&pMapsvrDlg->csClientDlg);
				int nCount = pMapsvrDlg->m_ClientDlg->m_ClientGrid.GetRows();
				if(pMapsvrDlg->m_ClientDlg->m_ClientGrid.GetTextMatrix(1, 0)==_T(""))
					nCount =0;
				else
					nCount= nCount-1;

				CString strUserCount;
				strUserCount.Format(_T("%d"),nCount);
				pMapsvrDlg->GetDlgItem(IDC_USERCOUNT)->SetWindowText(strUserCount);
				LeaveCriticalSection(&pMapsvrDlg->csClientDlg);
			}
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}
}
