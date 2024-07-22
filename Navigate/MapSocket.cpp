#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSocket::CMapSocket()
{
	m_socket = INVALID_SOCKET;
	m_strPeerAddr = "";
	m_nPeerPort = 0;
	
	m_nThreadCount = 0;

	m_bCloseWait = TRUE;
	m_bStarting = FALSE;

	m_nTimeout = -1;

	m_hEventSend = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	m_hEventRecv = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	m_hEventGoOn = CreateEvent(nullptr, TRUE,  FALSE, nullptr);

	SetEvent(m_hEventGoOn);

	m_hThreadRecv = nullptr;
	m_hThreadSend = nullptr;
	m_hThreadWork = nullptr;

	InitializeCriticalSection(&m_csSend);
	InitializeCriticalSection(&m_csRecv);
	InitializeCriticalSection(&m_cs);
}

CMapSocket::~CMapSocket()
{
	if(IsRunning() == TRUE)
	{
		Stop();
	}

	CleanupLists();

	CloseHandle(m_hEventSend);
	CloseHandle(m_hEventRecv);
	CloseHandle(m_hEventGoOn);

	DeleteCriticalSection(&m_csSend);
	DeleteCriticalSection(&m_csRecv);
	DeleteCriticalSection(&m_cs);
}

BOOL CMapSocket::IsConnected()
{
	return m_socket != INVALID_SOCKET;
}

BOOL CMapSocket::IsShutingdown()
{
	return m_bShutingdown;
}

BOOL CMapSocket::IsRecvBufferEmpty()
{
	EnterCriticalSection(&m_csRecv);
	int nSize = m_listRecv.size();
	LeaveCriticalSection(&m_csRecv);

	return 0 == nSize;
}

BOOL CMapSocket::IsSendBufferEmpty()
{
	EnterCriticalSection(&m_csSend);
	int nSize = m_listSend.size();
	LeaveCriticalSection(&m_csSend);

	return 0 == nSize;
}

BOOL CMapSocket::Start(const char* pStartErrMsg = nullptr, unsigned int nSize = 0)
{
	if(IsRunning() == TRUE)
		return TRUE;

	if(m_bStarting == TRUE)
		return TRUE;

	m_bStarting = TRUE;

	CleanupLists();

	m_bShutingdown = FALSE;
	m_nLastActionTime = GetTickCount();

	m_hThreadSend = nullptr;
	m_hThreadWork = nullptr;

	m_nThreadCount = 0;		

	unsigned dwThread_ID;

	ResetEvent(m_hEventGoOn);

	m_hThreadRecv = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, this, 0, &dwThread_ID)
	if(m_hThreadRecv == nullptr)
	{
		if(pStartErrMsg && nSize > 0 && m_socket != INVALID_SOCKET)
		{
			send(m_socket, pStartErrMsg,nSize, 0);
		}

		if(m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		m_hThreadRecv = nullptr;
		m_nThreadCount = 0;

		SetEvent(m_hEventGoOn);
		m_bStarting = FALSE;
		return FALSE;
	}

	if(WaitForSingleObject(m_hEventGoOn, 30000) == WAIT_TIMEOUT)
	{
		goto outSend;
	}

	ResetEvent(m_hEventGoOn);	
	m_hThreadSend = (HANDLE)_beginthreadex(nullptr, 0, SendThread, this, 0, &dwThread_ID)
	if(m_hThreadSend == nullptr)
	{
outSend:
		if(pStartErrMsg && nSize > 0 && m_socket != INVALID_SOCKET)
			send(m_socket, pStartErrMsg,nSize, 0);

		if(m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		if(WaitForSingleObject(m_hThreadRecv, 30000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThreadRecv, 1);
			CloseHandle(m_hThreadRecv);
		}

		m_hThreadRecv = nullptr;
		m_hThreadSend = nullptr;

		m_nThreadCount = 0;

		SetEvent(m_hEventGoOn);
		m_bStarting = FALSE;
		return FALSE;
	}
	if(WaitForSingleObject(m_hEventGoOn, 30000) == WAIT_TIMEOUT)
	{
		goto outWork;
	}

	ResetEvent(m_hEventGoOn);	
	m_hThreadWork = (HANDLE)_beginthreadex(nullptr, 0, WorkThread, this, 0, &dwThread_ID);
	if(m_hThreadWork == nullptr)
	{
outWork:
		if(pStartErrMsg && nSize > 0 && m_socket != INVALID_SOCKET)
			send(m_socket, pStartErrMsg,nSize, 0);

		if(m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		if(WaitForSingleObject(m_hThreadRecv, 30000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThreadRecv, 1);
			CloseHandle(m_hThreadRecv);
		}
		if(WaitForSingleObject(m_hThreadSend, 30000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThreadSend, 1);
			CloseHandle(m_hThreadSend);
		}

		m_hThreadRecv = nullptr;
		m_hThreadSend = nullptr;
		m_hThreadWork = nullptr;

		m_nThreadCount = 0;

		SetEvent(m_hEventGoOn);
		m_bStarting = FALSE;
		return FALSE;
	}
	if(WaitForSingleObject(m_hEventGoOn, 30000) != WAIT_TIMEOUT)
	{
		if(pStartErrMsg && nSize > 0 && m_socket != INVALID_SOCKET)
			send(m_socket, pStartErrMsg,nSize, 0);

		if(m_socket != INVALID_SOCKET)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		if(WaitForSingleObject(m_hThreadWork, 3000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThreadWork, 1);
			CloseHandle(m_hThreadWork);
		}

		goto outWork;
	}


	m_bStarting = FALSE;
	return TRUE;
}

void CMapSocket::Stop()
{		
	while(m_bStarting == TRUE)
		Sleep(100);

	while(m_nThreadCount == 1 || m_nThreadCount == 2)
		Sleep(100);

	if(m_bShutingdown == TRUE)
		return;

	m_bShutingdown = TRUE;
	SetEvent(m_hEventRecv);  // cause Read return

	if(m_bCloseWait == TRUE)
		WaitForSingleObject(m_hEventGoOn, -1);

	if(m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_SEND);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	SetEvent(m_hEventRecv);
	SetEvent(m_hEventSend);

	if(WaitForSingleObject(m_hThreadRecv, 30000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThreadRecv, 21);
		CloseHandle(m_hThreadRecv);
	}

	SetEvent(m_hEventRecv);
	SetEvent(m_hEventSend);

	if(WaitForSingleObject(m_hThreadSend, 30000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThreadSend, 22);
		CloseHandle(m_hThreadSend);
	}

	SetEvent(m_hEventRecv);
	SetEvent(m_hEventSend);

	if(WaitForSingleObject(m_hThreadWork, 30000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThreadWork, 23);
		CloseHandle(m_hThreadWork);
	}

	m_hThreadRecv = nullptr;
	m_hThreadSend = nullptr;
	m_hThreadWork = nullptr;

	m_nThreadCount = 0;

	CleanupLists();

	m_bShutingdown = FALSE;
}

void CMapSocket::Disconnect()
{
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}

void CMapSocket::CleanupLists()
{
	EnterCriticalSection(&m_csSend);

	BUFFER_LIST::iterator it;
	for(it = m_listSend.begin(); it != m_listSend.end(); ++it)
		delete *it;
	m_listSend.clear();

	LeaveCriticalSection(&m_csSend);

	EnterCriticalSection(&m_csRecv);	

	for(it = m_listRecv.begin(); it != m_listRecv.end(); ++it)
		delete *it;

	m_listRecv.clear();

	LeaveCriticalSection(&m_csRecv);
}

BOOL CMapSocket::IsRunning()
{
	return m_nThreadCount != 0;
}

static unsigned __stdcall CMapSocket::RecvThread(void* pParam)
{
	CMapSocket* pWorker = (CMapSocket*)pParam;
	InterlockedIncrement(&pWorker->m_nThreadCount);

	int nBufferSize = 32*1024;
	char* pRecvBuffer = new char [nBufferSize];

	SetEvent(pWorker->m_hEventGoOn);

	while(pWorker->m_bShutingdown == FALSE)
	{
		int nRecv = recv(pWorker->m_socket, pRecvBuffer, nBufferSize, 0);
		if(nRecv == SOCKET_ERROR || nRecv == 0)
		{
			int nWSAErr = WSAGetLastError();
			TRACE(TEXT("Recv Exit WSAErr: %d\n"), nWSAErr);

			if(nWSAErr == WSAEMSGSIZE)
			{
				delete [] pRecvBuffer;
				nBufferSize *= 2;
				pRecvBuffer = new char [nBufferSize];

				if(pRecvBuffer)
					continue;
			}
			else if(nWSAErr == WSAEINTR)
				continue;

			break;
		}
		else
		{
			pWorker->m_nLastActionTime = GetTickCount();

			EnterCriticalSection(&pWorker->m_csRecv);
			CDataBuffer* pBuffer = new CDataBuffer(pRecvBuffer, nRecv);
			if(pBuffer)
				pWorker->m_listRecv.push_back(pBuffer);
			else
			{
				Sleep(500);
				CDataBuffer* pBuffer = new CDataBuffer(pRecvBuffer, nRecv);
				if(pBuffer)
					pWorker->m_listRecv.push_back(pBuffer);
				else
				{
					break;
				}
			}
			LeaveCriticalSection(&pWorker->m_csRecv);

			SetEvent(pWorker->m_hEventRecv);
		}
	}
	
	SetEvent(pWorker->m_hEventRecv); // for block read

	delete [] pRecvBuffer;

	if(pWorker->m_socket != INVALID_SOCKET)
	{
		shutdown(pWorker->m_socket, SD_SEND);
		closesocket(pWorker->m_socket);
		pWorker->m_socket = INVALID_SOCKET;
	}

	InterlockedDecrement(&pWorker->m_nThreadCount);

	return 0;
}

static unsigned __stdcall CMapSocket::SendThread(void* pParam)
{
	CDataBuffer* pDataBuffer;

	const int nSendBufferSize = 16*1024; 
	char* pSendBuffer = new char [nSendBufferSize];
	int nBufferBytes;

	CMapSocket* pWorker = (CMapSocket*)pParam;
	InterlockedIncrement(&pWorker->m_nThreadCount);

	SetEvent(pWorker->m_hEventGoOn);
	while(pWorker->m_socket != INVALID_SOCKET)
	{
		if(WaitForSingleObject(pWorker->m_hEventSend, 5000) == WAIT_TIMEOUT)
		{
			if(GetTickCount() - pWorker->m_nLastActionTime > pWorker->m_nTimeout)
				break;
			else
				continue;
		}

		do
		{
			pDataBuffer = nullptr;
			nBufferBytes = 0;

			BOOL bConti = TRUE;
			do
			{
				EnterCriticalSection(&pWorker->m_csSend);
				BUFFER_LIST::iterator it = pWorker->m_listSend.begin();
				if(it != pWorker->m_listSend.end())
				{
					pDataBuffer = *it;
					if(pDataBuffer->GetSize() <= nSendBufferSize - nBufferBytes)
					{
						memcpy(pSendBuffer + nBufferBytes, pDataBuffer->GetBuffer(), pDataBuffer->GetSize());
						nBufferBytes += pDataBuffer->GetSize();
						pDataBuffer->ReleaseBuffer();
						delete pDataBuffer;
						pDataBuffer = nullptr;
						pWorker->m_listSend.pop_front();						
					}
					else
						bConti = FALSE;
				}
				else
					bConti = FALSE;
				LeaveCriticalSection(&pWorker->m_csSend);
			}while(bConti);

			if(nBufferBytes != 0)
			{
				int nSent = send(pWorker->m_socket, pSendBuffer, nBufferBytes, 0);
				pWorker->m_nLastActionTime = GetTickCount();

				if(SOCKET_ERROR == nSent || 0 == nSent)
				{
					TRACE(TEXT("Send Exit WSAErr: %d\n"), WSAGetLastError());

					if(pDataBuffer = new CDataBuffer(pSendBuffer, nBufferBytes))
					{
						EnterCriticalSection(&pWorker->m_csSend);
						pWorker->m_listSend.push_front(pDataBuffer);
						LeaveCriticalSection(&pWorker->m_csSend);
					}

					break;
				}

				continue;
			}
			else if(nullptr != pDataBuffer)// just use the large dataBuffer
			{
				int nSent = 0;	
				if(pDataBuffer->GetSize())
				{
					nSent = send(pWorker->m_socket, pDataBuffer->GetBuffer(), pDataBuffer->GetSize(), 0);
					pWorker->m_nLastActionTime = GetTickCount();
					pDataBuffer->ReleaseBuffer();

					if(SOCKET_ERROR == nSent || 0 == nSent)
					{
						TRACE(TEXT("Send Exit WSAErr: %d\n"), WSAGetLastError());
						break;
					}
				}

				EnterCriticalSection(&pWorker->m_csSend);
				pWorker->m_listSend.pop_front();
				LeaveCriticalSection(&pWorker->m_csSend);

				delete pDataBuffer;
			}
			else
				break;
		}while(true);

		if(pWorker->m_bShutingdown == TRUE)
			break;
	}

	if(pWorker->m_socket != INVALID_SOCKET)
	{
		shutdown(pWorker->m_socket, SD_SEND);
		closesocket(pWorker->m_socket);
		pWorker->m_socket = INVALID_SOCKET;
	}

	InterlockedDecrement(&pWorker->m_nThreadCount);

	delete [] pSendBuffer;

	return 0;		
}


static unsigned __stdcall CMapSocket::WorkThread(void* pParam)
{
	CMapSocket* pWorker = (CMapSocket*)pParam;
	TDerivedWorker* pDerivedWorker = (TDerivedWorker*)pWorker;

	InterlockedIncrement(&pWorker->m_nThreadCount);

	SetEvent(pWorker->m_hEventGoOn);

	ResetEvent(pWorker->m_hEventGoOn);
	pDerivedWorker->ProcessData();
	SetEvent(pWorker->m_hEventGoOn);

	TRACE(TEXT("Work Exit WSAErr: %d\n"), WSAGetLastError());

	if(pWorker->m_socket != INVALID_SOCKET)
	{
		shutdown(pWorker->m_socket, SD_SEND);
		closesocket(pWorker->m_socket);
		pWorker->m_socket = INVALID_SOCKET;
	}

	InterlockedDecrement(&pWorker->m_nThreadCount);

	return 0;
}

void CMapSocket::ProcessData()
{
	if(m_socket != INVALID_SOCKET)
	{	
		shutdown(m_socket, SD_SEND);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	/***********************************************
	const int nBufferSize = 1024;

	char* pBuffer = new char[nBufferSize+2];
	if(!pBuffer)
	return;

	memset(pBuffer, 0, nBufferSize + 2);

	int nRead;
	while(nRead = Read(pBuffer, nBufferSize))
	{
	if(IsShutingdown())
	break;
	else
	Write(pBuffer, nRead);
	}

	delete pBuffer;
	*********************************************/
}

unsigned int CMapSocket::Read(char*pBuffer, const unsigned int nBufferSize, BOOL bBlock = TRUE)
{
	if(nullptr == pBuffer || nBufferSize == 0)
		return 0;		

	unsigned int nFreeSize = nBufferSize;
	char* pFree = pBuffer;

	while(true)
	{
		CDataBuffer* p;

		EnterCriticalSection(&m_csRecv);
		BUFFER_LIST::iterator it = m_listRecv.begin();
		if(it != m_listRecv.end())
			p = *it;
		else
			p = nullptr;
		LeaveCriticalSection(&m_csRecv);

		if(p)
		{
			if(p->GetSize() <= nFreeSize)
			{
				memcpy(pFree, p->GetBuffer(), p->GetSize());
				pFree += p->GetSize();
				nFreeSize -= p->GetSize();
				p->ReleaseBuffer();
				delete p;
				m_listRecv.pop_front();

				if(nFreeSize == 0)
					break;
			}
			else
			{ // the data size of the p object is larger than our free buffer
				memcpy(pFree, p->GetBuffer(), nFreeSize);
				p->ShrinkBuffer(nFreeSize);
				p->ReleaseBuffer();
				nFreeSize = 0;
				break;
			}
		}
		else
		{// p == null
			if(!bBlock)
				break;
			else if(IsConnected())
				WaitForSingleObject(m_hEventRecv, INFINITE);
			else
				break;
		}
	}


	return nBufferSize - nFreeSize;
}

BOOL CMapSocket::Write(const char*pBuffer, const unsigned int nBufferSize, BOOL bSendRightNow = TRUE)
{
	if(pBuffer == nullptr || nBufferSize == 0)
		return TRUE;

	EnterCriticalSection(&m_csSend);
	CDataBuffer* p = new CDataBuffer(pBuffer, nBufferSize);
	if(p != nullptr)
		m_listSend.push_back(p);
	LeaveCriticalSection(&m_csSend);

	if(bSendRightNow)
		SetEvent(m_hEventSend);

	return p != nullptr;
}

void CMapSocket::SendRightNow()
{
	SetEvent(m_hEventSend);
}

void CMapSocket::SetCloseWaitFlag(BOOL bWait = TRUE)
{
	m_bCloseWait = bWait;
}

unsigned long CMapSocket::GetTimeout()
{
	return m_nTimeout;
}

unsigned long CMapSocket::SetTimeout(unsigned long nNewVal)
{
	if(nNewVal == 0)
		nNewVal = -1;

	unsigned long nOldVal = m_nTimeout;
	m_nTimeout = nNewVal;

	return nOldVal;
}
