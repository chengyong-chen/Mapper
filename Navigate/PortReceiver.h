#pragma once

class CPort;

class __declspec(dllexport) CPortReceiver : public CPort
{
	DECLARE_DYNAMIC(CPortReceiver);
public:
	CPortReceiver(CFrameWnd* pParents);
	bool Open() override;
	void Close() override;
	void ReceiveMsg() override;

protected:
	CRITICAL_SECTION m_csCommunicationSync;

	HANDLE m_hKillEvent;
	HANDLE m_hWriteEvent;
	HANDLE m_hEventArray[3];

	HANDLE m_hPort;
	OVERLAPPED m_ov;
public:
	virtual void ReceiveChar(COMSTAT comstat);
	virtual void WriteChar()
	{
	}
	virtual bool ProcessRead(char RXBuff);
	static UINT ReceiverThread(LPVOID pParam);

public:
	static CString m_RMCResult;
	static CString m_GGAResult;

	static bool m_bSyndrome;

protected:
	static void ProcessErrorMessage(const char* ErrorText);
};
