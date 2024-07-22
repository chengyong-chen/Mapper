// tear.cpp : implements the TEAR console application
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1997-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
#include "stdafx.h"

#include <afxwin.h>
#include <afxinet.h>
#include "HTTPSession.h"
#include "Global.h"

#include "ComThread.h"

#include "../Coding/Coder.h"
#include "../Socket/ExStdCFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHTTPSession object

// TEAR wants to use its own derivative of the CInternetSession class
// just so it can implement an OnStatusCallback() override.

CHTTPSession::CHTTPSession(LPCTSTR pszAppName, int nMethod)
	: CInternetSession(pszAppName, 1, nMethod)
{
	m_pHTTPConnection = nullptr;
	m_pHTTPFile = nullptr;
	RecvThreadEnded = nullptr;
}

BOOL CHTTPSession::Open(CString strURL, CString strParameter)
{
	try// check to see if this is a reasonable URL
	{
		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;
		if(AfxParseURL(strURL+strParameter, dwServiceType, strServerName, strObject, nPort)==FALSE)
		{
			return FALSE;
		}
		if(dwServiceType!=INTERNET_SERVICE_HTTP)
		{
			return FALSE;
		}

		//	if (bProgressMode)
		{
			//	cerr << _T("Opening Internet...");
			//	VERIFY(this->EnableStatusCallback(TRUE));//应用程序使用 CInternetSession::EnableStatusCallback 来获取有关连接的进度的信息
		}

		m_pHTTPConnection = this->GetHttpConnection(strServerName, nPort);
		if(m_pHTTPConnection==nullptr)
		{
			return FALSE;
		}

		m_pHTTPFile = m_pHTTPConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, nullptr, 1, nullptr, nullptr, INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_NO_AUTO_REDIRECT|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
		if(m_pHTTPFile==nullptr)
		{
			return FALSE;
		}
		m_pHTTPFile->AddRequestHeaders(_T("Accept: text/*\r\nUser-Agent: MFC_Tear_Sample\r\n"));

		DWORD dwRet;
		do
		{
			m_pHTTPFile->SendRequest();
			m_pHTTPFile->QueryInfoStatusCode(dwRet);

			if(dwRet==HTTP_STATUS_DENIED)// if access was denied, prompt the user for the password
			{
				if(m_pHTTPFile->ErrorDlg(nullptr, ERROR_INTERNET_INCORRECT_PASSWORD, FLAGS_ERROR_UI_FLAGS_GENERATE_DATA|FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, nullptr)==ERROR_INTERNET_FORCE_RETRY)// if the user cancelled the dialog, bail out
				{
					continue;
				}
				else
				{
					if(m_pHTTPFile!=nullptr)
					{
						m_pHTTPFile->Close();
						delete m_pHTTPFile;
						m_pHTTPFile = nullptr;
					}
					if(m_pHTTPConnection!=nullptr)
					{
						m_pHTTPConnection->Close();
						delete m_pHTTPConnection;
						m_pHTTPConnection = nullptr;
					}

					return FALSE;
				}
			}
		} while(dwRet!=HTTP_STATUS_OK);

		if(AfxBeginThread(CHTTPSession::RecvMsgThread, (LPVOID)this)==nullptr)
		{
			AfxMessageBox(_T("启动Http接收发送消息线程启动失败！"));
		}
		else
		{
			return TRUE;
		}
	}
	catch(CInternetException*ex)
	{
		if(m_pHTTPFile!=nullptr)
		{
			m_pHTTPFile->Close();
			delete m_pHTTPFile;
			m_pHTTPFile = nullptr;
		}
		if(m_pHTTPConnection!=nullptr)
		{
			m_pHTTPConnection->Close();
			delete m_pHTTPConnection;
			m_pHTTPConnection = nullptr;
		}

		TCHAR szErr[1024];
		ex->GetErrorMessage(szErr, 1024);
		ex->Delete();
	}

	return FALSE;
}

void CHTTPSession::Close()
{
	if(m_pHTTPFile!=nullptr)
	{
		m_bNeedDate = false;

		if(RecvThreadEnded!=nullptr)
		{
			WaitForSingleObject(RecvThreadEnded, INFINITE);
			CloseHandle(RecvThreadEnded);
			RecvThreadEnded = nullptr;
		}

		m_pHTTPFile->Close();
		delete m_pHTTPFile;
		m_pHTTPFile = nullptr;
	}

	if(m_pHTTPConnection!=nullptr)
	{
		m_pHTTPConnection->Close();
		delete m_pHTTPConnection;
		m_pHTTPConnection = nullptr;
	}
}

void CHTTPSession::OnStatusCallback(DWORD /* dwContext */, DWORD dwInternetStatus, LPVOID /* lpvStatusInfomration */, DWORD /* dwStatusInformationLen */)
{
	//	if (!bProgressMode)
	//		return;

	//	if (dwInternetStatus == INTERNET_STATUS_CONNECTED_TO_SERVER)
	//		cerr << _T("Connection made!") << endl;
}

void CHTTPSession::StripTags(LPTSTR pszBuffer)
{
	static BOOL bInTag = FALSE;
	LPTSTR pszSource = pszBuffer;
	LPTSTR pszDest = pszBuffer;

	while(*pszSource!='\0')
	{
		if(bInTag)
		{
			if(*pszSource=='>')
				bInTag = FALSE;
			pszSource++;
		}
		else
		{
			if(*pszSource=='<')
				bInTag = TRUE;
			else
			{
				*pszDest = *pszSource;
				pszDest++;
			}
			pszSource++;
		}
	}
	*pszDest = '\0';
}

UINT CHTTPSession::RecvMsgThread(LPVOID lParam)
{
	CHTTPSession* pHTTPSession = (CHTTPSession*)lParam;
	VERIFY(pHTTPSession!=nullptr);

	pHTTPSession->RecvThreadEnded = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	pHTTPSession->m_bNeedDate = true;

	DWORD dwRet;
	pHTTPSession->m_pHTTPFile->SendRequest();
	pHTTPSession->m_pHTTPFile->QueryInfoStatusCode(dwRet);

	CString strLeft;
	while(pHTTPSession->m_bNeedDate==true&&pHTTPSession->m_pHTTPFile!=nullptr&&dwRet==HTTP_STATUS_OK)
	{
		CString str;
		if(pHTTPSession->m_pHTTPFile->ReadString(str)==TRUE)
		{
			str.TrimLeft();
			str = str+strLeft;
			const int pos = str.Find(_T("$GPSGD"));
			if(pos==-1)
			{
				strLeft.Empty();
				continue;
			}
			else if(pos!=0)
			{
				str = str.Mid(pos);
			}

			do
			{
				CString date;
				const int pos2 = str.Find(_T("$GPSGD"), 6);
				if(pos2!=-1)
				{
					date = str.Left(pos2);
					str = str.Mid(pos2);
				}
				else if(str.GetLength()>=68)
				{
					date = str.Left(68);
					str = str.Mid(68);
				}

				if(date.IsEmpty()==FALSE)
				{
					RecvHTTPPackage* pPack = new RecvHTTPPackage;
					pPack->len = date.GetLength();
					memcpy(pPack->buff, date.GetBuffer(), date.GetLength());
					date.ReleaseBuffer();

					pHTTPSession->DealRecvMsg(pPack);
				}
			} while(str.Find(_T("$GPSGD"))!=-1);

			strLeft = str;
		}
		else
		{
			pHTTPSession->m_pHTTPFile->SendRequest();
			pHTTPSession->m_pHTTPFile->QueryInfoStatusCode(dwRet);
		}
	}

	SetEvent(pHTTPSession->RecvThreadEnded);

	return 0;
}

UINT CHTTPSession::DealRecvMsg(RecvHTTPPackage* pPack) const
{
	try
	{
		if(pPack->len<=13||pPack->len>=pPack->lMaxLen)
		{
			delete pPack;
			pPack = nullptr;
			return 1;
		}

		CMessageReceived* pMsg = new CMessageReceived;
		pMsg->m_wCommand = 0XFF;
		pMsg->m_dwVehicle = 0;
		pMsg->m_Direct = 0;
		pMsg->m_Lon = 0.0;
		pMsg->m_Lat = 0.0;
		pMsg->m_Veo = 0.0;
		pMsg->m_strMessage = _T("");
		pMsg->m_dwStatus = 0;
		pMsg->m_varRevcTime = COleDateTime::GetCurrentTime();

		CCoder* pCoder = nullptr;
		if(pPack->buff[0]==_T('$')&&pPack->buff[4]==_T('G'))
		{
			pCoder = CCoder::GetCoder("STARG3_PS660");
		}

		if(pCoder==nullptr)
		{
			delete pPack;
			pPack = nullptr;

			delete pMsg;
			return 0;
		}
		CStringA ddd(pPack->buff);
		if(pCoder->DeCoding(ddd, *pMsg)==true)
		{
			pMsg->m_strIVUSN.MakeUpper();
			SVehicle* pVehicle = CComThread::FindVehicleInfo(pMsg->m_strIVUSN, true);// 按照车机序列号查找车辆
			if(pVehicle!=nullptr)
			{
				pMsg->m_dwVehicle = pVehicle->m_dwId;
				pMsg->m_strIVUSN = pVehicle->m_strIVUSN;

				CComThread::PushRecvMsgToQueue(pMsg);
				//需要加用于回发信息的IP地址和端口的信息
			}
			else
			{
				pMsg->m_dwVehicle = 0;

				delete pMsg;
				pMsg = nullptr;

				delete pPack;
				pPack = nullptr;
			}
		}
		else
		{
			if(pMsg->m_strMessage.IsEmpty()==FALSE)
			{
				CComThread::PutMsgToMessageWnd(pMsg->m_strMessage);
				Log_or_AfxMessageBox(pMsg->m_strMessage);
			}

			delete pMsg;
		}

		delete pPack;
		pPack = nullptr;
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	return 0;
}
