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
#include <stdlib.h>

#include <afxwin.h>
#include <afxinet.h>
#include "HTTPSession.h"

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
	EnableStatusCallback(TRUE);

	m_pHTTPConnection = nullptr;
	m_pHTTPFile = nullptr;
}

BOOL CHTTPSession::Open(CString strURL, CString strParameter, DWORD dwFlags)
{
	DWORD dwRet = 0;
	try// check to see if this is a reasonable URL
	{
		CString strServer;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;
		if(AfxParseURL(strURL+strParameter, dwServiceType, strServer, strObject, nPort)==FALSE)
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
			//	VERIFY(this->EnableStatusCallback(TRUE));//Ӧ�ó���ʹ�� CInternetSession::EnableStatusCallback ����ȡ�й����ӵĽ��ȵ���Ϣ
		}

		m_pHTTPConnection = this->GetHttpConnection(strServer, nPort);
		if(m_pHTTPConnection==nullptr)
		{
			return FALSE;
		}
		m_pHTTPConnection->SetOption(INTERNET_OPTION_SECURITY_FLAGS, SECURITY_FLAG_IGNORE_UNKNOWN_CA);

		m_pHTTPFile = m_pHTTPConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, nullptr, 1, nullptr, nullptr, dwFlags|INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
		if(m_pHTTPFile==nullptr)
		{
			return FALSE;
		}

		CString strHeaders = _T("Accept: text/*\r\n");
		strHeaders += _T("User - Agent: HttpCall\r\n");
		strHeaders += _T("Accept - Language: en - us\r\n");
		m_pHTTPFile->AddRequestHeaders(strHeaders);

		do
		{
			m_pHTTPFile->SendRequest();
			m_pHTTPFile->QueryInfoStatusCode(dwRet);
			switch(dwRet)
			{
			case HTTP_STATUS_DENIED: // if access was denied, prompt the user for the password
				if(m_pHTTPFile->ErrorDlg(nullptr, ERROR_INTERNET_INCORRECT_PASSWORD, FLAGS_ERROR_UI_FLAGS_GENERATE_DATA|FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, nullptr)==ERROR_INTERNET_FORCE_RETRY)// if the user cancelled the dialog, bail out
					continue;
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
				break;
			case HTTP_STATUS_MOVED:
			case HTTP_STATUS_REDIRECT_METHOD:
			case HTTP_STATUS_REDIRECT:
			{
				DWORD dwLen;
				CString strNewLocation;
				m_pHTTPFile->QueryInfo(HTTP_QUERY_LOCATION, nullptr, &dwLen, 0);
				m_pHTTPFile->QueryInfo(HTTP_QUERY_LOCATION, strNewLocation.GetBufferSetLength(dwLen), &dwLen, 0);
				strNewLocation.ReleaseBuffer(dwLen);

				// ȷ���µ�ַ������������   
				CString strNewServer;
				AfxParseURL(strNewLocation, dwServiceType, strNewServer, strObject, nPort);

				// �����������������   
				if(strNewServer.IsEmpty())
				{
					// ���ӷ�������   
					CString strTempLocation = strServer;
					if(strNewLocation.Left(1)!="/")
						strTempLocation += "/";
					strTempLocation += strNewLocation;
					strNewLocation = strTempLocation;
				}

				//�����µĵ�ַ   
				strURL = strNewLocation;
			}
			break;
			}
		} while(dwRet!=HTTP_STATUS_OK);
	}
	catch(CInternetException* ex)
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

	return dwRet==HTTP_STATUS_OK;
}

void CHTTPSession::Close()
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
}

void CHTTPSession::OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	CStringA message;
	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		message.Format("Resolving name for %s", lpvStatusInformation);
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		message.Format("Resolved name for %s", lpvStatusInformation);
		break;
	case INTERNET_STATUS_HANDLE_CREATED:
		message.Format("Handle %8.8X created", dwContext);
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		message.Format("Connecting to socket address ");
		break;
	case INTERNET_STATUS_REQUEST_SENT:
		message.Format("Request sent");
		break;
	case INTERNET_STATUS_SENDING_REQUEST:
		message.Format("Sending request...");
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		message.Format("Connected to socket address");
		break;
	case INTERNET_STATUS_RECEIVING_RESPONSE:
		return;
		message.Format("Receiving response...");
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		message.Format("Response received");
		break;
	case INTERNET_STATUS_CLOSING_CONNECTION:
		message.Format("Closing the connection to the server");
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED:
		message.Format("Connection to the server closed");
		break;
	case INTERNET_STATUS_HANDLE_CLOSING:
		message.Format("Handle closed");
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		// See the CInternetSession constructor for details on INTERNET_FLAG_ASYNC.
		// The lpvStatusInformation parameter points at an INTERNET_ASYNC_RESULT 
		// structure, and dwStatusInformationLength contains the final completion 
		// status of the asynchronous function. If this is ERROR_INTERNET_EXTENDED_ERROR, 
		// the application can retrieve the server error information by using the 
		// Win32 function InternetGetLastResponseInfo. See the ActiveX SDK for more 
		// information about this function. 
		if(dwStatusInformationLength==sizeof(INTERNET_ASYNC_RESULT))
		{
			const INTERNET_ASYNC_RESULT* pResult = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
			message.Format("Request complete, dwResult = %8.8X, dwError = %8.8X",
				pResult->dwResult, pResult->dwError);
		}
		else
			message.Format("Request complete");
		break;
	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
	case INTERNET_STATUS_REDIRECT:
	default:
		message.Format("Unknown status: %d", dwInternetStatus);
		break;
	}

	TRACE("CHttpSession::OnStatusCallback: %s\n", message);
}

CString CHTTPSession::ReadContent() const
{
	DWORD dwRet;
	m_pHTTPFile->QueryInfoStatusCode(dwRet);

	CString strContent;
	char buffer[2000];
	DWORD dwLen = m_pHTTPFile->Read(buffer, 2000-1);
	while(dwLen>0)
	{
		buffer[dwLen] = 0;
		strContent += buffer;
		dwLen = m_pHTTPFile->Read(buffer, 2000-1);
	}

	return strContent;
}
