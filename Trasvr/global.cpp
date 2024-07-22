#include "stdafx.h"
#include "Global.h"
#include "ChildView.h"
#include "ClientForm.h"
#include "VehicleForm.h"

#include "..\Socket\ListenSocket.h"
#include "..\Socket\GpsSinkSocket.h" 
#include "..\Socket\XmlSinkSocket.h"

#include "..\Socket\ExStdCFunction.h"
#include "..\Coding\Instruction.h"
#include "..\Coding\Coder.h"

#include "..\Smcom\Global.h"
#include "..\Smcom\ComThread.h"

#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ���ջ��Ϣ�߳�
UINT DealRecvMsgThread(LPVOID lParam)
{
	CChildView* pChildView = (CChildView*)lParam;
	VERIFY(pChildView != nullptr);

	do
	{
		try
		{
			if(CComThread::g_recv_msg_queue.WaitForMsg() != WAIT_OBJECT_0)
				continue;
			
			DWORD length = 0;
			CMessageReceived* pMsg = (CMessageReceived*)CComThread::g_recv_msg_queue.PopMessage(length);
			if(pMsg != nullptr) 
			{
				pChildView->ProcessInMsg(pMsg,CComThread::g_recv_msg_queue.GetMsgCount());			
			}
		}
		catch(CException* e)
		{
			char buff[MAX_PATH+1];
			memset(buff, 0, MAX_PATH+1);
			e->GetErrorMessage(buff, MAX_PATH);
			e->Delete();
			CString str = "ActivitiesThread MemoryException: ";
			str += buff;
			Log(str);
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}while(pChildView->m_pSMSThread != nullptr || pChildView->m_pUDPThread != nullptr || pChildView->m_pTCPThread != nullptr || pChildView->m_pHTTPThread != nullptr);

	CComThread::g_recv_msg_queue.Clear();
	
	return 0;
}


///////////////////////////////////////////////////////////////////////

UINT SaveSendMsgThread(LPVOID lpParam) //��ôʱ������أ�
{
	CChildView* pChildView = (CChildView*)lpParam;
	VERIFY(pChildView != nullptr);

	ADOCG::_ConnectionPtr connectPtr = pChildView->m_connectPtr;
	
	do
	{
		try
		{
			if(CChildView::g_SendMsgSaveQueue.WaitForMsg() != WAIT_OBJECT_0)
				continue;

			DWORD length = 0;
			CMessageWillSend* pMsg = (CMessageWillSend*)CChildView::g_SendMsgSaveQueue.PopMessage(length);
			if(pMsg != nullptr) 
			{
				CString strTime;
				VariantTimeToYYMMDDhhmmString(COleDateTime::GetCurrentTime(), strTime);

				CString strSQL;
				switch(pMsg->m_wCommand)
				{
				case GPSCLIENT_VEHICLE_MESSAGE:
					{
						CString strMsg = pMsg->m_strData;
						strMsg.Replace("'", "''");

						CString strTime ;
						VariantTimeToYYMMDDhhmmString(COleDateTime::GetCurrentTime(), strTime);

						strSQL.Format(_T("INSERT INTO DualMsg (Vehicle,UserID,����,ʱ��,����) VALUES(%d,%d,%d,'%s','%s') "),pMsg->m_dwVehicle,pMsg->m_dwUser,1,strTime,strMsg);
					}
					break;
				default:
					strSQL.Format(_T("INSERT INTO SentCmd (Vehicle, UserID, ʱ��, ָ��) VALUES(%d, %d, '%s', %d)"),pMsg->m_dwVehicle,pMsg->m_dwUser,strTime, pMsg->m_wCommand);
				}

				try
				{
					_variant_t varRowsAff;
					connectPtr->BeginTrans();
					connectPtr->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);
					connectPtr->CommitTrans();
				}
				catch(_com_error &)
				{
					Log("Error SQL statement.");
					Log(strSQL);
					_bstr_t bstr;
					_variant_t var(short(0));

					if(connectPtr->Errors->Count > 0)
					{
						bstr = connectPtr->Errors->Item[var]->Description;
						Log_or_AfxMessageBox(bstr);
					}
					connectPtr->RollbackTrans();
				}
	
				delete pMsg;
				pMsg = nullptr;
			}
		}
		catch(CException* e)
		{
			char buff[MAX_PATH+1];
			memset(buff, 0, MAX_PATH+1);
			e->GetErrorMessage(buff, MAX_PATH);
			e->Delete();
			CString str = "ActivitiesThread MemoryException: ";
			str += buff;
			Log(str);
		}
	}while(pChildView->m_pSMSThread != nullptr || pChildView->m_pUDPThread != nullptr || pChildView->m_pTCPThread != nullptr || pChildView->m_pHTTPThread != nullptr);//��̫�������Ϊ��Ȼ4���߳��Ѿ������˵��洢��û�н���

	CChildView::g_SendMsgSaveQueue.Clear();
	return 0;
}

UINT SaveRecvMsgThread(LPVOID lpParam)//��ôʱ������أ�
{
	CChildView* pChildView = (CChildView*)lpParam;
	VERIFY(pChildView != nullptr);

	ADOCG::_ConnectionPtr connectPtr = pChildView->m_connectPtr;
	
	do
	{
		try
		{
			if(CChildView::g_RecvMsgSaveQueue.WaitForMsg() != WAIT_OBJECT_0)
				continue;
			
			DWORD length = 0;
			CMessageReceived* pMsg = (CMessageReceived*)CChildView::g_RecvMsgSaveQueue.PopMessage(length);
			if(pMsg != nullptr) 
			{
				 WriteReceivedMsgToDataBase(pMsg, connectPtr);

				delete pMsg;
				pMsg = nullptr;
			}
		}
		catch(CException* e)
		{
			char buff[MAX_PATH+1];
			memset(buff, 0, MAX_PATH+1);
			e->GetErrorMessage(buff, MAX_PATH);
			e->Delete();
			CString str = "ActivitiesThread MemoryException: ";
			str += buff;
			Log(str);
		}
	}while(pChildView->m_pSMSThread != nullptr || pChildView->m_pUDPThread != nullptr || pChildView->m_pTCPThread != nullptr || pChildView->m_pHTTPThread != nullptr);//��̫�������Ϊ��Ȼ4���߳��Ѿ������˵��洢��û�н���

	CChildView::g_RecvMsgSaveQueue.Clear();
	return 0;
}

void AcptGpsClient(SOCKET socket,DWORD dwParent)
{
	if(socket != INVALID_SOCKET)
	{
		CGpsSinkSocket* pClient = new CGpsSinkSocket();
		pClient->SetProcessFunction(ProcessGpsClientRecv);
		pClient->SetParent(dwParent);

		pClient->InitConnection(socket);
		if(pClient->CreateThreads() == FALSE) 
		{
			pClient->Close();
					
			delete pClient;
			pClient = nullptr;
		}
		else if(dwParent != nullptr)
		{
			EnterCriticalSection(&((CChildView*)dwParent)->csClientlist);
			((CChildView*)dwParent)->m_SinkSocketList.AddTail(pClient);
			LeaveCriticalSection(&((CChildView*)dwParent)->csClientlist);
		}
	}
}

void AcptXmlClient(SOCKET socket,DWORD dwParent)
{
	if(socket != INVALID_SOCKET)
	{
		CXmlSinkSocket* pClient = new CXmlSinkSocket();
		pClient->SetProcessFunction(ProcessXmlClientRecv);
		pClient->SetParent(dwParent);

		pClient->InitConnection(socket);
		if(pClient->CreateThreads() == FALSE) 
		{
			pClient->Close();
		
			delete pClient;
			pClient = nullptr;
		}
		else if(dwParent != nullptr)
		{
			EnterCriticalSection(&((CChildView*)dwParent)->csClientlist);
			((CChildView*)dwParent)->m_SinkSocketList.AddTail(pClient);
			LeaveCriticalSection(&((CChildView*)dwParent)->csClientlist);
		}
	}
}

void AcptWebClient(SOCKET socket,DWORD dwParent)
{
	if(socket == INVALID_SOCKET)
		return;

	try
	{
		CCoder::SParameter parameter;
		WORD wCmd;
		CString strcmd = "";

		int l = 201;
		char* buf = new char[l];
		memset(buf, 0, l);
		int len = recv(socket, buf, l, 0);
		if(len==SOCKET_ERROR || len==0 || len >=200) 
		{
			delete[] buf;
			closesocket(socket);
			return;
		}
		OLECHAR* szW = new OLECHAR[len/2+1];
		memset(szW, 0, (len/2+1)*2);
		memcpy(szW, buf, len);
		
		char* pStr = ConvertToAnsi(szW);

		delete[] buf;
		delete[] szW;

		using std::string;
		using namespace boost;			
		const std::string test_string = pStr;
		
		typedef tokenizer<char_separator<char> > Tok;
		char_separator<char> sep("|", nullptr, boost::keep_empty_tokens);
		Tok t(test_string, sep);
		Tok::iterator it = t.begin();
		if(it == t.end())
			goto EXITA;

		LPCSTR szField = nullptr;
		szField = it->c_str();
		if(_T('$') != szField[0])
			goto EXITA;


		CMessageWillSend* pMsg1 = new CMessageWillSend();

		it ++;
		if(it == t.end()) 
			goto EXITA;
		else
			pMsg1->m_dwUser = _ttoi (it->c_str());
		
		it ++;
		if(it == t.end())
			goto EXITA;
		else
			pMsg1->m_strUser = it->c_str();
		
		it ++;
		if(it == t.end())
			goto EXITA;
		SVehicle* pVhc =((CChildView*)dwParent)->m_vectVehicle.GetVehicleInfo(it->c_str());
		if(pVhc == nullptr)
			goto EXITA;
		else
			pMsg1->m_dwVehicle = pVhc->m_dwId;
		
		it ++;
		if(it == t.end())
			goto EXITA;
		DWORD IVUid = _ttoi (it->c_str());
		
		it ++;
		if(it == t.end())
			goto EXITA;

		wCmd = _ttoi (it->c_str());
		
		it ++;
		if(it != t.end())
			strcmd = it->c_str();

		CCoder* pCoder = CCoder::GetCoder(pVhc->m_strIVUType);
		if(pCoder != nullptr)
		{
			pMsg1->m_bSupportSMS = pCoder->IsSupportSMS();
			pMsg1->m_bSupportUDP = pCoder->IsSupportUDP();
			pMsg1->m_bSupportTCP = pCoder->IsSupportTCP();
		}
		else
			goto EXITA;

		switch(IVUid)
		{
		case 2:
			{
				switch(wCmd)
				{
				case 7000:
					{
						parameter.dwParam[0] = _ttoi (strcmd);
						break;
					}
				case 7001:
					break;
				case 7004:
					break;
				case 7010:
					{
						parameter.strParam[0] = strcmd;
						break;
					}
				default:
					{
						goto EXITA;
						break;
					}
				}
				break;
			}
		case 4:
			{
				switch(wCmd)
				{
				case 7004:
					break;
				case 7010:
					{
						parameter.strParam[0] = strcmd;
						break;
					}
				default:
					{
						goto EXITA;
						break;
					}
				}
				break;
			}
		default:
			{
				goto EXITA;
				break;
			}
		}
		
		parameter.m_password = pVhc->m_strIVUPwd;	
		if(pCoder->Encoding(wCmd,parameter,pMsg1->m_strData) == true)
		{
			CMessageWillSend* pMsg2 = new CMessageWillSend(pMsg1);
			((CChildView*)dwParent)->PutSendMsgToSaveQueue(pMsg2);
			
			if(pCoder->IsSupportUDP() == true)
			{
				((CChildView*)dwParent)->PutMsgToUDPSendQueue(pMsg1,0);
			}
			else if(pCoder->IsSupportTCP() == true)
			{
				((CChildView*)dwParent)->PutMsgToTCPSendQueue(pMsg1,0);
			}
			else if(pCoder->IsSupportSMS() == true)
			{
				((CChildView*)dwParent)->PutMsgToSMSSendQueue(pMsg1,0);
			}
			else
			{
				delete pMsg1;
				pMsg1 = nullptr;
			}

			send(socket, "SUCCED", 6, 0);
			closesocket(socket);
			return;
		}

EXITA:
		try
		{
			send(socket, "FAILED", 6, 0);

			delete[] pStr;
			pStr = nullptr;

			delete pMsg1;
			pMsg1 = nullptr;
			closesocket(socket);
		}
		catch(CException* ex)
		{
			ex->Delete();
		};
	}
	catch(CException* ex)
	{
		ex->Delete();
		send(socket, "FAILED", 6, 0);
		closesocket(socket);
	};
}

void ProcessGpsClientRecv(void* lp, CGpsSinkSocket* pSocket,CChildView* pChildView)
{
	try
	{
		if(lp == nullptr) 
			return;

		if(pSocket == nullptr)
			return;

		STRUCT_MSG_HEADER hdr;
		memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));

		switch(hdr.wCmdId)
		{
		case CLIENT_LOGIN:
			{
				STRUCT_CLIENT_LOGIN msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);	
	
				DWORD dwUserID, dwErr;
				if(pChildView->GetClientSocketByUser(msg.cName) != nullptr)
				{
					STRUCT_SERVER_REPLY_LOGIN* lp = new STRUCT_SERVER_REPLY_LOGIN(0X0004);
					lp->wCmdId = SERVER_LOGIN_FAILED;
					pSocket->SendMsg((char*)lp, sizeof(*lp));
				//	pSocket->IsTerminated();
				}
				else if(pChildView->VerifyUser(msg.cName, msg.cPass,&dwErr, pChildView->m_connectPtr, &dwUserID) == FALSE)
				{
					if(dwErr == 0) 
						dwErr = 0X0002;

					STRUCT_SERVER_REPLY_LOGIN* lp = new STRUCT_SERVER_REPLY_LOGIN(dwErr);
					lp->wCmdId = SERVER_LOGIN_FAILED;
					pSocket->SendMsg((char*)lp, sizeof(*lp));
				//	pSocket->IsTerminated();		
				}
				else
				{
					STRUCT_SERVER_REPLY_LOGIN* lp = new STRUCT_SERVER_REPLY_LOGIN();
					lp->wCmdId = SERVER_LOGIN_SUCCEED;
					lp->dwUserID = dwUserID;

					pSocket->SendMsg((char*)lp, sizeof(*lp));
					pSocket->SetUserName(msg.cName);
					pSocket->SetUserID(dwUserID);
					pSocket->SetLogin(TRUE);

					pChildView->AddClient(pSocket);
				}
			}
			break;
		case CLIENT_LOGOFF:
			{		
				if(pChildView != nullptr)
				{
					pChildView->PostMessage(WM_DELETECLIENT,(UINT)pSocket,0);
				//	pChildView->DeleteClient(pSocket);���ñ������Ľ����޷��˳�
				}
			}
			break;
		case CLIENT_PULSE:
			pSocket->ResetPulsCounter();
			break;
		case GPSCLIENT_CLEAR_WARNING:
			{
				CMessageReceived* pMsg = new CMessageReceived();
				pMsg->m_dwVehicle = hdr.dwVehicle;
				pMsg->m_wCommand = hdr.wCmdId;
				pChildView->BroadCastMsgToEveryClient(*pMsg);
				
				delete pMsg; // ��Ҫɾ��
				pMsg = nullptr;

				if(pChildView->m_pActiveWnd != nullptr && pChildView->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)))
				{
					((CVehicleForm*)pChildView->m_pActiveWnd)->SetInspectedVehicleWarningMark(pMsg->m_dwVehicle, FALSE,0,0.0,STATE_WARNING);
				}//ȷ���Ƿ�Ҫ�򳵻�����ֹͣ����ָ��
			}
			break;
		case GPSCLIENT_VEHICLE_CALL:
			{					
				if(pChildView->AskInspectOneVehicle(hdr.dwVehicle, pSocket) == TRUE)
				{
				}
			}
			break;
		case GPSCLIENT_VEHICLE_EREASE:
			{
				CVehicleMap::iterator it = pSocket->m_mapIdVehicle.find(hdr.dwVehicle);
				if(it != pSocket->m_mapIdVehicle.end())
				{
					VehicleStatus* pVhcStatus = (VehicleStatus*)it->second;
					pSocket->m_mapIdVehicle.erase(it);
					
					if(pChildView->m_pActiveWnd != nullptr && pChildView->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
					{
						((CVehicleForm*)pChildView->m_pActiveWnd)->RemoveVehicle(pSocket,pVhcStatus);
					}
					else if(pChildView->m_pActiveWnd != nullptr && pChildView->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CClientForm)) == TRUE)
					{
						((CClientForm*)pChildView->m_pActiveWnd)->ChangeValue(pSocket);
					}

					delete pVhcStatus;
					pVhcStatus = nullptr;
					
					//��ʵ�Ƿ���Ҫ�򳵻�����ֹͣ�ر���ָ��
				}
			}
			break;
		case GPSCLIENT_VEHICLE_TRACK:
			{
				if(pChildView->m_pActiveWnd != nullptr && pChildView->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
				{
					((CVehicleForm*)pChildView->m_pActiveWnd)->SetInspectedVehicleTrapMark(hdr.dwVehicle, TRUE, pSocket);
				}
			}
			break;
		case GPSCLIENT_VEHICLE_UNTRACK:
			{
				if(pChildView->m_pActiveWnd != nullptr && pChildView->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
				{
					((CVehicleForm*)pChildView->m_pActiveWnd)->SetInspectedVehicleTrapMark(hdr.dwVehicle, FALSE, pSocket);
				}
			}
			break;
		case GPSCLIENT_VEHICLE_DISPATCH:
			{
		/*		CMessageReceived* pMsg = new CMessageReceived();��Ҫ�ָ�
				pMsg->m_wCommand = GPSCLIENT_VEHICLE_DISPATCH;
				pMsg->m_dwVehicle = hdr.dwVehicle;

				pChildView->PutMsgToWriteQueun(pMsg);*/
			}
			break;
		case GPSCLIENT_VEHICLE_REPLAY:
			{
				STRUCT_GPSCLIENT_REPLAY msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);

				if(pSocket->GetReplaySet() != nullptr)
				{
					CString str;
					str.Format(_T("�ͻ������ڶԳ���%d���й켣�طţ�һ���ͻ���ͬʱֻ�ɶ�һ�������й켣�ط�!"), pSocket->GetReplayingVhcID());
					pSocket->SendStringToClient(pSocket->GetReplayingVhcID(), (LPTSTR)(LPCTSTR)str);
				}
				else
					pSocket->SetReplaySet(pChildView->m_connectPtr,msg.dwVehicle, msg.startTime, msg.endTime, msg.uDelay);
			}
			break;
		case GPSCLIENT_VEHICLE_REPLAY_STOP:
			{
				if(hdr.dwVehicle == pSocket->GetReplayingVhcID() && pSocket->GetReplaySet() != nullptr)
				{
					try
					{
						(pSocket->GetReplaySet())->Close();
						Sleep(500);
					}	
					catch(_com_error &)
					{
					}
				}
			}
			break;
		case GPSCLIENT_VEHICLE_MESSAGE:
			{
				STRUCT_GPSCLIENT_VEHICLE_MESSAGE msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);
				
				CMessageWillSend* pMsg1 = new CMessageWillSend();
				pMsg1->m_dwVehicle = msg.dwVehicle;
				pMsg1->m_wCommand = msg.wCmdId;
				pMsg1->m_strData = msg.msg;
				pMsg1->m_dwUser = pSocket->GetUserID();
				pMsg1->m_strUser = pSocket->GetUserName();
				pChildView->PutSendMsgToSaveQueue(pMsg1);
				
				SVehicle* pVhc = pChildView->m_vectVehicle.GetVehicleInfo(msg.dwVehicle);
				if(pVhc == nullptr)
					break;
				
				CMessageWillSend* pMsg2 = new CMessageWillSend(pMsg1);
				pMsg2->m_wCommand = CCoder::VHC_CMD_SND_MESSAGE;
				
				CString strIVU = pVhc->m_strIVUType;
				CCoder* pCoder = CCoder::GetCoder(strIVU);
				if(pCoder != nullptr)
				{						
					pMsg2->m_bSupportSMS = pCoder->IsSupportSMS();
					pMsg2->m_bSupportUDP = pCoder->IsSupportUDP();
					pMsg2->m_bSupportTCP = pCoder->IsSupportTCP();
				}
				else
				{
					delete pMsg2;
					pMsg2 = nullptr;
					break;
				}

				CCoder::SParameter parameter;
				parameter.strParam[0] = msg.msg;
				parameter.m_password = pVhc->m_strIVUPwd;

				CString strCmd;
				if(pCoder->Encoding(CCoder::VHC_CMD_SND_MESSAGE,parameter,strCmd) == true)
				{
					pMsg2->m_strData = strCmd;
				
					if(pCoder->IsSupportUDP() == true)
					{
						pChildView->PutMsgToUDPSendQueue(pMsg2,0);
					}
					else if(pCoder->IsSupportTCP() == true)
					{
						pChildView->PutMsgToTCPSendQueue(pMsg2,0);
					}
					else if(pCoder->IsSupportSMS() == true)
					{
						pChildView->PutMsgToSMSSendQueue(pMsg2,0);
					}
					else
					{
						delete pMsg2;
						pMsg2 = nullptr;
					}
				}
			}
			break;
		case GPSCLIENT_VEHICLE_COMMAND:
			{
				STRUCT_GPSCLIENT_VEHICLE_COMMAND msg;
				memcpy((char*)&msg, lp, hdr.dwTotalLen);
				CMessageWillSend* pMsg1 = new CMessageWillSend();
				pMsg1->m_dwVehicle = msg.dwVehicle;
				pMsg1->m_wCommand = msg.wCmd;
				pMsg1->m_strData = msg.cCmd;
				pMsg1->m_dwUser = pSocket->GetUserID();
				pMsg1->m_strUser = pSocket->GetUserName();
				
				CMessageWillSend* pMsg2 = new CMessageWillSend(pMsg1);
				pChildView->PutSendMsgToSaveQueue(pMsg1);
				
				SVehicle* pVhc = pChildView->m_vectVehicle.GetVehicleInfo(msg.dwVehicle);
				if(pVhc == nullptr)
				{
					delete pMsg2;
					pMsg2 = nullptr;
					break;
				}

				CString strIVU = pVhc->m_strIVUType;
				CCoder* pCoder = CCoder::GetCoder(strIVU);
				if(pCoder != nullptr)
				{
					pMsg2->m_bSupportSMS = pCoder->IsSupportSMS();
					pMsg2->m_bSupportUDP = pCoder->IsSupportUDP();
					pMsg2->m_bSupportTCP = pCoder->IsSupportTCP();
				}
				else
				{
					delete pMsg2;
					pMsg2 = nullptr;
					break;
				}

				if(pCoder->IsSupportUDP() == true)
				{
					pChildView->PutMsgToUDPSendQueue(pMsg2,0);
				}
				if(pCoder->IsSupportTCP() == true)
				{
					pChildView->PutMsgToTCPSendQueue(pMsg2,0);
				}
				else if(pCoder->IsSupportSMS() == true)
				{
					pChildView->PutMsgToSMSSendQueue(pMsg2,0);
				}
				else
				{
					delete pMsg2;
					pMsg2 = nullptr;
				}
			}
			break;
		default:
			break;
		}
	}
	catch(CException* ex)
	{
		ex->Delete();
		try
		{
			pChildView->PostMessage(WM_DELETECLIENT,(UINT)pSocket,0);
		//	pChildView->DeleteClient(pSocket);���ñ������Ľ����޷��˳�
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}
}

void ProcessXmlClientRecv(void* lp, CXmlSinkSocket* pSocket,CChildView* pChildView)
{
	CMarkup* pXmlParser = new CMarkup((LPCTSTR)lp);
	pXmlParser->ResetPos();
	try
	{
		if(!pXmlParser->FindElem("cmd"))
		{
			delete pXmlParser;
			pXmlParser = nullptr;
			return;
		}

		CString strCmd = pXmlParser->GetAttrib("name");
		if(strCmd.CompareNoCase("login") == 0)
		{
			CString name, pass;
			DWORD dwUserId = 111;
			if(pXmlParser->FindChildElem("name"))
			{
				pXmlParser->IntoElem();
				name = pXmlParser->GetData();
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("pass"))
			{
				pXmlParser->IntoElem();
				pass = pXmlParser->GetData();
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("ver"))
			{
				pXmlParser->IntoElem();
				CString ver = pXmlParser->GetData();
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("result"))
			{
				pXmlParser->IntoElem();
				pXmlParser->SetAttrib("code", 0);
				pXmlParser->SetData("��¼�ɹ�");
				pXmlParser->OutOfElem();
			}
			pXmlParser->ResetPos();
			if(pXmlParser->FindChildElem("name"))
			{
				pXmlParser->IntoElem();
				pXmlParser->SetAttrib("id", dwUserId);
				pXmlParser->OutOfElem();
			}
			STRUCT_CLIENT_LOGIN msg((LPTSTR)(LPCTSTR)name, (LPTSTR)(LPCTSTR)pass);
			ProcessGpsClientRecv((void*)&msg, pSocket,pChildView);
		}
		else if(strCmd.CompareNoCase("logout") == 0)
		{
			CString name;
			DWORD id;
			if(pXmlParser->FindChildElem("name"))
			{
				pXmlParser->IntoElem();
				name = pXmlParser->GetData();
				id = _ttoi (pXmlParser->GetAttrib("id"));
				pXmlParser->OutOfElem();
			}
			pChildView->PostMessage(WM_DELETECLIENT,(UINT)pSocket,0);
		//	pChildView->DeleteClient(pSocket);���ñ������Ľ����޷��˳�
		}
		else if(strCmd.CompareNoCase("track") == 0)
		{
			CString start,end;
			DWORD id;
			if(pXmlParser->FindChildElem("id"))
			{
				pXmlParser->IntoElem();
				id = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("start"))
			{
				pXmlParser->IntoElem();
				start = pXmlParser->GetData();
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("end"))
			{
				pXmlParser->IntoElem();
				end = pXmlParser->GetData();
				pXmlParser->OutOfElem();
			}
			STRUCT_GPSCLIENT_REPLAY msg(id, (LPTSTR)(LPCTSTR)start, (LPTSTR)(LPCTSTR)end, 1000);
			ProcessGpsClientRecv((void*)&msg, pSocket,pChildView);
		}
		else if(strCmd.CompareNoCase("trackstop") == 0)
		{
			CString start,end;
			DWORD id;
			if(pXmlParser->FindChildElem("id"))
			{
				pXmlParser->IntoElem();
				id = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			STRUCT_MSG_HEADER msg(id,GPSCLIENT_VEHICLE_REPLAY_STOP);
			ProcessGpsClientRecv((void*)&msg, pSocket,pChildView);
		}
		else if(strCmd.CompareNoCase("���λر�") == 0)
		{
			CString strCmd, vhcType;
			DWORD vhcId, cmdId;
			if(pXmlParser->FindChildElem("cmd"))
			{
				pXmlParser->IntoElem();
				cmdId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("vhc"))
			{
				pXmlParser->IntoElem();
				vhcId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}

			SVehicle* pVhc = pChildView->m_vectVehicle.GetVehicleInfo(vhcId);
			if(pVhc == nullptr) 
				return;				
			CCoder* pCoder = CCoder::GetCoder(pVhc->m_strIVUType);
			if(pCoder == nullptr)
				return;

			CCoder::SParameter parameter;
			parameter.m_password = pVhc->m_strIVUPwd;
			if(pCoder->Encoding(cmdId,parameter,strCmd) == true)
			{
				STRUCT_GPSCLIENT_VEHICLE_COMMAND msg2(vhcId, cmdId, (LPTSTR)(LPCTSTR)strCmd);
				ProcessGpsClientRecv((void*)&msg2, pSocket,pChildView);
			}
		}
		else if(strCmd.CompareNoCase("���λر�") == 0)
		{
			CString strCmd, vhcType;
			DWORD vhcId, cmdId;			
			
			CCoder::SParameter parameter;
			if(pXmlParser->FindChildElem("cmd"))
			{
				pXmlParser->IntoElem();
				cmdId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("vhc"))
			{
				pXmlParser->IntoElem();
				vhcId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("param"))
			{
				pXmlParser->IntoElem();
				parameter.dwParam[0] = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("param"))
			{
				pXmlParser->IntoElem();
				parameter.dwParam[1] = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}

			SVehicle* pVhc = pChildView->m_vectVehicle.GetVehicleInfo(vhcId);
			if(pVhc == nullptr) 
				return;

			CCoder* pCoder = CCoder::GetCoder(pVhc->m_strIVUType);
			if(pCoder == nullptr) 
				return;

			parameter.m_password = pVhc->m_strIVUPwd;
			if(pCoder->Encoding(cmdId,parameter,strCmd) == true)
			{
				STRUCT_GPSCLIENT_VEHICLE_COMMAND msg2(vhcId, cmdId, (LPTSTR)(LPCTSTR)strCmd);
				ProcessGpsClientRecv((void*)&msg2, pSocket,pChildView);
			}
		}
		else if(strCmd.CompareNoCase("��ʱ�ر�") == 0)
		{
			CCoder::SParameter parameter;

			CString strCmd, vhcType;
			DWORD vhcId, cmdId;			
			CMessageReceived msg;
			if(pXmlParser->FindChildElem("cmd"))
			{
				pXmlParser->IntoElem();
				cmdId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("vhc"))
			{
				pXmlParser->IntoElem();
				vhcId = _ttoi (pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("param"))
			{
				pXmlParser->IntoElem();
				parameter.dDate[0] = _ttof(pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}
			if(pXmlParser->FindChildElem("param"))
			{
				pXmlParser->IntoElem();
				parameter.dDate[1] = _ttof(pXmlParser->GetData());
				pXmlParser->OutOfElem();
			}

			SVehicle* pVhc = pChildView->m_vectVehicle.GetVehicleInfo(vhcId);
			if(pVhc == nullptr)
				return;				

			CCoder* pCoder = CCoder::GetCoder(pVhc->m_strIVUType);
			if(pCoder == nullptr)
				return;

			parameter.m_password = pVhc->m_strIVUPwd;
			if(pCoder->Encoding(cmdId,parameter,strCmd) == true)
			{
				STRUCT_GPSCLIENT_VEHICLE_COMMAND msg2(vhcId, cmdId, (LPTSTR)(LPCTSTR)strCmd);
				ProcessGpsClientRecv((void*)&msg2, pSocket,pChildView);
			}
		}
	}
	catch(CException* ex)
	{
		ex->Delete();
	}

	try
	{
		delete pXmlParser;
		pXmlParser = nullptr;
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
}

void ProcessSocketError(CGpsSinkSocket* pSocket,CChildView* pChildView)
{
	try
	{
		pChildView->PostMessage(WM_DELETECLIENT,(UINT)pSocket,0);
	//	pChildView->DeleteClient(pSocket);���ñ������Ľ����޷��˳�
	}
	catch(CException* ex)
	{
		ex->Delete();
	};
}