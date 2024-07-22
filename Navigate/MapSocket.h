class CMapSocket
{
public:
	CMapSocket();
private:
	~CMapSocket();

public:
	string m_strPeerAddr;
	unsigned short m_nPeerPort;
	
protected:		
	BUFFER_LIST m_listSend;4
	BUFFER_LIST m_listRecv;

	CRITICAL_SECTION m_csSend;X
	CRITICAL_SECTION m_csRecv;
	CRITICAL_SECTION m_cs;
	
	HANDLE m_hEventSend;
	HANDLE m_hEventRecv;
	HANDLE m_hEventGoOn;

private:	
	SOCKET m_socket;

	HANDLE m_hThreadRecv;
	HANDLE m_hThreadSend;
	HANDLE m_hThreadWork;;

	unsigned long m_nTimeout;
	unsigned long m_nLastActionTime;

	BOOL m_bStarting;
	BOOL m_bShutingdown;
	BOOL m_bCloseWait;

	long m_nThreadCount;

	BOOL IsConnected();
	BOOL IsShutingdown();
	BOOL IsRecvBufferEmpty();
	BOOL IsSendBufferEmpty();
	
	BOOL Start(const char* pStartErrMsg = nullptr, unsigned int nSize = 0);
	void Stop();
	void Disconnect();
	
private:
	void CleanupLists();
	BOOL IsRunning();

	static unsigned __stdcall RecvThread(void* pParam);
	static unsigned __stdcall SendThread(void* pParam);
	static unsigned __stdcall WorkThread(void* pParam);
	
public:	
	virtual void ProcessData() override;
	unsigned int Read(char*pBuffer, const unsigned int nBufferSize, BOOL bBlock = TRUE);
	BOOL Write(const char*pBuffer, const unsigned int nBufferSize, BOOL bSendRightNow = TRUE);
	void SendRightNow();
	void SetCloseWaitFlag(BOOL bWait = TRUE);

	unsigned long GetTimeout();
	unsigned long SetTimeout(unsigned long nNewVal);
};